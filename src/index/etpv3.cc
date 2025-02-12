#include "index/etpv3.h"

void EntropyV3::etp_tracker_access_update(bool is_hit, uint32_t type, uint64_t address) {
   /*
    * update correlation trackers
    * rules:
    * - if bit_i and bit_j move in the same fashion (both flip, both not flip): correl_i_j = 1
    * - else: correl_i_j = 0
    */
   if (current_phase == Phase::DUEL) {
      for (uint32_t i=0; i < m_seq_a.size(); ++i) {
         int num_conflicts = 0;
         uint32_t curr_index = compute_index(address, m_seq_a[i].m_segid, m_seq_b[i].m_segid);
         vector<uint32_t> index_hist;
         
         for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
            index_hist.push_back(compute_index(m_addr_hist[idx], m_seq_a[i].m_segid, m_seq_b[i].m_segid));
         }
         
         // if there is a conflict => increment the entropy counter
         for (uint32_t idx=0; idx < index_hist.size(); ++idx) {
            if ((address != m_addr_hist[idx]) && (curr_index == index_hist[idx])) {
               m_seq_a[i].m_entropy++;
               m_seq_b[i].m_entropy++;
               // if (is_hit) {
               //    m_seq_a[i].m_entropy++;
               //    m_seq_b[i].m_entropy++;
               // }
               // else {
               //    m_seq_a[i].m_entropy += 2;
               //    m_seq_b[i].m_entropy += 2;
               // }
               // if (type == PREFETCH && type == WRITEBACK) {
               //    m_seq_a[i].m_entropy++;
               //    m_seq_b[i].m_entropy++;
               // }
               // else {
               //    m_seq_a[i].m_entropy += 2;
               //    m_seq_b[i].m_entropy += 2;
               // }
            }
         }
      }
   }

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

   if (should_reset_entropy) {
      num_reset_itv++;
      for (uint32_t i=0; i < itv_entropy.size(); ++i) {
         itv_entropy[i].m_entropy /= 2;
      }
   }

   if (should_reset_correl) {
      num_reset_cor++;
      for (uint32_t i=0; i < correl_table.size(); ++i) {
         correl_table[i].m_entropy /= 2;
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
}



void EntropyV3::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
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
      etp_tracker_access_update(is_hit, type, address);


   // should remap when last index config != current index config
   if (m_should_remap) {
      // if all sets have been remapped, skip this step
      if ((REMAP_INTERVAL > 0) && (access_ctr % REMAP_INTERVAL == 0) && (map_ptr < NUM_SET)) {
         move_cache_data(dram);
         if (map_ptr == NUM_SET)
            m_should_remap = false; // remapping done
      }
   }

   if ((current_phase == Phase::PROFILE) && (access_ctr > INTERVAL_SIZE*PROFILE_PHASE_RATIO)) {
      compute_candidate_config(itv_entropy, m_curr_seq_a, m_curr_seq_b);
      current_phase = Phase::DUEL;
      for (uint32_t i=0; i < m_seq_a.size(); ++i) {
         m_seq_a[i].m_entropy = 0;
         m_seq_b[i].m_entropy = 0;
      }
      cout << "\nEntering Dueling Phase" << endl;
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
      cout << "   num_reset_itv:  " << num_reset_itv << endl;
      cout << "   num_reset_cor:  " << num_reset_cor << endl;
      cout << "========================================================================" << endl;

      // save the current XOR-hash index config
      m_last_seq_a = m_curr_seq_a;
      m_last_seq_b = m_curr_seq_b;

      // compute new XOR-hash index config for new interval
      if (RECONFIG_NO_DELAY) {
         // update trackers
         etp_tracker_interval_update();
         compute_new_config(m_curr_seq_a, m_curr_seq_b);
      }
      else {
         m_curr_seq_a = m_next_seq_a;
         m_curr_seq_b = m_next_seq_b;

         // update trackers
         etp_tracker_interval_update();
         compute_new_config(m_next_seq_a, m_next_seq_b);
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

      current_phase = Phase::PROFILE;
      cout << "Entering Profiling Phase" << endl;
   }

   warmup_check();
}



void EntropyV3::compute_candidate_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
   // size of the bit sequence being tracked for entropy must greater than the size of the index sequences
   assert((entropy.size() > x_seq_a.size()) && (entropy.size() > x_seq_b.size()));

   // sort the index bits based on their entropy, from lowest to highest
   sort(entropy.begin(), entropy.end());
   cout << "\nSorted ITV: " << etp_vector_to_string(entropy) << endl;

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
      max_etp_bits.erase(max_etp_bits.begin());    // erase it in max_etp_bits

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

   // save the candidates to m_seq_a and m_seq_b vectors
   m_seq_a.push_back(c2_seq_a);
   m_seq_a.push_back(c3_seq_a);
   m_seq_b.push_back(c2_seq_b);
   m_seq_b.push_back(c3_seq_b);

   return;
}



