#ifndef ENTROPY_INDEX_H
#define ENTROPY_INDEX_H

#include "index/index.h"

/*
 * FIX_SEQUENCE options for EntropyIndex:
 * 0: A = maxETP, B = minETP
 * 1: A = unhashed_idx, B = maxETP
 * 2: A = maxETP, B = zeros
 * 3: A = maxETP, B = next_maxETP
 * 4: A = maxETP/minIdx, B = next_maxETP/nextIdx
 */

// common params
#define HISTORY_LENGTH  1
#define SENSE_THRESHOLD 20
#define RECONFIG_NO_DELAY 1
#define UPDATE_MISS_ONLY 1

// entropy index params
#define FIX_SEQUENCE 3
#define NOHASH_IF_SEQ_IDX_HIGHEST_ETP 0
#define SKIP_SAME_BIT_CANDIDATE 0
#define WARMUP_INTERVALS 2


inline
    string matching_option_to_string(uint32_t opt) {
    string str;
    if (opt == 0)
        str = "A = maxETP, B = minETP";
    else if (opt == 1)
        str = "A = unhashed_idx, B = maxETP";
    else if (opt == 2)
        str = "A = maxETP, B = zeros";
    else if (opt == 3)
        str = "A = maxETP, B = next_maxETP";
    else if (opt == 4)
        str = "A = maxETP/minIdx, B = next_maxETP/nextIdx";
    return str;
}

inline
    vector<uint32_t> zeros_segment(uint32_t size) {
    vector<uint32_t> zeros_segment;
    for (uint32_t i=0; i < size; ++i)
        zeros_segment.push_back(0);
    return zeros_segment;
} 

struct BitEntropy {
public:
    BitEntropy(uint32_t x_id, int32_t x_entropy=0)
        :m_id(x_id), m_entropy(x_entropy), m_is_segment(false) {}

    BitEntropy(vector<uint32_t> x_segid, int32_t x_entropy=0)
        :m_segid(x_segid), m_entropy(x_entropy), m_id(MAX_ADDRESS_BITS), m_is_segment(true) {}

    BitEntropy()
        :m_id(MAX_ADDRESS_BITS), m_entropy(0), m_is_segment(false) {}

    ~BitEntropy() {}

    BitEntropy(const BitEntropy& x_bit) {
        m_id         = x_bit.m_id;
        m_segid      = x_bit.m_segid;
        m_entropy    = x_bit.m_entropy;
        m_is_segment = x_bit.m_is_segment;
    }

    string to_string()  { return (get_str_id() + std::to_string(m_entropy)); }
    bool is_id_matched(vector<uint32_t>& x_segid) { return x_segid == m_segid; }
    bool is_id_matched(uint32_t& x_id)            { return x_id == m_id; }

    string get_str_id() {
        if (!m_is_segment)
            return ("[" + std::to_string(m_id) + "]");
        else {
            string str="[";
            for (uint32_t i=0; i < m_segid.size(); ++i)
                str += std::to_string(m_segid[i]) + " ";
            str.pop_back();
            str += "]";
            return str;
        }
    }

    friend bool operator<  (const BitEntropy &b1, const BitEntropy &b2) { return b1.m_entropy <  b2.m_entropy; }
    friend bool operator<= (const BitEntropy &b1, const BitEntropy &b2) { return b1.m_entropy <= b2.m_entropy; }
    friend bool operator>  (const BitEntropy &b1, const BitEntropy &b2) { return b1.m_entropy >  b2.m_entropy; }
    friend bool operator>= (const BitEntropy &b1, const BitEntropy &b2) { return b1.m_entropy >= b2.m_entropy; }
    friend bool operator== (const BitEntropy &b1, const BitEntropy &b2) { return b1.m_entropy == b2.m_entropy; }

    uint32_t m_id;              // index of this bit in the address
    vector<uint32_t> m_segid;   // index of this segment in the address
    int32_t m_entropy;          // entropy of this bit/segment
    bool m_is_segment;          // whether this is a segment or a single bit
};



//========================================================================//
// ENTROPY INDEX
//========================================================================//
class EntropyIndex : public Index {
public:
    EntropyIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
        uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
        :Index(x_numset, x_numway, x_interval_length), m_should_remap(false),
        REMAP_INTERVAL(remap_length), FILL_LEVEL(x_fill_lvl), block_array(arr), CPU_ID(cpu)
    {
        IDX_SIZE = lg2(NUM_SET);

        for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
            ovr_entropy.push_back(BitEntropy(i));
            itv_entropy.push_back(BitEntropy(i));
        }

