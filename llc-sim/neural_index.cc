#include "neural_index.h"

//========================================================================//
// CLASS NEURAL_INDEX
//========================================================================//
NeuralIndex::NeuralIndex(int32_t buffer_size) {
    m_port   = readPortConfig(CONFIG_FILE);
    m_ipaddr = readIPConfig(CONFIG_FILE);
    m_bsize  = buffer_size;
    m_sock   = createSocket();

    num_intervals   = 0;
    num_misses_ckpt = 0;
    
    for (uint32_t i=0; i < SAMPLE_BUFFER_SIZE; ++i)
        sample_buffer[i] = new Sample();

    // for (int32_t i=NUM_GROUPS-1; i >= 0; --i) {
    //     curr_sel[i] = i;
    //     next_sel[i] = i;
    // }

    // for (int32_t i=0; i < NUM_GROUPS; ++i) {
    //     curr_sel[i] = NUM_GROUPS-1-i;
    //     next_sel[i] = NUM_GROUPS-1-i;
    // }

    memset(curr_sel, 0, sizeof(curr_sel));
    memset(next_sel, 0, sizeof(next_sel));

    cout << endl;
    cout << "========================================================================" << endl;
    cout << "NEURAL INDEX CONFIGURATION" << endl;
    cout << "========================================================================" << endl;
    cout << "Config file path: " << CONFIG_FILE << endl;
    cout << "State length:     " << STATE_LENGTH << endl;
    cout << "Chunk size:       " << CHUNK_SIZE << endl;
    cout << "Num chunks:       " << NUM_CHUNKS << endl;
    cout << "Window size:      " << WINDOW_SIZE << endl;
    cout << "Num groups:       " << NUM_GROUPS << endl;
    cout << "========================================================================" << endl;
    cout << endl;

    connectServer();
}

//========================================================================//
NeuralIndex::~NeuralIndex() {
    for (uint32_t i=0; i < SAMPLE_BUFFER_SIZE; ++i) {
        delete sample_buffer[i];
    }
    disconnectServer();
}

bool NeuralIndex::update(uint64_t address) {
    uint32_t neural_set = get_set(address);
    uint32_t normal_set = address & ((1 << lg2(LLC_SET)) - 1);

    N_ACCESS[normal_set]++;
    D_ACCESS[neural_set]++;

    if ((CURRENT_PTR % INTERVAL_SIZE == 0) && (CURRENT_PTR > 0)) {
        cout << "Summary of Interval: " << num_intervals << endl;
        cout << "selection_(n):   ";
        for (uint32_t i=0; i < NUM_GROUPS; ++i)
            cout << curr_sel[i] << " ";
        cout << endl;
        cout << "selection_(n+1): ";
        for (uint32_t i=0; i < NUM_GROUPS; ++i)
            cout << next_sel[i] << " ";
        cout << endl;

        num_intervals++;
        
        bool need_move = !(arr_to_string(curr_sel, NUM_GROUPS, ",") == arr_to_string(next_sel, NUM_GROUPS, ","));
        
        if (need_move)
            memcpy(curr_sel, next_sel, sizeof(curr_sel));
            move_cache_data();
        
        get_prediction();
        
        // Reset the runtime counters
        memset(N_ACCESS, 0, sizeof(N_ACCESS));
        memset(D_ACCESS, 0, sizeof(D_ACCESS));
        memset(INTERVAL_EVICTIONS, 0, sizeof(INTERVAL_EVICTIONS));
        return need_move;
    }
    return false;
}


// move current cache data to new places based on the new mapping
// rule: first come first serve, blocks coming late are dropped
void NeuralIndex::move_cache_data() {
    uint64_t mirrored_cache[LLC_SET][LLC_WAY];
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way)
            mirrored_cache[set][way] = 0;
    }

    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way) {
            uint32_t set_index = get_set(CACHE[set][way]);
            bool is_drop = true;
            for (uint32_t i=0; i < LLC_WAY; ++i) {
                if (mirrored_cache[set_index][i] == 0) {
                    mirrored_cache[set_index][i] = CACHE[set][way];
                    is_drop = false;
                    break;
                }
            }
            if (is_drop)
                NUM_DROPS++;
        }
    }
    
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way)
            CACHE[set][way] = mirrored_cache[set][way];
    }
}


