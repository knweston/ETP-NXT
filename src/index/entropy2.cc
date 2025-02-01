#include "index/entropy2.h"

void EntropyV2::move_cache_data(MEMORY *lower_level) {
   for (uint32_t way=0; way < NUM_WAY; ++way) {
      BLOCK *src_block = &block_array[map_ptr][way];

      // initiate a move if the source block is valid
      if (src_block->valid) {
         
         // compute new index
         uint32_t dest_index = compute_index(src_block->address, m_curr_seq_a, m_curr_seq_b);
         uint32_t last_index = compute_index(src_block->address, m_last_seq_a, m_last_seq_b);

         // if new index == map_ptr => this src_block is already in the new position => skip
         if (dest_index == map_ptr)
            continue;

         // sanity check
         uint32_t curr_index = get_set(src_block->address);
         if (curr_index != map_ptr) {
            cout << "ERROR: curr_index != map_ptr" << endl;
            cout << "address:  " << src_block->address << endl;
            cout << "cpu:      " << CPU_ID << endl;
            cout << "curr_idx: " << curr_index << endl;
            cout << "map_ptr:  " << map_ptr << endl;
            cout << "dest_idx: " << dest_index << endl;
            cout << "last_idx: " << last_index << endl;
            assert(false);
         }

         uint32_t dest_way = get_lru_victim(dest_index);
         BLOCK *dest_block = &block_array[dest_index][dest_way];
         bool fill_ready = remap_tie_break(src_block, dest_block, lower_level);

         // overwrite destination block with source block
         // we need not to up-invalidate the data in the destination block since the cache is non-inclusive
         if (fill_ready) {
            if (dest_block->valid)
               increment_eviction(dest_index);

            override_block(src_block, dest_block);
            lru_update(dest_index, dest_way, src_block->lru);
         }
         
         // invalidate the source block
         src_block->valid = 0;

         // update extra read statistics
         num_read++;
      }
   }
   map_ptr++;
}

bool EntropyV2::remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level) {
   if (!dest->valid)
      return true;
   
   bool overwrite_dest = true;

   // prefetch -> dirty
   if (!dest->prefetch) {
      // dest NOT prefetch but the src is, drop the src, regardless of src dirty state
      if (src->prefetch) {
         num_drop++;
         overwrite_dest = false;
         if (src->dirty) {
            bool writeback_success = force_writeback(src, lower_level, FILL_LEVEL, CPU_ID);
            if (writeback_success) {
               num_writeback++;
            }
         }
      }
   }
   else if (dest->dirty) {
      // dest is prefetch dirty
      if (!src->dirty) {
         // if the destination block is dirty but the source block is clean, drop the source block (do not fill)
         num_drop++;
         overwrite_dest = false;
      }
      else { // if both blocks are dirty, write back the destination block then move the source block to the destination
         bool writeback_success = force_writeback(dest, lower_level, FILL_LEVEL, CPU_ID);
         if (writeback_success) {
            num_writeback++;    // update extra writeback statistics
         }
      }
   }
   else {
      // dest prefetch clean -> just overwrite, no further action
   }
   return overwrite_dest;
}

