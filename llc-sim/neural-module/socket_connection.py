import os
import socket
from itertools import combinations, permutations
from copy import deepcopy
from random import shuffle

# ==================================================================================== #
# CLASS PREDICTION_SERVER
# ==================================================================================== #
class SocketConnection:
    def __init__(self, pred_engine):
        self.prediction_engine  = pred_engine
        self.done_episodes      = 0
        self.num_episodes       = 1
        self.complete_signal    = 0
        self.buffer_size        = 16*1024*1024
        
        if pred_engine.num_features >= 1024:
            self.MAX_STATE_PACKETS  = int(pred_engine.num_features / 1024)
        else:
            self.MAX_STATE_PACKETS  = 1
        self.MAX_SAMPLE_PACKETS = int(self.MAX_STATE_PACKETS * 2 + 2)

        # initialize action index table
        # self.ACTION_INDEX_MAP = list(permutations(range(8), 6))
        # nohash_mapping = (0,0,0,0,0,0)
        # self.ACTION_INDEX_MAP = list(permutations(range(8), 5))
        # nohash_mapping = (0,0,0,0,0)
        # self.ACTION_INDEX_MAP = list(permutations(range(7), 4))
        # nohash_mapping = (0,0,0,0)

        # self.ACTION_INDEX_MAP.insert(0, nohash_mapping)


    # start connection with client
    def start_connection(self, port_no, ipaddress=''):
        # create a socket object
        s = socket.socket()
        print("Socket successfully created")

        # max wait time = 1200s
        s.settimeout(1200)

        # reserve a port on your computer in our
        # case it is 12345 but it can be anything
        port = port_no

        # next bind to the port
        # we have not typed any ip in the ip field
        # instead we have input an empty string
        # this makes the server listen to requests
        # coming from other computers on the network
        s.bind((ipaddress, port))
        print("socket binded to %s" % (port))

        # put the socket into listening mode
        s.listen(5)
        print("socket is listening")
        return s
    
    # process client request
    def process_request(self, request, connection):
        
        if len(request) == 0:
            return
        
        elif request == "make prediction":
            reply = "Request received: make inference"
            connection.send(reply.encode())
            state = []
            for packet in range(self.MAX_STATE_PACKETS):
                new_chunk = []
                data = connection.recv(self.buffer_size).decode()
                
                if len(data) == 0:
                    reply = "Empty message"
                else:
                    new_chunk = data.split("_")
                    new_chunk = [float(numeric_str) for numeric_str in new_chunk]
                    state.extend(new_chunk)
                    if packet < (self.MAX_STATE_PACKETS-1):
                        reply = "Data received"
                        connection.send(reply.encode())

            add_noise = True
            if self.num_episodes % 10 == 0:
                add_noise = False
            
            scaled_action, legal_action, noise = self.prediction_engine.predict(state, add_noise)
            # print(". index: " + str(round(float(scaled_action))))
            # action = self.ACTION_INDEX_MAP[round(float(scaled_action))]
            # for num in action:
            #     reply += str(num) + " "
            # reply += str(scaled_action) + " " + str(legal_action) + " " + str(noise)
            # print(reply)
            # connection.send(reply.encode())

            print("action: ", scaled_action)
            reply = ""
            for num in scaled_action:
                reply += str(num) + " "
            reply += str(legal_action) + " " + str(noise)
            print(reply)
            connection.send(reply.encode())

        elif request == "new sample":
            reply = "Request received: add sample"
            connection.send(reply.encode())
            sample_data = []
            for packet in range(self.MAX_SAMPLE_PACKETS):
                new_chunk = []
                data = connection.recv(self.buffer_size).decode()
                if len(data) == 0:
                    reply = "Empty message"
                else:
                    new_chunk = data.split("_")
                    new_chunk = [float(numeric_str) for numeric_str in new_chunk]
                    sample_data.extend(new_chunk)
                    if packet < (self.MAX_SAMPLE_PACKETS-1):
                        reply = "Data received"
                        connection.send(reply.encode())

            # print(sample_data)
            self.prediction_engine.add_sample(sample_data)
            reply = "New sample added successfully"
            connection.send(reply.encode())

        elif request == "retrain":
            if int(self.prediction_engine.eps*100) > 0:
                self.prediction_engine.retrain()
                reply = str(self.prediction_engine.num_retrains)
                connection.send(reply.encode())
            else:
                reply = str(self.prediction_engine.num_retrains)
                connection.send(reply.encode())              

        elif request == "disconnect":
            reply = str(self.prediction_engine.eps) + "_" + str(self.prediction_engine.total_reward)
            connection.send(reply.encode())
            num_hits = int(connection.recv(self.buffer_size).decode())

            if self.num_episodes % 10 == 0:
                self.prediction_engine.finalize_episode(num_hits, use_noise=False)
            else:
                self.prediction_engine.finalize_episode(num_hits, use_noise=True)
                
            reply = "Server disconnected successfully"
            connection.send(reply.encode())

            self.num_episodes += 1
            self.complete_signal = 1
            
        else:
            reply = "unknown request"
            connection.send(reply.encode())

    def start(self, port_no, ipaddress):
        sock = self.start_connection(port_no, ipaddress)
        while True:
            # Establish connection with client 0.
            conn, addr = sock.accept()
            print('Connected to ', addr)
            
            self.complete_signal = 0
            while self.complete_signal != 1:
                request = conn.recv(self.buffer_size).decode()
                self.process_request(request, conn)

        # Close the connection with the client
        conn.close()
    