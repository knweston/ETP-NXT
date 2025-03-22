#ifndef ENTROPY_INDEX_V2_H
#define ENTROPY_INDEX_V2_H

#include "index/entropy.h"

#define ENTROPY_CTR_MAX 1048576  // 20-bit
#define CORREL_CTR_MAX 1048576   // 20-bit
#define SKIP_UPDATE_PF 1
#define SKIP_UPDATE_WB 0
#define SKIP_UPDATE_WB_HIT 0
#define UPDATE_HITMISS_SETTING 1 // 0: update on both hit/miss, 1: update miss only, 2: update hit only
#define SKIP_UPDATE_DURING_REMAP 0

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

      // for (uint32_t i=0; i < IDX_SIZE; ++i) {
      //    bitpair_entropy.push_back(BitEntropy(i));
      // }
      // for (uint32_t i=1; i < IDX_SIZE; ++i) {
      //    for (uint32_t j=0; j < i; ++j) {
      //       vector<uint32_t> bit_pair = {i,j};
      //       bitpair_correl_table.push_back(BitEntropy(bit_pair));
      //    }
      // }

      vector<uint32_t> hard_coded_idx = {0,1,2,3,30,28,21,15,18,22,24,19,4,5,8,10,6,7,12,11,9,14};       // entropy
      // vector<uint32_t> hard_coded_idx = {0,1,2,3,15,4,5,30,28,21,18,10,19,8,24,12,6,22,7,9,11,14};       // etp2

      // for 32 tracking bits, there are 496 possible pairs -> 495*248=122760 correlations to track
      for (uint32_t i=1; i < MAX_ADDRESS_BITS; ++i) {
         for (uint32_t j=0; j < i; ++j) {
            vector<uint32_t> bit_pair = {i,j};
            bitpair_entropy.push_back(BitEntropy(bit_pair));
            bitpair_pattern_hist.push_back(BitEntropy(bit_pair));
            // last_access_flip_behavior.push_back(-1);
         }
      }
      last_access_flip_behavior.resize(bitpair_pattern_hist.size());

      for (uint32_t i=0; i<hard_coded_idx.size()/2; ++i) {
         vector<uint32_t> bit_pair_a = {hard_coded_idx[i],hard_coded_idx[i+hard_coded_idx.size()/2]};
         vector<uint32_t> bit_pair_b = {hard_coded_idx[i+hard_coded_idx.size()/2],hard_coded_idx[i]};
         for (uint32_t j=0; j < bitpair_entropy.size(); ++j) {
            if (bitpair_entropy[j].is_id_matched(bit_pair_a) || bitpair_entropy[j].is_id_matched(bit_pair_b)) {
               hard_coded_bpe_ids.push_back(j);
            }
         }
      }

      for (uint32_t i=1; i < bitpair_entropy.size(); ++i) {
         for (uint32_t j=0; j < i; ++j) {
            vector<uint32_t> bit_pair = {i,j};
            bitpair_correl_table.push_back(BitEntropy(bit_pair));
         }
      }

      cout << "Hardcoded BPE:" << endl;
      for (uint32_t i=0; i<hard_coded_bpe_ids.size(); ++i) {
         cout << bitpair_entropy[hard_coded_bpe_ids[i]].to_string() << endl;
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
         cout << "Max address bits:   " << MAX_ADDRESS_BITS << endl;
         cout << "Sensitivity:        " << SENSE_THRESHOLD << " (%)" << endl;
         cout << "ETP match option:   " << matching_option_to_string(FIX_SEQUENCE) << endl;
         cout << "UnhashIdx maxETP:   " << ((NOHASH_IF_SEQ_IDX_HIGHEST_ETP == 1)? "hashing OFF" : "hashing ON") << endl;
         cout << "No-delay reconfig:  " << (RECONFIG_NO_DELAY? "Yes" : "No") << endl;
         cout << "Update Hit/Miss:    " << (UPDATE_HITMISS_SETTING==1? "MISS ONLY" : (UPDATE_HITMISS_SETTING==2? "HIT ONLY" : "BOTH HIT/MISS")) << endl;
         cout << "Skip Prefetch:      " << (SKIP_UPDATE_PF? "Yes" : "No") << endl;
         cout << "Skip Writeback:     " << (SKIP_UPDATE_WB? "Yes" : "No") << endl;
         cout << "Skip Writeback Hit: " << (SKIP_UPDATE_WB_HIT? "Yes" : "No") << endl;
         cout << "Skip During Remap:  " << (SKIP_UPDATE_DURING_REMAP? "Yes" : "No") << endl;
         cout << "ENTROPY_CTR_MAX:    " << ENTROPY_CTR_MAX << endl;
         cout << "CORREL_CTR_MAX:     " << CORREL_CTR_MAX << endl;
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

   void     update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
   void     etp_tracker_access_update(uint64_t address);
   // void etp_tracker_access_update(bool is_hit, uint32_t type, uint64_t address);
   void     compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   void     move_cache_data(MEMORY *lower_level);
   bool     remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level);
   uint32_t get_correl(uint32_t bit_a, uint32_t bit_b);
   uint32_t get_correl_table_index(uint32_t bit_a, uint32_t bit_b);
   uint32_t find_min_correl_bit_from_sequence(uint32_t bit_a, vector<uint32_t>& sequence);
   uint32_t find_min_total_correl_bit_in_sequence(vector<uint32_t>& sequence);
   uint32_t compute_sum_correl_two_sequences(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b);
   uint32_t compute_sum_correl_bit_vs_sequence(uint32_t bit_a, vector<uint32_t>& sequence);
   bool     should_update(bool is_hit, uint32_t type);
   void     eviction_update(BLOCK &victim_blk);


protected:
   vector<BitEntropy> correl_table;
   uint32_t num_reset_cor{0};
   uint32_t num_reset_itv{0};
   uint32_t num_reset_bitpair_etp{0};
   uint32_t num_reset_bitpair_correl{0};
   uint32_t num_reset_bitpair_ptn{0};
   vector<BitEntropy> bitpair_entropy;
   vector<BitEntropy> bitpair_pattern_hist;
   vector<BitEntropy> bitpair_correl_table;

   vector<uint32_t> last_interval_bpe_ids;
   vector<bool> last_access_flip_behavior;
   vector<uint32_t> hard_coded_bpe_ids;
};

#endif // ENTROPY_INDEX_V3_H