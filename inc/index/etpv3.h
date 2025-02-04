#ifndef ENTROPY_INDEX_V3_H
#define ENTROPY_INDEX_V3_H

#include "index/entropy2.h"

#define SENSE_DIFFLEVEL SENSE_THRESHOLD*3
#define PROFILE_PHASE_RATIO 0.75

enum class Phase{PROFILE, DUEL};

class EntropyV3 : public EntropyV2 {
public:
   EntropyV3(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
                 uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu, bool x_print_cfg=true)
      :EntropyV2(x_numset, x_numway, x_interval_length, remap_length, x_fill_lvl, arr, cpu, false)
   {
      // vector<uint32_t> zeros_segment;
      // vector<uint32_t> bit_sequence;
      // for (int32_t i=IDX_SIZE-1; i >= 0; --i) {
      //    zeros_segment.push_back(0);
      //    bit_sequence.push_back(i);
      // }
      // m_seq_a.push_back(BitEntropy(bit_sequence));
      // m_seq_b.push_back(BitEntropy(zeros_segment));

      if (x_print_cfg) {
         cout << endl;
         cout << "========================================================================" << endl;
         cout << "ENTROPY-V3 INDEX CONFIGURATION" << endl;
         cout << "========================================================================" << endl;
         cout << "Warmup intervals:    " << WARMUP_INTERVALS << endl;
         cout << "Remap interval:      " << REMAP_INTERVAL << endl;
         cout << "Interval size:       " << INTERVAL_SIZE << endl;
         cout << "History length:      " << HISTORY_LENGTH << endl;
         cout << "Sensitivity:         " << SENSE_THRESHOLD << " (%)" << endl;
         cout << "Sensitivity:         " << SENSE_DIFFLEVEL << endl;
         cout << "ETP match option:    " << matching_option_to_string(FIX_SEQUENCE) << endl;
         cout << "UnhashIdx maxETP:    " << ((NOHASH_IF_SEQ_IDX_HIGHEST_ETP == 1)? "hashing OFF" : "hashing ON") << endl;
         cout << "No-delay reconfig:   " << (RECONFIG_NO_DELAY? "Yes" : "No") << endl;
         cout << "Update Miss Only:    " << (UPDATE_MISS_ONLY? "Yes" : "No") << endl;
         cout << "Skip Prefetch:       " << (SKIP_UPDATE_PF? "Yes" : "No") << endl;
         cout << "Skip Writeback:      " << (SKIP_UPDATE_WB? "Yes" : "No") << endl;
         cout << "ENTROPY_CTR_MAX:     " << ENTROPY_CTR_MAX << endl;
         cout << "CORREL_CTR_MAX:      " << CORREL_CTR_MAX << endl;
         cout << "PROFILE_PHASE_RATIO: " << PROFILE_PHASE_RATIO << endl;
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
   ~EntropyV3() {}

   void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
   void etp_tracker_access_update(bool is_hit, uint32_t type, uint64_t address);
   void compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   void compute_candidate_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   bool should_update(bool is_hit, uint32_t type);

private:
   /*
    * An index function candidate (A^B) consists of 2 sequences: A in m_seq_a, B in m_seq_b.
    * The conflict tracker of each candidate is updated in both
    */
   vector<BitEntropy> m_seq_a;
   vector<BitEntropy> m_seq_b;
   Phase current_phase{Phase::PROFILE};
};

#endif // ENTROPY_INDEX_V3_H