void EntropyV3::compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
   sort(m_seq_a.begin(), m_seq_a.end());
   sort(m_seq_b.begin(), m_seq_b.end());

   cout << "Sorted CONFLICT_CTR SEQ_A: " << etp_vector_to_string(m_seq_a) << endl;
   cout << "Sorted CONFLICT_CTR SEQ_B: " << etp_vector_to_string(m_seq_b) << endl;

   // assign A and B to be the sequences with lowest conflicts
   if (num_interval < WARMUP_INTERVALS) {
      x_seq_a = m_seq_a.front().m_segid;
      x_seq_b = m_seq_b.front().m_segid;
   }

   else if (SENSE_THRESHOLD > 0) {
      double next_conflicts = m_seq_a.front().m_entropy;
      double curr_conflicts = 0;
      for (uint32_t i=0; i < m_seq_a.size(); ++i) {
         if (m_seq_a[i].is_id_matched(x_seq_a) && m_seq_b[i].is_id_matched(x_seq_b))
            curr_conflicts = m_seq_a[i].m_entropy;
      }
      double conflict_diff = (curr_conflicts-next_conflicts)*100/curr_conflicts;
      bool conflict_diff_significant = (curr_conflicts - next_conflicts) >= SENSE_DIFFLEVEL;
      cout << std::setprecision(0) << "Current Cfg Conflicts:  " << curr_conflicts << endl;
      cout << std::setprecision(0) << "Next Cfg Conflicts:     " << next_conflicts << endl;
      if ((conflict_diff >= SENSE_THRESHOLD) && conflict_diff_significant) {
         cout << std::setprecision(2) << "Conflict difference: " << conflict_diff << "% >= " << SENSE_THRESHOLD << "%" << " => Switch to new candidate." << endl;
         x_seq_a = m_seq_a.front().m_segid;
         x_seq_b = m_seq_b.front().m_segid;
      }
      else {
         if (conflict_diff < SENSE_THRESHOLD)
            cout << std::setprecision(2) << "Conflict difference: " << conflict_diff << "% < " << SENSE_THRESHOLD << "%";
         else
            cout << std::setprecision(2) << "Conflict difference: " << conflict_diff << "% >= " << SENSE_THRESHOLD << "%";

         if (conflict_diff_significant)
            cout << ". |Conflict difference| > SENSE_DIFFLEVEL => Keep current candidate." << endl;
         else
            cout << ". |Conflict difference| <= SENSE_DIFFLEVEL => Keep current candidate." << endl;
      }
      cout << std::setprecision(5);
   }

   else {
      x_seq_a = m_seq_a.front().m_segid;
      x_seq_b = m_seq_b.front().m_segid;
   }

   m_seq_a.resize(0);
   m_seq_b.resize(0);
   m_seq_a.push_back(x_seq_a);
   m_seq_b.push_back(x_seq_b);
   return;
}

bool EntropyV3::should_update(bool is_hit, uint32_t type) {
   // if (current_phase == Phase::DUEL && is_hit)
   //    return false;
   if (UPDATE_MISS_ONLY && is_hit)
      return false;
   if (SKIP_UPDATE_PF && (type == PREFETCH))
      return false;
   if (SKIP_UPDATE_WB && (type == WRITEBACK))
      return false;
   return true;
}