        // seq_a = original idx, seq_b = 0 => idx = original idx
        for (int32_t i=IDX_SIZE-1; i >= 0; --i) {
            m_curr_seq_a.push_back(i);
            m_curr_seq_b.push_back(0);
        }
        m_last_seq_a = m_curr_seq_a;
        m_last_seq_b = m_curr_seq_b;

        m_next_seq_a = m_curr_seq_a;
        m_next_seq_b = m_curr_seq_b;

        if (x_print_cfg) {
            cout << endl;
            cout << "========================================================================" << endl;
            cout << "ENTROPY INDEX CONFIGURATION" << endl;
            cout << "========================================================================" << endl;
            cout << "Remap interval:   " << REMAP_INTERVAL << endl;
            cout << "Interval size:    " << INTERVAL_SIZE << endl;
            cout << "History length:   " << HISTORY_LENGTH << endl;
            cout << "Sensitivity:      " << SENSE_THRESHOLD << " (%)" << endl;
            cout << "ETP match option: " << matching_option_to_string(FIX_SEQUENCE) << endl;
            cout << "UnhashIdx maxETP: " << ((NOHASH_IF_SEQ_IDX_HIGHEST_ETP == 1)? "hashing OFF" : "hashing ON") << endl;
            cout << "No-delay reconfig: " << (RECONFIG_NO_DELAY? "Yes" : "No") << endl;
            cout << "Update Miss Only:  " << (UPDATE_MISS_ONLY? "Yes" : "No") << endl;
            cout << "Warmup intervals:  " << WARMUP_INTERVALS << endl;
            cout << "========================================================================" << endl;
            cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
            cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
            cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
            cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
            cout << "m_next_seq_a: " << vec_to_string(m_next_seq_a) << endl;
            cout << "m_next_seq_b: " << vec_to_string(m_next_seq_b) << endl;
            cout << endl;
        }

        m_addr_hist.push_back(0);
        PAST_PI_EVICTION = new uint64_t [NUM_SET];
        memset(PAST_PI_EVICTION, 0, NUM_SET * sizeof(*PAST_PI_EVICTION));
    }

    ~EntropyIndex() {}

    uint32_t get_set(uint64_t address);
    uint32_t compute_index(uint64_t address, vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b);
    void     print_stats();
    string   etp_vector_to_string(vector<BitEntropy>& entropy);
    void     warmup_check();
    bool     is_same_sequence(vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b);
    bool     is_zero_segment(vector<uint32_t>& x_seq_a);
    bool     check_remap();

    virtual void move_cache_data(MEMORY *lower_level);
    virtual void compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
    virtual void etp_tracker_access_update(uint64_t address);
    virtual void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address);
    virtual void etp_tracker_interval_update();

    // replacement policy
    uint32_t get_rrip_victim(uint32_t set);
    void     rrip_update(uint32_t set, uint32_t way, uint32_t src_age);
    uint32_t get_lru_victim(uint32_t set);
    void     lru_update(uint32_t set, uint32_t way, uint32_t src_lru_pos);
    uint32_t get_rlr_victim(uint32_t set);
    void     rlr_update(uint32_t set, uint32_t way, uint32_t src_set, uint32_t src_way);
    uint32_t global_idx(uint32_t set, uint32_t way);

protected:
    // XOR-hash configuration
    vector<uint32_t> m_curr_seq_a, m_curr_seq_b;
    vector<uint32_t> m_last_seq_a, m_last_seq_b;
    vector<uint32_t> m_next_seq_a, m_next_seq_b;

    // entropy runtime counters
    vector<BitEntropy> ovr_entropy;
    vector<BitEntropy> itv_entropy;
    vector<uint64_t> m_addr_hist;
    bool m_should_remap;

    uint32_t IDX_SIZE;
    uint32_t REMAP_INTERVAL;
    uint32_t FILL_LEVEL;
    uint32_t CPU_ID;
    BLOCK**  block_array; // ptr to the cache set data array

    uint32_t access_ctr    = 0;
    uint32_t map_ptr       = 0;
    uint64_t num_writeback = 0;
    uint64_t num_drop      = 0;
    uint64_t num_read      = 0;

    uint64_t* PAST_PI_EVICTION;
};

#endif // ENTROPY_INDEX_H