import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

import sys
import numpy as np
import tensorflow as tf
from tensorflow.keras import Model, layers, optimizers
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.initializers import GlorotNormal
from tensorflow.keras.regularizers import l2
from itertools import combinations, permutations


# ==================================================================================== #
# HELPER FUNCTIONS
# ==================================================================================== #
# This update target parameters slowly
# Based on rate `tau`, which is much less than one.
@tf.function
def update_target(target_weights, weights, tau):
    for (a, b) in zip(target_weights, weights):
        a.assign(b * tau + a * (1 - tau))

# ==================================================================================== #
# EXPLORATION NOISE
# ==================================================================================== #
class OUActionNoise:
    def __init__(self, mean, std_deviation, theta=0.15, dt=0.01, x_initial=None):
        self.theta = theta
        self.mean = mean
        self.std_dev = std_deviation
        self.dt = dt
        self.x_initial = x_initial
        self.reset()

    def __call__(self):
        # Formula taken from https://www.wikipedia.org/wiki/Ornstein-Uhlenbeck_process.
        x = (self.x_prev + self.theta * (self.mean - self.x_prev) * self.dt
                         + self.std_dev * np.sqrt(self.dt) * np.random.normal(size=self.mean.shape))
        # Store x into x_prev
        # Makes next noise dependent on current one
        self.x_prev = x
        return x

    def reset(self):
        if self.x_initial is not None:
            self.x_prev = self.x_initial
        else:
            self.x_prev = np.zeros_like(self.mean)

# ==================================================================================== #
# REPLAY BUFFER
# ==================================================================================== #
class Buffer:
    def __init__(self, state_dim, action_dim, capacity=10000, batch_size=32):
        self.buffer_capacity = capacity
        self.batch_size = batch_size
        
        # Its tells us num of times record() was called.
        self.buffer_counter = 0

        # Instead of list of tuples as the exp.replay concept go
        # We use different np.arrays for each tuple element
        self.state_buffer = np.zeros((self.buffer_capacity, state_dim))
        self.action_buffer = np.zeros((self.buffer_capacity, action_dim))
        self.reward_buffer = np.zeros((self.buffer_capacity, 1))
        self.next_state_buffer = np.zeros((self.buffer_capacity, state_dim))

    # Takes (s,a,r,s') obervation tuple as input
    def record(self, obs_tuple):
        # Set index to zero if buffer_capacity is exceeded,
        # replacing old records
        index = self.buffer_counter % self.buffer_capacity

        self.state_buffer[index] = obs_tuple[0]
        self.action_buffer[index] = obs_tuple[1]
        self.reward_buffer[index] = obs_tuple[2]
        self.next_state_buffer[index] = obs_tuple[3]

        self.buffer_counter += 1
    

