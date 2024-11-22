#ifndef ENTROPY_INDEX_V3_H
#define ENTROPY_INDEX_V3_H

#include "index/entropy.h"
#define READ_DBLK_FILE_INTERNAL 1

class EntropyV3 : public EntropyIndex {
public:
   EntropyV3(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
                 uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
      :EntropyIndex(x_numset, x_numway, x_interval_length, remap_length, x_fill_lvl, arr, cpu, false)
   {
      if (x_print_cfg) {
         cout << endl;
         cout << "========================================================================" << endl;
         cout << "ENTROPY-V3 INDEX CONFIGURATION" << endl;
         cout << "========================================================================" << endl;
         cout << "Warmup intervals:   " << WARMUP_INTERVALS << endl;
         cout << "Remap interval:     " << REMAP_INTERVAL << endl;
         cout << "Interval size:      " << INTERVAL_SIZE << endl;
         cout << "History length:     " << HISTORY_LENGTH << endl;
         cout << "Sensitivity:        " << SENSE_THRESHOLD << " (%)" << endl;
         cout << "ETP match option:   " << matching_option_to_string(FIX_SEQUENCE) << endl;
         cout << "UnhashIdx maxETP:   " << ((NOHASH_IF_SEQ_IDX_HIGHEST_ETP == 1)? "hashing OFF" : "hashing ON") << endl;
         cout << "No-delay reconfig:  " << (RECONFIG_NO_DELAY? "Yes" : "No") << endl;
         cout << "Update Miss Only:   " << (UPDATE_MISS_ONLY? "Yes" : "No") << endl;
         cout << "Read dblk_file internally: " << (READ_DBLK_FILE_INTERNAL? "Yes" : "No") << endl;
         cout << "========================================================================" << endl;
         cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
         cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
         cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
         cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
         cout << "m_next_seq_a: " << vec_to_string(m_next_seq_a) << endl;
         cout << "m_next_seq_b: " << vec_to_string(m_next_seq_b) << endl;
         cout << endl;
      }
      if (READ_DBLK_FILE_INTERNAL) {
         read_dblk_file();
      }
   }
   ~EntropyV3() {}

   void move_cache_data(MEMORY *lower_level);
   void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
   void compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   bool should_update_trackers(uint64_t address, bool is_hit, uint32_t type);
   bool remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level);
   void read_dblk_file();
   bool is_dead_block(uint64_t x_addr) {
      return binary_search(dead_blocks.begin(), dead_blocks.end(), x_addr);
   }

private:
   bool m_perfect_deadblock_mode = false;
   vector<uint64_t> dead_blocks;
};

#endif // ENTROPY_INDEX_V3_H