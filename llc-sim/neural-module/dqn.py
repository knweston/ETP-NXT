import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

import random
from collections import deque

import numpy as np
from numpy import savetxt
from numpy import loadtxt
import tensorflow as tf
from tensorflow.keras import models
from tensorflow.keras import layers
from tensorflow.keras import optimizers
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.losses import Huber as huber_loss
from tensorflow.keras.initializers import GlorotNormal as GlorotNormal
from tensorflow.keras.initializers import HeNormal as HeNormal
from tensorflow.keras.initializers import HeUniform as HeUniform

# ==================================================================================== #
# CLASS DEEP Q NETWORK
# ==================================================================================== #
class DQN:
    def __init__(self, _num_features, _num_output, _app_name, _checkpt="none"):
        self.gamma        = 1.00    # discount factor
        self.alpha        = 0.0005  # learning rate
        self.eps          = 0.95    # exploration rate
        self.decay        = 1.00    # decay rate of eps
        self.STOP_EPS     = 0.05    # minimum eps
        self.UPDATE_INTVL = 10
        self.num_actions  = _num_output
        self.num_features = _num_features
        self.app_name     = _app_name
        
        self.avg_loss     = 0
        self.beta         = 0.05
            
        self.network        = self.createModel(self.num_features, self.num_actions, _checkpt)
        self.target_network = self.createModel(self.num_features, self.num_actions, _checkpt)
        self.best_model     = self.createModel(self.num_features, self.num_actions, _checkpt)

        self.replay_buffer = deque(maxlen=150)
        print("New model created\n")

        # statistics
        self.num_retrains = 0
        self.action_count = [0] * self.num_actions
        self.total_reward = 0.0
        self.max_hits     = 0
        self.noise        = 0

    def createModel(self, _num_input, _num_output, _checkpoint):
        model = models.Sequential()
        model.add(layers.Dense(128, activation='relu', input_shape=(_num_input,), kernel_initializer=GlorotNormal()))
        model.add(layers.Dense(_num_output, activation='linear'))
        if _checkpoint != "none":
            model.load_weights(_checkpoint)
        model.compile(optimizer=Adam(learning_rate=self.alpha), loss=huber_loss())
        return model
    
    def predict(self, state):
        action = 0
        if np.random.randint(0,100) < int(self.eps*100):
            action = np.random.randint(0,self.num_actions)
            print("random action", end="")
        else:
            x_input = [state]
            x_input = np.array(x_input)
            action = np.argmax(self.network.predict(x_input))
            print("learned action", end="")

        # collect statistics
        self.action_count[action] += 1
        
        return [action]

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

        self.replay_buffer.append((state, int(action), next_state, reward))
        self.total_reward += reward


    def get_training_parameters(self):
        if len(self.replay_buffer) <= 32:
            return len(self.replay_buffer), len(self.replay_buffer), 1
        elif len(self.replay_buffer) > 32 and len(self.replay_buffer) <= 256:
            return 32, 8, 2
        elif len(self.replay_buffer) > 256 and len(self.replay_buffer) <= 1024:
            return 64, 16, 2
        elif len(self.replay_buffer) > 1024 and len(self.replay_buffer) <= 2048:
            return 128, 32, 2
        else:
            return 256, 32, 3

    def retrain(self):
        # sanity check: there must be something in the replay buffer
        assert(len(self.replay_buffer))

        draw_size, train_batch_size, num_epochs = self.get_training_parameters()
        batch = random.sample(self.replay_buffer, draw_size)

        states_batch, actions_batch, next_states_batch, rewards_batch = map(np.array, zip(*batch))
        current_Q_batches = self.network.predict(states_batch)
        next_Q_batches = self.target_network.predict(next_states_batch)

        losses = []
        # update new Q-value: Q(s,a) = Q(s,a) + alpha * (reward + gamma * maxQ(s',a') - Q(s,a))
        for index in range(len(current_Q_batches)):
            td_loss = rewards_batch[index] - self.avg_loss + self.gamma * np.max(next_Q_batches[index]) - current_Q_batches[index][actions_batch[index]]
            losses.append(td_loss)
            current_Q_batches[index][actions_batch[index]] += self.alpha * td_loss
        
        # update average loss
        self.avg_loss += self.beta * sum(losses)/len(losses)
        
        self.network.fit(states_batch, current_Q_batches, batch_size=train_batch_size, \
                        verbose=True, epochs=num_epochs)
        self.num_retrains += 1

        if self.num_retrains % self.UPDATE_INTVL == 0:
            self.target_network.set_weights(self.network.get_weights())

    def save_model(self):
        weight_dir  = os.path.dirname(os.getcwd())+'/data/weights/'+str(self.gamma)+'-'+str(self.alpha)+'-'+str(self.decay)
        weight_file = weight_dir+'/'+self.app_name+'.hdf5'
        if not os.path.exists(weight_dir):
            os.makedirs(weight_dir)
        self.best_model.save(weight_file)
    
    def finalize_episode(self, num_hits):        
        # self.eps   *= self.decay
        # if int(self.eps*100) > int(self.STOP_EPS*100):
        #     if num_hits > self.max_hits:
        #         self.best_model.set_weights(self.target_network.get_weights())
        #         self.max_hits = num_hits
        #         # self.save_model()
        # else:
        #     self.eps = 0
        #     self.network.set_weights(self.best_model.get_weights())
        #     self.target_network.set_weights(self.best_model.get_weights())

        # self.total_reward = 0.0

        # if int(self.eps*100):
        #     if num_hits > self.max_hits:
        #         self.best_model.set_weights(self.target_network.get_weights())
        #         self.max_hits = num_hits
        #         # self.save_model()
        # else:
        #     self.network.set_weights(self.best_model.get_weights())
        #     self.target_network.set_weights(self.best_model.get_weights())

        self.total_reward = 0.0
        
