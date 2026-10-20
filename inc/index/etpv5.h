#ifndef ENTROPY_INDEX_V5_H
#define ENTROPY_INDEX_V5_H

#include "index/entropy.h"

#ifndef V5_SKIP_TRAINING_DURING_REMAP
#define V5_SKIP_TRAINING_DURING_REMAP 0
#endif
#if V5_SKIP_TRAINING_DURING_REMAP != 0 && V5_SKIP_TRAINING_DURING_REMAP != 1
#error "V5_SKIP_TRAINING_DURING_REMAP must be 0 or 1"
#endif

#define V5_ETP_CTR_MAX 65536  // 20-bit: 1048576
#define V5_SKIP_UPDATE_PF 1
#define V5_SKIP_UPDATE_WB 0
#define V5_CUTOFF_PRORATE 0.5
// Accept a candidate when MIN * N <= E <= MAX * N.
// Use long-double literals (L suffix) to preserve precision at the boundaries.
#ifndef V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT
#define V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT 1.6L
#endif
#ifndef V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT
#define V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT 2.4L
#endif
static_assert(V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT >= 0.0,
    "V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT must be nonnegative");
static_assert(V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT >= V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT,
    "V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT must be at least the minimum coefficient");
#define V5_REPLACEMENT_POLICY 1    // 0: LRU, 1: SRRIP, 2: RLR

class EntropyV5 : public EntropyIndex {
public:
    EntropyV5(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
        uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
        :EntropyIndex(x_numset, x_numway, x_interval_length, remap_length, x_fill_lvl, arr, cpu, false)
    {
        for (uint32_t i = 0; i < IDX_SIZE; ++i) {
            for (uint32_t j = IDX_SIZE; j < MAX_ADDRESS_BITS; ++j) {
                vector<uint32_t> bit_pair = {i, j};
                bitpair_entropy.push_back(BitEntropy(bit_pair));
            }
        }

        if (x_print_cfg) {
            cout << endl;
            cout << "========================================================================" << endl;
            cout << "ENTROPY-V5 INDEX CONFIGURATION" << endl;
            cout << "========================================================================" << endl;
            cout << "Remap interval:       " << REMAP_INTERVAL << endl;
            cout << "Interval size:        " << INTERVAL_SIZE << endl;
            cout << "Max address bits:     " << MAX_ADDRESS_BITS << endl;
            cout << "Skip Prefetch:        " << (V5_SKIP_UPDATE_PF? "Yes" : "No") << endl;
            cout << "Skip Writeback:       " << (V5_SKIP_UPDATE_WB? "Yes" : "No") << endl;
            cout << "V5_ETP_CTR_MAX:       " << V5_ETP_CTR_MAX << endl;
            cout << "V5_CUTOFF_PRORATE:       " << V5_CUTOFF_PRORATE << endl;
            cout << "V5_REPLACEMENT_POLICY:   " << (V5_REPLACEMENT_POLICY==1? "SRRIP" : (V5_REPLACEMENT_POLICY==2? "RLR" : "LRU")) << endl;
            cout << "========================================================================" << endl;
            cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
            cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
            cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
            cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
            cout << "V5_SKIP_TRAINING_DURING_REMAP: "
                 << (V5_SKIP_TRAINING_DURING_REMAP ? "Yes" : "No") << endl;
            cout << "V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT: "
                 << V5_CANDIDATE_ENTROPY_MIN_COEFFICIENT << endl;
            cout << "V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT: "
                 << V5_CANDIDATE_ENTROPY_MAX_COEFFICIENT << endl;
            cout << endl;
        }
    }
    void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) override;
    void move_cache_data(MEMORY *lower_level);
    bool remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level);
    bool should_update(bool is_hit, uint32_t type);
    void eviction_pair_update(uint64_t address, uint64_t eviction_address);
    void reset_eviction_pair_counters();
    void compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
    uint32_t replacement_getvictim(uint32_t index);
    void replacement_update(uint32_t index, uint32_t way, uint32_t remapping_index, uint32_t remapping_way);
    uint32_t compute_pair_table_idx(uint32_t bit_a, uint32_t bit_b);
    void get_cfg_5(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b,
        vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b);

protected:
    vector<BitEntropy> bitpair_entropy;
    uint32_t num_reset_itv{0};
    uint32_t num_reset_bitpair_etp{0};
    uint64_t num_eviction_pairs{0};
    // Same updates as num_eviction_pairs; halved with the bit-pair counters.
    uint64_t scaled_num_eviction_pairs{0};
};

#endif // ENTROPY_INDEX_V5_H