void EntropyV2::etp_tracker_access_update(uint64_t address) {

   /*
    * update correlation trackers
    * rules:
    * - if bit_i and bit_j move in the same fashion (both flip, both not flip): correl_i_j = 1
    * - else: correl_i_j = 0
    */ 

   bool should_reset_correl = false;
   for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
      // do not update the trackers if curr_addr == last_addr
      if ((m_addr_hist[idx] == 0) || (address == m_addr_hist[idx]))
         continue;

      uint64_t bit_flips = address ^ m_addr_hist[idx];
      uint32_t correl_table_idx = 0;
      for (uint32_t i=1; i < MAX_ADDRESS_BITS; ++i) {
         for (uint32_t j=0; j < i; ++j) {
            uint32_t bit_i_j_flip_correl = ((bit_flips >> i) & 1) ^ ((bit_flips >> j) & 1);
            if (bit_i_j_flip_correl == 0) {
               correl_table[correl_table_idx].m_entropy++;
               if (correl_table[correl_table_idx].m_entropy == CORREL_CTR_MAX)
                  should_reset_correl = true;
            }
            correl_table_idx++;
         }
      }
   }

   // update entropy trackers
   bool should_reset_entropy = false;
   for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
      // do not update the trackers if curr_addr == last_addr
      if (address == m_addr_hist[idx])
         continue;
      
      // compare curr_addr vs. last_addr bit by bit
      for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
         if (((address >> i) & 1) ^ ((m_addr_hist[idx] >> i) & 1)) {    // A ^ B == 1 => A != B => high entropy => increment entropy
            itv_entropy[i].m_entropy++;
            if (itv_entropy[i].m_entropy == ENTROPY_CTR_MAX) {
               should_reset_entropy = true;
            }
         }
      }
   }

   bool duplicate_addr = false;
   for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
      if (address == m_addr_hist[idx])
         duplicate_addr = true;
   }

   // save curr_addr
   if (!duplicate_addr) {
      m_addr_hist.push_back(address);
      while (m_addr_hist.size() > HISTORY_LENGTH)
         m_addr_hist.erase(m_addr_hist.begin());
   }

   if (should_reset_entropy) {
      cout << "ITV reset" << endl;
      for (uint32_t i=0; i < itv_entropy.size(); ++i) {
         itv_entropy[i].m_entropy /= 2;
      }
   }

   if (should_reset_correl) {
      cout << "Correlation Table reset" << endl;
      for (uint32_t i=0; i < correl_table.size(); ++i) {
         correl_table[i].m_entropy /= 2;
      }
   }
}

// return the correlation counter between bit A and bit B in the correlation table
uint32_t EntropyV2::get_correl(uint32_t bit_a, uint32_t bit_b) {
   vector<uint32_t> pair_id     = {bit_a, bit_b};
   vector<uint32_t> pair_id_rev = {bit_b, bit_a};
   for (uint32_t idx=0; idx < correl_table.size(); ++idx) {
      if (correl_table[idx].is_id_matched(pair_id) || correl_table[idx].is_id_matched(pair_id_rev)) {
         return correl_table[idx].m_entropy;
      }
   }
}

// find the lowest correlated bit in the input sequence for the input bit
uint32_t EntropyV2::find_min_correl_bit_from_sequence(uint32_t bit_a, vector<uint32_t>& sequence) {
   uint32_t min_correl = get_correl(bit_a, sequence[0]);
   uint32_t min_correl_idx = 0;
   for (uint32_t i=1; i < sequence.size(); ++i) {\
      uint32_t bitpair_correl = get_correl(bit_a, sequence[i]);
      if (bitpair_correl < min_correl) {
         min_correl = bitpair_correl;
         min_correl_idx = i;
      }
   }
   return min_correl_idx;
}

