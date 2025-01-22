#ifndef ENTROPY_INDEX_V2_H
#define ENTROPY_INDEX_V2_H

#include "index/entropy.h"

class EntropyV2 : public EntropyIndex {
public:
   EntropyV2(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
                 uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
      :EntropyIndex(x_numset, x_numway, x_interval_length, remap_length, x_fill_lvl, arr, cpu, false)
   {
      /*
       * Bitwise Correlation Tracker. 
       * Each entry is a correlation between 2 address bits. Example: correl(bit_1, bit_0), correl(bit_2, bit_1), etc.
       * correl(bit_1, bit_0) <=> correl(bit_0, bit_1), so only keep one.
       */
      for (uint32_t i=1; i < MAX_ADDRESS_BITS; ++i) {
         for (uint32_t j=0; j < i; ++j) {
            vector<uint32_t> bit_pair = {i,j};
            correl_table.push_back(BitEntropy(bit_pair));
         }
      }
      if (x_print_cfg) {
         cout << endl;
         cout << "========================================================================" << endl;
         cout << "ENTROPY-V2 INDEX CONFIGURATION" << endl;
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
         cout << "========================================================================" << endl;
         cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
         cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
         cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
         cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
         cout << "m_next_seq_a: " << vec_to_string(m_next_seq_a) << endl;
         cout << "m_next_seq_b: " << vec_to_string(m_next_seq_b) << endl;
         cout << endl;
      }
   }
   ~EntropyV2() {}

   void etp_tracker_access_update(uint64_t address);
   void compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   void move_cache_data(MEMORY *lower_level);
   bool remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level);

protected:
   vector<BitEntropy> correl_table;
};

#endif // ENTROPY_INDEX_V3_H