//========================================================================//
void NeuralIndex::get_prediction() {
    string reply = sendMessage("make prediction");

    // // Compute reward (difference btw. std_devs)
    // double neural_sd = compute_std_dev(D_ACCESS, LLC_SET);
    // double normal_sd = compute_std_dev(N_ACCESS, LLC_SET);

    // // Compute reward (eviction sd)
    // double evict_sd = compute_std_dev(INTERVAL_EVICTIONS, LLC_SET);
    // double reward   = (-1)*evict_sd;

    // // Compute state (actual eviction distribution)
    // double state_array[STATE_LENGTH];
    // standardize_array(INTERVAL_EVICTIONS, state_array, STATE_LENGTH);

    // Compute reward (miss rate)
    double total_misses = 0;
    for (uint32_t i=0; i < NUM_TYPES; ++i)
        total_misses += MISSES[i];
    double reward = (-10.0)*(total_misses-num_misses_ckpt)/INTERVAL_SIZE;
    // double reward = (total_misses-num_misses_ckpt)/INTERVAL_SIZE;
    num_misses_ckpt = total_misses;

    // Compute state [stdev(hash), stdev(nohash),stdev(eviction)]
    double neural_sd = compute_std_dev(D_ACCESS, LLC_SET);
    double normal_sd = compute_std_dev(N_ACCESS, LLC_SET);
    double evict_sd  = compute_std_dev(INTERVAL_EVICTIONS, LLC_SET);

    // Compute reward (eviction rate)
    // double total_evicts = 0;
    // for (uint32_t i=0; i < LLC_SET; ++i)
    //     total_evicts += INTERVAL_EVICTIONS[i];
    // double reward = (-1)*total_evicts/INTERVAL_SIZE;

    // Compute state (actual eviction distribution)
    double state_array[STATE_LENGTH] = {10*neural_sd/normal_sd, 10*evict_sd/normal_sd};
    // standardize_array(state_array, STATE_LENGTH);

    // Send state vector to the prediction server (sending in chunks)
    for (uint32_t chunk=0; chunk < NUM_CHUNKS; ++chunk)
        reply = sendMessage(flatten(state_array, (chunk+1)*CHUNK_SIZE, chunk*CHUNK_SIZE));
    
    // Server replies the prediction i.e., action
    // double scaled_action = 0;
    double legal_action = 0;
    double noise = 0;
    stringstream ss(reply);
    for (uint32_t i=0; i < NUM_GROUPS; ++i)
        ss >> next_sel[i];
    // ss >> scaled_action;
    ss >> legal_action;
    ss >> noise;
    
    // Add new sample for this interval (interval n)
    uint32_t curr_index = (num_intervals-1) % SAMPLE_BUFFER_SIZE;
    memcpy(sample_buffer[curr_index]->state, state_array, sizeof(sample_buffer[curr_index]->state));
    sample_buffer[curr_index]->action = legal_action;

    // Update and send sample of (interval n-2)
    if (num_intervals > 2) {
        uint32_t update_index = num_intervals % SAMPLE_BUFFER_SIZE;
        memcpy(sample_buffer[update_index]->next_state, state_array, sizeof(sample_buffer[update_index]->next_state));
        sample_buffer[update_index]->reward = reward;
        sendSample(sample_buffer[update_index]);
    } 

    cout << std::fixed 
        //  << "scaled_action: " << scaled_action << endl;
         << "legal_action:  " << legal_action << endl;     
    cout << "noise:       " << noise << endl;
    cout << "normal sd:   " << normal_sd << endl;
    cout << "neural_sd:   " << neural_sd << endl;
    cout << "eviction_sd: " << evict_sd << endl;
    cout << "reward:      " << reward << endl;
    cout << "state:       " << arr_to_string(state_array, STATE_LENGTH, ",") << endl;
    cout << endl;

    // Send retrain signal
    if (num_intervals % 3 == 0)
        retrain();
}

