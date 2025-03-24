#ifndef ENTROPY_INDEX_V3_H
#define ENTROPY_INDEX_V3_H

#include "index/entropy.h"

#define V3_ETP_CTR_MAX 1048576  // 20-bit
#define V3_COR_CTR_MAX 1048576   // 20-bit
#define V3_SKIP_UPDATE_PF 0
#define V3_SKIP_UPDATE_WB 0
#define V3_SKIP_UPDATE_WB_HIT 0
#define V3_UPDATE_HITMISS_SETTING 1 // 0: update on both hit/miss, 1: update miss only, 2: update hit only
#define V3_SENSE_THRESHOLD 4
#define CORR_PRORATE 0.75

class EntropyV3 : public EntropyIndex {
public:
    EntropyV3(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
                    uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
        :EntropyIndex(x_numset, x_numway, x_interval_length, remap_length, x_fill_lvl, arr, cpu, false)
    {
        /*
        * Bitwise Correlation Tracker. 
        * Each entry is a correlation between 2 address bits. Example: correl(bit_1, bit_0), correl(bit_2, bit_1), etc.
        * correl(bit_1, bit_0) <=> correl(bit_0, bit_1), so only keep one.
        */
        for (uint32_t i=0; i < IDX_SIZE; ++i) {
            for (uint32_t j=IDX_SIZE; j < MAX_ADDRESS_BITS; ++j) {
                vector<uint32_t> bit_pair = {i,j};
                correl_table.push_back(BitEntropy(bit_pair));
            }
        }
        

        // for 32 tracking bits, there are 496 possible pairs -> 495*248=122760 correlations to track
        for (uint32_t i=0; i < IDX_SIZE; ++i) {
            for (uint32_t j=IDX_SIZE; j < MAX_ADDRESS_BITS; ++j) {
                vector<uint32_t> bit_pair = {i,j};
                bitpair_entropy.push_back(BitEntropy(bit_pair));
                bitpair_ptn_ctr.push_back(BitEntropy(bit_pair));
            }
        }
        last_access_flip_behavior.resize(bitpair_ptn_ctr.size());

        if (x_print_cfg) {
            cout << endl;
            cout << "========================================================================" << endl;
            cout << "ENTROPY-V3 INDEX CONFIGURATION" << endl;
            cout << "========================================================================" << endl;
            cout << "Warmup intervals:   " << WARMUP_INTERVALS << endl;
            cout << "Remap interval:     " << REMAP_INTERVAL << endl;
            cout << "Interval size:      " << INTERVAL_SIZE << endl;
            cout << "History length:     " << HISTORY_LENGTH << endl;
            cout << "Max address bits:   " << MAX_ADDRESS_BITS << endl;
            cout << "Sensitivity:        " << SENSE_THRESHOLD << " (%)" << endl;
            cout << "Update Hit/Miss:    " << (V3_UPDATE_HITMISS_SETTING==1? "MISS ONLY" : (V3_UPDATE_HITMISS_SETTING==2? "HIT ONLY" : "BOTH HIT/MISS")) << endl;
            cout << "Skip Prefetch:      " << (V3_SKIP_UPDATE_PF? "Yes" : "No") << endl;
            cout << "Skip Writeback:     " << (V3_SKIP_UPDATE_WB? "Yes" : "No") << endl;
            cout << "Skip Writeback Hit: " << (V3_SKIP_UPDATE_WB_HIT? "Yes" : "No") << endl;
            cout << "V3_ETP_CTR_MAX:     " << V3_ETP_CTR_MAX << endl;
            cout << "V3_COR_CTR_MAX:     " << V3_COR_CTR_MAX << endl;
            cout << "========================================================================" << endl;
            cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
            cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
            cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
            cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
            cout << endl;
        }
    }
    ~EntropyV3() {}

    void     move_cache_data(MEMORY *lower_level);
    bool     remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level);
    bool     should_update(bool is_hit, uint32_t type);

    void     update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
    void     etp_tracker_access_update(uint64_t address);
    void     compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
    
    uint32_t compute_sum_correl_two_sequences(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
    uint32_t get_correl(uint32_t bit_a, uint32_t bit_b);
    uint32_t compute_pair_table_idx(uint32_t bit_a, uint32_t bit_b);
    void     get_cfg_1(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
                       vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b);
    void     get_cfg_2(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
                       vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b);
    void     get_cfg_3(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
                       vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b);

protected:
   vector<BitEntropy>  correl_table;
   uint32_t num_reset_cor{0};
   uint32_t num_reset_itv{0};
   uint32_t num_reset_bitpair_etp{0};
   uint32_t num_reset_bitpair_ptn{0};
   vector<BitEntropy> bitpair_entropy;
   vector<BitEntropy> bitpair_ptn_ctr;

//    vector<uint32_t> last_interval_bpe_ids;
   vector<bool> last_access_flip_behavior;

//    vector<BitEntropy> sorted_ovr;
//    vector<BitEntropy> sorted_itv;
//    vector<BitEntropy> sorted_bitpair_etp;
//    vector<BitEntropy> sorted_bitpair_pattern;
};

#endif // ENTROPY_INDEX_V3_H