// etp2r14
void EntropyV2::compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
   // size of the bit sequence being tracked for entropy must greater than the size of the index sequences
   assert((entropy.size() > x_seq_a.size()) && (entropy.size() > x_seq_b.size()));

   // sort the index bits based on their entropy, from lowest to highest
   sort(entropy.begin(), entropy.end());
   cout << "Sorted ITV:       " << etp_vector_to_string(entropy) << endl;

   // // ETP2 candidate:
   // vector<uint32_t> c1_seq_a = x_seq_a;
   // vector<uint32_t> c1_seq_b = x_seq_b;

   // // ETP2 candidate: gather the highest and next highest entropy bits to form sequence A and B
   // for (uint32_t i=0; i < x_seq_a.size(); ++i) {
   //    c1_seq_a[i] = entropy[entropy.size()-1-i].m_id;
   //    c1_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
   // }

   // // ETP2 candidate: re-order sequence B based on the bitwise correlation counters
   // vector<uint32_t> reordered_indices = {};
   // for (uint32_t i=0; i < c1_seq_a.size(); ++i) {
   //    vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
   //    for (uint32_t j=0; j < c1_seq_b.size(); ++j) {
   //       seq_a_bit_correl.push_back(get_correl(c1_seq_a[i], c1_seq_b[j]));
   //    }
   //    uint32_t min_correl_idx = find_min_index(seq_a_bit_correl, reordered_indices);
   //    reordered_indices.push_back(i);

   //    // pair this bit in A with the least correlated bit in B
   //    if (min_correl_idx != i)
   //       swap(c1_seq_b[i], c1_seq_b[min_correl_idx]);
   // }
   // cout << endl;
   // cout << "c1_seq_a:  " << vec_to_string(c1_seq_a) << endl;
   // cout << "c1_seq_b:  " << vec_to_string(c1_seq_b) << endl;
   // uint32_t c1_corr = compute_sum_correl_two_sequences(c1_seq_a, c1_seq_b);
   // cout << "CORR c1:   " << c1_corr << endl;

   // ETP2r7 candidate:
   vector<uint32_t> c2_seq_a(x_seq_a.size(), 0);
   vector<uint32_t> c2_seq_b(x_seq_b.size(), 0);

   // ETP2r7 candidate: gather the highest entropy bits
   vector<uint32_t> max_etp_bits;
   for (uint32_t i=0; i < x_seq_a.size()*2; ++i) {
      max_etp_bits.push_back(entropy[entropy.size()-1-i].m_id);
   }
   cout << endl;
   cout << "max_etp_bits: " << vec_to_string(max_etp_bits) << endl;

   for (uint32_t idx=0; idx < c2_seq_a.size(); ++idx) {
      // get the highest entropy bit and and its lowest correlated pair.
      c2_seq_a[idx] = max_etp_bits[0];             // save it in sequence A
      max_etp_bits.erase(max_etp_bits.begin());   // erase it in max_etp_bits

      // find its corresponding lowest correlation bit and save in B
      uint32_t lowest_corr_idx = find_min_correl_bit_from_sequence(c2_seq_a[idx], max_etp_bits);
      c2_seq_b[idx] = max_etp_bits[lowest_corr_idx];
      max_etp_bits.erase(max_etp_bits.begin()+lowest_corr_idx);   // erase it in max_etp_bits
   }
   cout << "c2_seq_a:  " << vec_to_string(c2_seq_a) << endl;
   cout << "c2_seq_b:  " << vec_to_string(c2_seq_b) << endl;
   uint32_t c2_corr = compute_sum_correl_two_sequences(c2_seq_a, c2_seq_b);
   cout << "CORR c2:   " << c2_corr << endl << endl;

   // ETP candidate:
   vector<uint32_t> c3_seq_a = x_seq_a;
   vector<uint32_t> c3_seq_b = x_seq_b;

   // ETP candidate: gather the highest and next highest entropy bits to form sequence A and B
   for (uint32_t i=0; i < x_seq_a.size(); ++i) {
      c3_seq_a[i] = entropy[entropy.size()-1-i].m_id;
      c3_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
   }
   cout << "c3_seq_a:  " << vec_to_string(c3_seq_a) << endl;
   cout << "c3_seq_b:  " << vec_to_string(c3_seq_b) << endl;
   uint32_t c3_corr = compute_sum_correl_two_sequences(c3_seq_a, c3_seq_b);
   cout << "CORR c3:   " << c3_corr << endl;

   if (c2_corr <= c3_corr) {
      x_seq_a = c2_seq_a;
      x_seq_b = c2_seq_b;
      cout << "Choose C2" << endl;
   }
   else {
      x_seq_a = c3_seq_a;
      x_seq_b = c3_seq_b;
      cout << "Choose C3" << endl;
   }
   cout << endl;


   double next_corr = (double)compute_sum_correl_two_sequences(x_seq_a, x_seq_b);
   double last_corr = (double)compute_sum_correl_two_sequences(m_last_seq_a, m_last_seq_b);
   double corr_diff = (last_corr-next_corr)*100/last_corr;

   cout << "CORR Last Cfg:   " << last_corr << endl;
   cout << "CORR Next Cfg:   " << next_corr << endl;
   cout << "CORR difference: " << corr_diff << endl;

   if (num_interval < WARMUP_INTERVALS) {}
   // exceed the threshold + not decided to revert to nohash + not previously nohash (is_zero_segment(m_curr_seq_b))
   else if (!RECONFIG_NO_DELAY && (SENSE_THRESHOLD >= 0) && !is_zero_segment(m_curr_seq_b)) {
      // compute next_etp
      double next_etp = 0;
      for (uint32_t i=0; i < x_seq_a.size(); ++i) {
         next_etp += itv_entropy[x_seq_a[i]].m_entropy;
      }
      
      for (uint32_t i=0; i < x_seq_b.size(); ++i) {
         next_etp += itv_entropy[x_seq_b[i]].m_entropy;
      }
      
      // compute curr_etp
      double curr_etp = 0;
      for (uint32_t i=0; i < m_curr_seq_a.size(); ++i) {
         curr_etp += itv_entropy[m_curr_seq_a[i]].m_entropy;
      }
      
      for (uint32_t i=0; i < m_curr_seq_b.size(); ++i) {
         curr_etp += itv_entropy[m_curr_seq_b[i]].m_entropy;
      }
      
      // compute etp difference
      double etp_diff = (next_etp-curr_etp)*100/abs(curr_etp);

      // compare with threshold
      cout << "seq_A current:   " << vec_to_string(m_curr_seq_a) << endl;
      cout << "seq_B current:   " << vec_to_string(m_curr_seq_b) << endl;
      cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
      cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
      cout << std::setprecision(0) << "Current Cfg ETP:  " << curr_etp << endl;
      cout << std::setprecision(0) << "Next Cfg ETP:     " << next_etp << endl;
      if (etp_diff >= SENSE_THRESHOLD) {
         cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% >= " << SENSE_THRESHOLD << "%. Switch to new candidate." << endl;
      }
      else {
         cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% < " << SENSE_THRESHOLD << "%. Keep current candidate." << endl;
         x_seq_a = m_curr_seq_a;
         x_seq_b = m_curr_seq_b;
      }
      cout << std::setprecision(5);
   }
   else if (RECONFIG_NO_DELAY && (SENSE_THRESHOLD >= 0) && !is_zero_segment(m_last_seq_b)) {
      // compute next_etp
      double next_etp = 0;
      for (uint32_t i=0; i < x_seq_a.size(); ++i) {
         next_etp += itv_entropy[x_seq_a[i]].m_entropy;
      }
      
      for (uint32_t i=0; i < x_seq_b.size(); ++i) {
         next_etp += itv_entropy[x_seq_b[i]].m_entropy;
      }
      
      // compute last_etp
      double last_etp = 0;
      for (uint32_t i=0; i < m_last_seq_a.size(); ++i) {
         last_etp += itv_entropy[m_last_seq_a[i]].m_entropy;
      }
      
      for (uint32_t i=0; i < m_last_seq_b.size(); ++i) {
         last_etp += itv_entropy[m_last_seq_b[i]].m_entropy;
      }
      
      // compute etp difference
      double etp_diff = (next_etp-last_etp)*100/abs(last_etp);

      // compare with threshold
      cout << "seq_A last:   " << vec_to_string(m_last_seq_a) << endl;
      cout << "seq_B last:   " << vec_to_string(m_last_seq_b) << endl;
      cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
      cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
      cout << std::setprecision(0) << "Last Cfg ETP: " << last_etp << endl;
      cout << std::setprecision(0) << "Next Cfg ETP: " << next_etp << endl;
      if (etp_diff > SENSE_THRESHOLD && corr_diff > SENSE_THRESHOLD) {
         cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% > " << SENSE_THRESHOLD << "%. CORR difference: " << corr_diff << "% > " << SENSE_THRESHOLD << "%. Switch to new candidate." << endl;
      }
      else {
         if (etp_diff <= SENSE_THRESHOLD)
            cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% <= " << SENSE_THRESHOLD << "%." << endl;
         if (corr_diff <= SENSE_THRESHOLD)
            cout << std::setprecision(2) << "CORR difference: " << corr_diff << "% <= " << SENSE_THRESHOLD << "%." << endl;
         cout << "Keep last candidate.\n";

         x_seq_a = m_last_seq_a;
         x_seq_b = m_last_seq_b;
      }
      cout << std::setprecision(5);
   }
   return;
}