//========================================================================//
void NeuralIndex::retrain() {
    string reply = sendMessage("retrain");
    cout << "Model retrained successfully" << endl;
}

//========================================================================//
uint32_t NeuralIndex::get_set(uint64_t address) {
    return compute_index(address, curr_sel);
}

//========================================================================//
void NeuralIndex::connectServer() {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(m_port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, m_ipaddr.c_str(), &serv_addr.sin_addr) <= 0)
        throw "Invalid address / Address not supported";
    if (connect(m_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        throw "Connection Failed";
}

//========================================================================//
void NeuralIndex::disconnectServer() {
    string reply   = sendMessage("disconnect");
    string eps     = "";
    string reward  = "";
    bool is_reward = false;
    for (uint32_t i=0; i < reply.size(); ++i) {
        if (reply[i] == '_') {
            is_reward = true;
            continue;
        }
        if (!is_reward)
            eps += reply[i];
        else
            reward += reply[i];
    }

    uint64_t num_hits = HITS[LOAD] + HITS[RFO] + HITS[PREFETCH] + HITS[WRITEBACK];
    reply             = sendMessage(to_string(num_hits));

    // Ending episode statistics
    cout << "Prediction module statistics" << endl;
    cout << "EPS:     " << eps << endl;
    cout << "Rewards: " << reward << endl;
    cout << "========================================================================" << endl;
    close(m_sock); 
}

//========================================================================//
string NeuralIndex::sendMessage(string msg) {
    send(m_sock, msg.c_str(), strlen(msg.c_str()), 0);
    return getReply();
}

//========================================================================//
string NeuralIndex::getReply() {
    char buffer[m_bsize];
    int32_t valread = read(m_sock, buffer, m_bsize);
    string reply;
    for (int32_t i=0; i < valread; ++i) 
        reply += buffer[i];
    return reply;
}

//========================================================================//
void NeuralIndex::sendSample(Sample* sample) {
    string reply = sendMessage("new sample");

    for (uint32_t chunk=0; chunk < NUM_CHUNKS; ++chunk)
        reply = sendMessage(flatten(sample->state, (chunk+1)*CHUNK_SIZE, chunk*CHUNK_SIZE));
    
    for (uint32_t chunk=0; chunk < NUM_CHUNKS; ++chunk)
        reply = sendMessage(flatten(sample->next_state, (chunk+1)*CHUNK_SIZE, chunk*CHUNK_SIZE));

    reply = sendMessage(to_string(sample->action));
    reply = sendMessage(to_string(sample->reward));
}

uint32_t NeuralIndex::compute_index(uint64_t address, uint32_t* bit_sel) {
    uint32_t unhashed_index = (address & ((1 << lg2(LLC_SET)) - 1));

    // nohash config: when bit_sel = [0,0,...,0]
    bool is_nohash = true;
    for (uint32_t i=0; i < NUM_GROUPS; ++i) {
        if (bit_sel[i] > 0) {
            is_nohash = false;
            break;
        }
    }
    if (is_nohash)
        return unhashed_index;

    uint64_t tag = address >> lg2(LLC_SET);
    uint32_t selected_pairs[NUM_GROUPS] = {0};
    for (uint32_t i=0; i < NUM_GROUPS; ++i) {
        selected_pairs[i] = (tag >> (WINDOW_SIZE*bit_sel[i])) & ((1 << WINDOW_SIZE) - 1);
    }

    uint32_t mask = 0;
    for (uint32_t i=0; i < NUM_GROUPS; ++i) {
        mask = (mask << WINDOW_SIZE) | selected_pairs[i];
    }
    mask &= ((1 << lg2(LLC_SET)) - 1);  // remove redundant bits in the right (if any)

    // sanity check
    if (mask >= LLC_SET) {
        cout << "Invalid mask: " << mask << endl;
        assert(false);
    }
    return mask ^ unhashed_index;
}