# ==================================================================================== #
# CLASS CONVERGE DQN
# ==================================================================================== #
class ConvergeDQN:
    def __init__(self, _num_features, _num_output, _app_name, _batch, _checkpt="none"):
        self.gamma        = 0.98    # discount factor
        self.alpha        = 0.0001  # learning rate
        self.eps          = 1.00    # exploration rate
        self.decay        = 0.99    # decay rate of eps
        self.STOP_EPS     = 0.10    # minimum eps
        self.num_actions  = _num_output
        self.num_features = _num_features
        self.app_name     = _app_name
        self.batch_no     = _batch
            
        # self.network, load_status        = self.createModel(self.num_features, self.num_actions, _checkpt)
        # self.target_network, load_status = self.createModel(self.num_features, self.num_actions, _checkpt)

        self.network        = self.createModel(self.num_features, self.num_actions)
        self.target_network = self.createModel(self.num_features, self.num_actions)

        if _checkpt=="none" or not self.load_model(_checkpt):
            # Making the weights equal initially
            self.target_network.set_weights(self.network.get_weights()) 
            print("New Models Created")

        else:
            print("Saved Models Loaded")

        self.replay_buffer = deque(maxlen=150)
        

        # statistics
        self.num_retrains = 0
        self.action_count = [0] * self.num_actions
        self.total_reward = 0.0
        self.max_hits     = 0
        self.noise        = 0

        # Create action index map for DQN
        # if self.app_name.find("mix") != -1:
        #     action_t_rev  = [*range(15)]
        #     action_t      = list(reversed(action_t_rev))
        #     action_z_rev  = [*range(15,30)]
        #     action_z      = list(reversed(action_z_rev))
        #     action_nohash = [0] * 15
        
        # else:
        #     action_t_rev  = [*range(13)]
        #     action_t      = list(reversed(action_t_rev))
        #     action_z_rev  = [*range(13,26)]
        #     action_z      = list(reversed(action_z_rev))
        #     action_nohash = [0] * 13

        # self.action_list = []
        # if self.num_actions == 4:
        #     self.action_list = [action_t, action_z, action_t_rev, action_z_rev]

        # elif self.num_actions == 5:
        #     self.action_list = [action_t, action_z, action_t_rev, action_z_rev, action_nohash]
        #     # self.action_list = [action_nohash, action_t, action_z, action_t_rev, action_z_rev]

        self.log2_num_sets = 11
        self.action_list = []
        if self.num_actions == 24:
            for i in range(12):
                t = [*range(i, i+self.log2_num_sets)]
                t_rev = list(reversed(t))
                self.action_list.append(t)
                self.action_list.append(t_rev)
        
        elif self.num_actions == 20:
            for i in range(10):
                t = [*range(i, i+self.log2_num_sets)]
                t_rev = list(reversed(t))
                self.action_list.append(t)
                self.action_list.append(t_rev)

        else:
            print("unrecognized num_action: ", _num_output)
            exit(1)

        nohash_mapping = [0] * self.log2_num_sets
        self.action_list.insert(0, nohash_mapping)
        print("total actions: %d" % len(self.action_list))

    def createModel(self, _num_input, _num_output):
        model = models.Sequential()
        model.add(layers.Dense(32, activation='relu', input_shape=(_num_input,), kernel_initializer=HeUniform()))
        # model.add(layers.Dense(32, activation='relu'))
        model.add(layers.Dense(_num_output, activation='linear'))
        model.compile(optimizer=Adam(learning_rate=self.alpha), loss=huber_loss())
        return model
        
    
    def load_model(self, weight_dir=None):
        if weight_dir == None:
            weight_dir  = os.path.dirname(os.getcwd())+'/data/weights/dqn-'+str(self.gamma)+'-'+str(self.alpha)+'-'+str(self.decay)+'-b'+str(self.batch_no)
        target_weight  = weight_dir+'/'+self.app_name+'-target.hdf5'
        current_weight = weight_dir+'/'+self.app_name+'-current.hdf5'
        if os.path.exists(target_weight) and os.path.exists(current_weight):
            self.target_network.load_weights(target_weight)
            self.network.load_weights(target_weight)
            return True
        return False
    
    def predict(self, state, add_noise=False):
        action = 0
        random = 0
        if add_noise and (np.random.randint(0,100) < int(self.eps*100)):
            action = np.random.randint(0,self.num_actions)
            random = 1

        else:
            x_input = [state]
            x_input = np.array(x_input)
            prediction = self.network.predict(x_input)
            action = np.argmax(prediction)
            print("x_input: ", x_input)
            print("prediction: ", prediction)

        # collect statistics
        self.action_count[action] += 1
        return self.action_list[action], action, random

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

        self.replay_buffer.append((state, int(action), next_state, reward))
        self.total_reward += reward


    def get_training_parameters(self):
        if len(self.replay_buffer) <= 16:
            if len(self.replay_buffer) > 4:
                return len(self.replay_buffer), 4, 1
            else:
                return len(self.replay_buffer), len(self.replay_buffer), 1
        elif len(self.replay_buffer) > 16 and len(self.replay_buffer) < 128:
            return len(self.replay_buffer), 8, 1
        else:
            return 64, 8, 1


    def retrain(self):
        # sanity check: there must be something in the replay buffer
        assert(len(self.replay_buffer))

        draw_size, train_batch_size, num_epochs = self.get_training_parameters()
        batch = random.sample(self.replay_buffer, draw_size)

        states_batch, actions_batch, next_states_batch, rewards_batch = map(np.array, zip(*batch))
        current_Q_batches = self.network.predict(states_batch)
        next_Q_batches = self.target_network.predict(next_states_batch)

        # update new Q-value: Q(s,a) = Q(s,a) + alpha * (reward + gamma * maxQ(s',a') - Q(s,a))
        for index in range(len(current_Q_batches)):
            current_Q_batches[index][actions_batch[index]] += self.alpha * \
                                                              (rewards_batch[index] + \
                                                              self.gamma * np.max(next_Q_batches[index]) - \
                                                              current_Q_batches[index][actions_batch[index]])  
        
        self.network.fit(states_batch, current_Q_batches, batch_size=train_batch_size, \
                        verbose=True, epochs=num_epochs)
        self.num_retrains += 1


    def save_model(self):
        weight_dir  = os.path.dirname(os.getcwd())+'/data/weights/dqn-'+str(self.gamma)+'-'+str(self.alpha)+'-'+str(self.decay)+'-b'+str(self.batch_no)
        target_weight  = weight_dir+'/'+self.app_name+'-target.hdf5'
        current_weight = weight_dir+'/'+self.app_name+'-current.hdf5'
        if not os.path.exists(weight_dir):
            os.makedirs(weight_dir)
        self.target_network.save(target_weight)
        self.network.save(current_weight)
    

    def finalize_episode(self, num_hits, use_noise): 
        print("Total Reward: %0.6f" % self.total_reward)
        self.total_reward = 0.0

        if use_noise:
            self.eps *= self.decay
            self.target_network.set_weights(self.network.get_weights()) # update target_network every episode

        else:
            if num_hits > self.max_hits:
                # self.best_model.set_weights(self.target_network.get_weights())
                self.max_hits = num_hits
                self.save_model()
                print("New Optimal Mapping Found!")
            else:
                self.network = self.createModel(self.num_features, self.num_actions)
                self.target_network = self.createModel(self.num_features, self.num_actions)
                self.target_network.set_weights(self.network.get_weights()) 
                print("Reset Models")

        
