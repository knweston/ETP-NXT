#ifndef NEURAL_INDEX_H
#define NEURAL_INDEX_H

#include "index.h"

#define INTERVAL_SIZE      1000000  // 2M, 5M
#define SAMPLE_BUFFER_SIZE 3
#define STATE_LENGTH       2
#define WINDOW_SIZE        1   // 1, 2
#define NUM_GROUPS         11  // 12, 14, log2(LLC_SET) (for DQN)

#define CHUNK_SIZE         (STATE_LENGTH >= 1024 ? 1024 : STATE_LENGTH) 
#define NUM_CHUNKS         (STATE_LENGTH / 1024 ? (STATE_LENGTH / 1024) : 1)


struct Sample {
    double state[STATE_LENGTH]      = {0};
    double next_state[STATE_LENGTH] = {0};
    double action = 0;
    double reward = 0;
};

class NeuralIndex : public Index {
public:
    NeuralIndex(int32_t buffer_size=8192);
    ~NeuralIndex();
    void initialize(vector<Access>& history) {};
    uint32_t get_set(uint64_t address);
    bool update(uint64_t address);

    void   move_cache_data();   // move current cache data to new places based on the new mapping
    void   get_prediction();
    void   retrain();
    void   sendSample(Sample* sample);
    string sendMessage(string msg);
    string getReply();
    void   connectServer();
    void   disconnectServer();
    
    uint32_t compute_index(uint64_t address, uint32_t* bit_sel);

protected:
    // connection config
    int32_t m_port;
    int32_t m_sock;
    int32_t m_bsize;
    string m_ipaddr;

    // current XOR mask config
    uint32_t curr_sel[NUM_GROUPS] = {0};    // 13 possible pairs out of 26 least significant bits (excluding the offset)
    uint32_t next_sel[NUM_GROUPS] = {0};

    // Runtime access counters
    uint64_t N_ACCESS[LLC_SET] = {0};
    uint64_t D_ACCESS[LLC_SET] = {0};
    uint64_t num_misses_ckpt;
    
    Sample* sample_buffer[SAMPLE_BUFFER_SIZE];
    
    // Stats
    uint64_t num_intervals;
};





#endif // NEURAL_INDEX_H