uint32_t EntropyV2::compute_sum_correl_two_sequences(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
   uint32_t sum_correl = 0;
   for (uint32_t i=0; i < x_seq_a.size(); ++i) {
      sum_correl += get_correl(x_seq_a[i], x_seq_b[i]);
   }
   return sum_correl;
}

void EntropyV2::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
   uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
   uint32_t hashed_index   = get_set(address);
   N_ACCESS[unhashed_index]++;
   PI_N_ACCESS[unhashed_index]++;
   E_ACCESS[hashed_index]++;
   PI_E_ACCESS[hashed_index]++;
   if (!is_hit) {
      MISS[unhashed_index]++;
      PI_MISS[unhashed_index]++;
   }
   access_ctr++;

   if (should_update(is_hit, type))
      etp_tracker_access_update(address);


   // should remap when last index config != current index config
   if (m_should_remap) {
      // if all sets have been remapped, skip this step
      if ((REMAP_INTERVAL > 0) && (access_ctr % REMAP_INTERVAL == 0) && (map_ptr < NUM_SET)) {
         move_cache_data(dram);
         if (map_ptr == NUM_SET)
               m_should_remap = false; // remapping done
      }
   }

   if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
      cout << endl << endl;
      cout << "========================================================================" << endl;
      cout << "End of interval # " << num_interval << " Summary" << endl;
      cout << "========================================================================" << endl;
      cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
      cout << "   entropy_stdev:  " << std::fixed << compute_std_dev(PI_E_ACCESS, NUM_SET) << endl;
      cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
      cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
      cout << "   map_ptr:        " << map_ptr << endl;
      cout << "========================================================================" << endl;

      // save the current XOR-hash index config
      m_last_seq_a = m_curr_seq_a;
      m_last_seq_b = m_curr_seq_b;

      // compute new XOR-hash index config for new interval
      if (RECONFIG_NO_DELAY) {
         // update trackers
         etp_tracker_interval_update();
         compute_new_config(itv_entropy, m_curr_seq_a, m_curr_seq_b);
      }
      else {
         m_curr_seq_a = m_next_seq_a;
         m_curr_seq_b = m_next_seq_b;

         // update trackers
         etp_tracker_interval_update();
         compute_new_config(itv_entropy, m_next_seq_a, m_next_seq_b);
      }

      m_should_remap = check_remap();

      cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
      cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
      cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
      cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
      if (!RECONFIG_NO_DELAY) {
         cout << "m_next_seq_a: " << vec_to_string(m_next_seq_a) << endl;
         cout << "m_next_seq_b: " << vec_to_string(m_next_seq_b) << endl;
      }
      cout << "should_remap: " << (m_should_remap ? "Yes" : "No") << endl << endl;

      memcpy(PAST_PI_EVICTION, PI_EVICTION, NUM_SET * sizeof(*PI_N_ACCESS));
      memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
      memset(PI_E_ACCESS, 0, NUM_SET * sizeof(*PI_E_ACCESS));
      memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
      memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
      
      num_interval++;
      map_ptr    = 0;
      access_ctr = 0;
   }

   warmup_check();
}

bool EntropyV2::should_update(bool is_hit, uint32_t type) {
   if (UPDATE_MISS_ONLY && is_hit)
      return false;
   if (SKIP_UPDATE_PF && (type == PREFETCH))
      return false;
   return true;
}