# ==================================================================================== #
# CLASS DDPG_ENGINE
# ==================================================================================== #
class DDPG:
    def __init__(self, _num_features, _num_output, _app_name, _batch, _checkpt="none"):
        self.gamma        = 1.00
        self.tau          = 0.0001  # target network update rate
        self.beta         = 0.05    # discounting rate of average reward update
        self.avg_cri_loss = tf.convert_to_tensor(0.0)  # average reward (for non-episodic tasks)
        self.avg_act_loss = tf.convert_to_tensor(0.0)  # average reward (for non-episodic tasks)
        self.num_actions  = _num_output
        self.num_features = _num_features
        self.app_name     = _app_name
        self.batch_no     = _batch

        self.upper_bound  = self.num_actions - 1
        self.lower_bound  = 0
        self.total_reward = 0.0

        # temp variables
        self.num_retrains = 0
        self.eps          = 0.95
        self.max_hits     = 0

        self.target_actor   = self.create_actor()
        self.target_critic  = self.create_critic()
        self.current_actor  = self.create_actor()
        self.current_critic = self.create_critic()

        # Making the weights equal initially
        self.target_actor.set_weights(self.current_actor.get_weights())
        self.target_critic.set_weights(self.current_critic.get_weights())

        self.critic_optimizer = Adam(0.0001) # learning rate = 0.002
        self.actor_optimizer  = Adam(0.00005) # learning rate = 0.001
        self.noise = OUActionNoise(mean=np.zeros(1), std_deviation=float(0.2) * np.ones(1)) # exploration
        self.buffer = Buffer(state_dim=self.num_features, action_dim=1)

    # ============ INTERFACE FUNCTIONS ============ #
    def predict(self, state):
        tf_state = tf.expand_dims(tf.convert_to_tensor(state), 0)
        sampled_actions = tf.squeeze(self.current_actor(tf_state))

        if self.noise != None:
            noise = self.noise() * np.random.randint(-1,2)
        else:
            noise = 0
        sampled_actions = sampled_actions.numpy() + noise

        # We make sure action is within bounds
        legal_action = np.clip(sampled_actions, -1, 1)
        print("action: %.6f, noise: %.6f" % (legal_action, noise), end="")

        # scale output action
        legal_action = (np.squeeze(legal_action) + 1) * self.upper_bound/2
        # legal_action = np.squeeze(legal_action) * self.upper_bound
        return [legal_action]
    
    def add_sample(self, data):
        state      = []
        next_state = []
        action     = data[-2] / self.upper_bound    
        action     = data[-2] * 2 / self.upper_bound - 1    # scale output action
        reward     = data[-1]
        
        state_length = self.num_features
        for index in range(state_length):
            state.append(data[index])
        for index in range(state_length, 2*state_length):
            next_state.append(data[index])

        self.buffer.record([state, action, reward, next_state])
        self.total_reward += reward

        
    def retrain(self):
        self.learn()
        update_target(self.target_actor.variables, self.current_actor.variables, self.tau)
        update_target(self.target_critic.variables, self.current_critic.variables, self.tau)


    # Eager execution is turned on by default in TensorFlow 2. Decorating with tf.function allows
    # TensorFlow to build a static graph out of the logic and computations in our function.
    # This provides a large speed up for blocks of code that contain many small TensorFlow operations such as this one.
    @tf.function
    def update(self, state_batch, action_batch, reward_batch, next_state_batch, avg_cri_loss, avg_act_loss):
        # Training and updating Actor & Critic networks.
        # See Pseudo Code.
        with tf.GradientTape() as tape:
            target_actions = self.target_actor(next_state_batch, training=True)
            y = reward_batch + self.gamma * self.target_critic([next_state_batch, target_actions], training=True)
            
            critic_value = self.current_critic([state_batch, action_batch], training=True)
            critic_loss = tf.math.reduce_mean(tf.math.square(y - critic_value))
            critic_loss -= avg_cri_loss
            
        critic_grad = tape.gradient(critic_loss, self.current_critic.trainable_variables)
        self.critic_optimizer.apply_gradients(zip(critic_grad, self.current_critic.trainable_variables))

        with tf.GradientTape() as tape:
            actions = self.current_actor(state_batch, training=True)
            critic_value = self.current_critic([state_batch, actions], training=True)
            # Used `-value` as we want to maximize the value given
            # by the critic for our actions
            actor_loss = -tf.math.reduce_mean(critic_value)
            actor_loss -= avg_act_loss

        actor_grad = tape.gradient(actor_loss, self.current_actor.trainable_variables)
        self.actor_optimizer.apply_gradients(zip(actor_grad, self.current_actor.trainable_variables))

        return critic_loss, actor_loss

    # We compute the loss and update parameters
    def learn(self):
        # Get sampling range
        record_range = min(self.buffer.buffer_counter, self.buffer.buffer_capacity)
        # Randomly sample indices
        batch_indices = np.random.choice(record_range, self.buffer.batch_size)

        # Convert to tensors
        state_batch = tf.convert_to_tensor(self.buffer.state_buffer[batch_indices])
        action_batch = tf.convert_to_tensor(self.buffer.action_buffer[batch_indices])
        reward_batch = tf.convert_to_tensor(self.buffer.reward_buffer[batch_indices])
        reward_batch = tf.cast(reward_batch, dtype=tf.float32)
        next_state_batch = tf.convert_to_tensor(self.buffer.next_state_buffer[batch_indices])
        critic_loss, actor_loss = self.update(state_batch, action_batch, reward_batch, next_state_batch, \
                                              self.avg_cri_loss, self.avg_act_loss)
        self.avg_cri_loss += self.beta*critic_loss
        self.avg_act_loss += self.beta*actor_loss
        print("avg_critic_loss: %.6f. avg_actor_loss: %.6f" % \
                (tf.squeeze(self.avg_cri_loss).numpy(), \
                tf.squeeze(self.avg_act_loss).numpy()))
        print()        

    def finalize_episode(self, num_hits):
        print("Total Reward: %0.6f" % self.total_reward)
        self.total_reward = 0.0
        if num_hits > self.max_hits:
            self.save_model()
            self.max_hits = num_hits
            print("New Optimal Mapping Found!")
        # else:
            # self.load_model()

    # ============ HELPER FUNCTIONS ============ #
    def create_actor(self):
        initializer = tf.random_uniform_initializer(minval=-0.003, maxval=0.003)
        inputs = layers.Input(shape=(self.num_features,))
        out = layers.Dense(300, activation="relu", kernel_initializer=GlorotNormal())(inputs)
        out = layers.Dense(400, activation="relu", kernel_initializer=GlorotNormal())(out)
        outputs = layers.Dense(1, activation="tanh", kernel_initializer=initializer)(out)
        model = Model(inputs, outputs)
        return model

    def create_critic(self):
        # State as input
        state_input = layers.Input(shape=(self.num_features))
        state_out = layers.Dense(32, activation="relu", kernel_initializer=GlorotNormal())(state_input)
        state_out = layers.Dense(64, activation="relu", kernel_initializer=GlorotNormal())(state_out)

        # Action as input
        action_input = layers.Input(shape=(1))  # continuous action space => single output
        action_out = layers.Dense(64, activation="relu")(action_input)

        # Both are passed through separate layer before concatenating
        # Output = Q-value of (S,A) input
        concat = layers.Concatenate()([state_out, action_out])
        out = layers.Dense(300, activation="relu", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(concat)
        out = layers.Dense(400, activation="relu", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(out)
        outputs = layers.Dense(1, activation="linear", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(out)  

        # Outputs single value for give state-action
        model = Model([state_input, action_input], outputs)
        return model

    def save_model(self):
        weight_dir  = os.path.dirname(os.getcwd())+'/data/weights/'+str(self.tau)+'_b'+str(self.batch_no)
        actor_weight  = weight_dir+'/'+self.app_name+'-actor.hdf5'
        critic_weight = weight_dir+'/'+self.app_name+'-critic.hdf5'
        if not os.path.exists(weight_dir):
            os.makedirs(weight_dir)
        
        # self.target_actor.compile(optimizer=self.actor_optimizer)
        # self.target_critic.compile(optimizer=self.critic_optimizer)

        self.target_actor.save(actor_weight)
        self.target_critic.save(critic_weight)

    def load_model(self):
        weight_dir         = os.path.dirname(os.getcwd())+'/data/weights/'+str(self.tau)+'_b'+str(self.batch_no)
        actor_weight       = weight_dir+'/'+self.app_name+'-actor.hdf5'
        critic_weight      = weight_dir+'/'+self.app_name+'-critic.hdf5'
        self.target_actor  = tf.keras.models.load_model(actor_weight)
        self.target_critic = tf.keras.models.load_model(critic_weight)

        # Making the weights equal
        self.current_actor.set_weights(self.target_actor.get_weights())
        self.current_critic.set_weights(self.target_critic.get_weights())




# ==================================================================================== #
# CLASS Convergence DDPG
# ==================================================================================== #
class ConvergeDDPG:
    def __init__(self, _num_features, _num_output, _app_name, _batch, _checkpt="none"):
        self.gamma        = 0.99
        self.tau          = 0.01   # target network update rate (small model = 0.0001)
        self.num_features = _num_features
        self.app_name     = _app_name
        self.batch_no     = _batch
        self.total_reward = 0.0

        # temp variables
        self.num_retrains = 0
        self.eps          = 0.95
        self.max_hits     = 0

        self.current_actor  = self.create_actor()
        self.current_critic = self.create_critic()

        if _checkpt == "none" or not self.load_model(_checkpt):
            self.target_actor   = self.create_actor()
            self.target_critic  = self.create_critic()

            # Making the weights equal initially
            self.target_actor.set_weights(self.current_actor.get_weights())
            self.target_critic.set_weights(self.current_critic.get_weights())
            print("New Models Created")
            
        else:
            print("Saved Models Loaded")

        self.critic_optimizer = Adam(0.0001)   # learning rate   (small model = 0.0002)
        self.actor_optimizer  = Adam(0.00005)  # learning rate   (small model = 0.0001)
        self.noise = OUActionNoise(mean=np.zeros(1), std_deviation=float(0.2) * np.ones(1)) # exploration
        self.buffer = Buffer(state_dim=self.num_features, action_dim=1)

        self.num_groups   = 6
        self.total_groups = 9
        self.action_list = list(permutations(range(self.total_groups), self.num_groups)) # 12P6
        nohash_mapping = tuple([0] * self.num_groups)
        self.action_list.insert(0, nohash_mapping)
        self.upper_bound = len(self.action_list) - 1
        self.lower_bound = 0

    # ============ INTERFACE FUNCTIONS ============ #
    def predict(self, state, add_noise=False):
        tf_state = tf.expand_dims(tf.convert_to_tensor(state), 0)
        sampled_actions = tf.squeeze(self.current_actor(tf_state))
        
        if add_noise:
            noise = self.noise()[0] * np.random.randint(-1,2)
        else:
            noise = 0
        sampled_actions = sampled_actions.numpy() + noise

        # We make sure action is within bounds
        legal_action = np.clip(sampled_actions, -1, 1)        

        # scale output action
        scaled_action = (np.squeeze(legal_action) + 1) * self.upper_bound/2
        print("legal action: %.6f, noise: %.6f, scaled_action: %d" % (legal_action, noise, int(scaled_action)))

        return self.action_list[int(scaled_action)], legal_action, noise
    
    def add_sample(self, data):
        state      = []
        next_state = []
        action     = data[-2]
        reward     = data[-1]
        
        state_length = self.num_features
        for index in range(state_length):
            state.append(data[index])
        for index in range(state_length, 2*state_length):
            next_state.append(data[index])

        self.buffer.record([state, action, reward, next_state])
        self.total_reward += reward

        
    def retrain(self):
        self.learn()
        update_target(self.target_actor.variables, self.current_actor.variables, self.tau)
        update_target(self.target_critic.variables, self.current_critic.variables, self.tau)


    # Eager execution is turned on by default in TensorFlow 2. Decorating with tf.function allows
    # TensorFlow to build a static graph out of the logic and computations in our function.
    # This provides a large speed up for blocks of code that contain many small TensorFlow operations such as this one.
    @tf.function
    def update(self, state_batch, action_batch, reward_batch, next_state_batch):
        # Training and updating Actor & Critic networks.
        # See Pseudo Code.
        with tf.GradientTape() as tape:
            target_actions = self.target_actor(next_state_batch, training=True)
            y = reward_batch + self.gamma * self.target_critic([next_state_batch, target_actions], training=True)
            
            critic_value = self.current_critic([state_batch, action_batch], training=True)
            critic_loss = tf.math.reduce_mean(tf.math.square(y - critic_value))
            
        critic_grad = tape.gradient(critic_loss, self.current_critic.trainable_variables)
        self.critic_optimizer.apply_gradients(zip(critic_grad, self.current_critic.trainable_variables))

        with tf.GradientTape() as tape:
            actions = self.current_actor(state_batch, training=True)
            critic_value = self.current_critic([state_batch, actions], training=True)
            # Used `-value` as we want to maximize the value given
            # by the critic for our actions
            actor_loss = -tf.math.reduce_mean(critic_value)

        actor_grad = tape.gradient(actor_loss, self.current_actor.trainable_variables)
        self.actor_optimizer.apply_gradients(zip(actor_grad, self.current_actor.trainable_variables))

        return critic_loss, actor_loss

    # We compute the loss and update parameters
    def learn(self):
        # Get sampling range
        record_range = min(self.buffer.buffer_counter, self.buffer.buffer_capacity)
        # Randomly sample indices
        if record_range < self.buffer.batch_size:
            batch_indices = np.random.choice(record_range, self.buffer.batch_size)
        else:
            batch_indices = np.random.choice(record_range, self.buffer.batch_size, replace=False)

        # Convert to tensors
        state_batch = tf.convert_to_tensor(self.buffer.state_buffer[batch_indices])
        action_batch = tf.convert_to_tensor(self.buffer.action_buffer[batch_indices])
        reward_batch = tf.convert_to_tensor(self.buffer.reward_buffer[batch_indices])
        reward_batch = tf.cast(reward_batch, dtype=tf.float32)
        next_state_batch = tf.convert_to_tensor(self.buffer.next_state_buffer[batch_indices])
        critic_loss, actor_loss = self.update(state_batch, action_batch, reward_batch, next_state_batch)
        print("critic_loss: %.6f. actor_loss: %.6f" % \
                (tf.squeeze(critic_loss).numpy(), \
                tf.squeeze(actor_loss).numpy()))
        print()        

    def finalize_episode(self, num_hits, use_noise):
        print("Total Reward: %0.6f" % self.total_reward)
        self.total_reward = 0.0
        if (not use_noise) and (num_hits > self.max_hits):
            self.save_model()
            self.max_hits = num_hits
            print("New Optimal Mapping Found!")


    # ============ HELPER FUNCTIONS ============ #
    def create_actor(self):
        initializer = tf.random_uniform_initializer(minval=-0.003, maxval=0.003)
        inputs = layers.Input(shape=(self.num_features,))
        out = layers.Dense(300, activation="relu", kernel_initializer=GlorotNormal())(inputs)
        out = layers.Dense(400, activation="relu", kernel_initializer=GlorotNormal())(out)
        outputs = layers.Dense(1, activation="tanh", kernel_initializer=initializer)(out)
        model = Model(inputs, outputs)
        return model

    def create_critic(self):
        # State as input
        state_input = layers.Input(shape=(self.num_features))
        state_out = layers.Dense(64, activation="relu", kernel_initializer=GlorotNormal())(state_input)
        state_out = layers.Dense(128, activation="relu", kernel_initializer=GlorotNormal())(state_out)

        # Action as input
        action_input = layers.Input(shape=(1))  # continuous action space => single output
        action_out   = layers.Dense(64, activation="relu")(action_input)
        action_out   = layers.Dense(128, activation="relu")(action_out)

        # Both are passed through separate layer before concatenating
        # Output = Q-value of (S,A) input
        concat = layers.Concatenate()([state_out, action_out])

        out = layers.Dense(300, activation="relu", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(concat)
        out = layers.Dense(400, activation="relu", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(out)
        outputs = layers.Dense(1, activation="linear", kernel_initializer=GlorotNormal(), kernel_regularizer=l2(0.01))(out)  

        # Outputs single value for give state-action
        model = Model([state_input, action_input], outputs)
        return model


    def save_model(self):
        weight_dir  = os.path.dirname(os.getcwd())+'/data/weights/'+str(self.tau)+'-b'+str(self.batch_no)
        actor_weight  = weight_dir+'/'+self.app_name+'-actor.hdf5'
        critic_weight = weight_dir+'/'+self.app_name+'-critic.hdf5'
        if not os.path.exists(weight_dir):
            os.makedirs(weight_dir)
        
        self.target_actor.compile(optimizer=self.actor_optimizer)
        self.target_critic.compile(optimizer=self.critic_optimizer)
        self.target_actor.save(actor_weight)
        self.target_critic.save(critic_weight)


    def load_model(self, weight_dir=None):
        if weight_dir == None:
            weight_dir = os.path.dirname(os.getcwd())+'/data/weights/'+str(self.tau)+'-b'+str(self.batch_no)
        
        actor_weight  = weight_dir+'/'+self.app_name+'-actor.hdf5'
        critic_weight = weight_dir+'/'+self.app_name+'-critic.hdf5'

        if os.path.exists(actor_weight) and os.path.exists(critic_weight):
            self.target_actor  = tf.keras.models.load_model(actor_weight)
            self.target_critic = tf.keras.models.load_model(critic_weight)

            # Making the weights equal
            self.current_actor.set_weights(self.target_actor.get_weights())
            self.current_critic.set_weights(self.target_critic.get_weights())
            return True
        
        else:
            return False

