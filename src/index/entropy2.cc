#include "index/entropy2.h"

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
   
   if (!UPDATE_MISS_ONLY)
      etp_tracker_access_update(address);
   else
      if (!is_hit)
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

      // reset interval bit entropy
      for (uint32_t i=0; i < itv_entropy.size(); ++i) {
         itv_entropy[i].m_entropy = 0;
      } 
   }

   warmup_check();
}



void EntropyV2::compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
   // size of the bit sequence being tracked for entropy must greater than the size of the index sequences
   assert((entropy.size() > x_seq_a.size()) && (entropy.size() > x_seq_b.size()));

   // sort the index bits based on their entropy, from lowest to highest
   sort(entropy.begin(), entropy.end());
   cout << "Sorted ITV:       " << etp_vector_to_string(entropy) << endl;

   for (uint32_t i=0; i < x_seq_a.size(); ++i) {
      x_seq_a[i] = entropy[entropy.size()-1-i].m_id;
      
      if (FIX_SEQUENCE == 0)
         x_seq_b[i] = entropy[i].m_id;
      else if (FIX_SEQUENCE == 3)
         x_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
   }

   for (uint32_t i=0; i < x_seq_a.size(); ++i) {
      x_seq_a[i] = entropy[entropy.size()-1-i].m_id;
      x_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
   }

   if (num_interval < WARMUP_INTERVALS) {}
   // exceed the threshold + not decided to revert to nohash + not previously nohash (is_zero_segment(m_curr_seq_b))
   else if (!RECONFIG_NO_DELAY && (SENSE_THRESHOLD > 0) && !is_zero_segment(m_curr_seq_b)) {
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
   else if (RECONFIG_NO_DELAY && (SENSE_THRESHOLD > 0) && !is_zero_segment(m_last_seq_b)) {
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
      if (etp_diff >= SENSE_THRESHOLD) {
            cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% >= " << SENSE_THRESHOLD << "%. Switch to new candidate." << endl;
      }
      else {
            cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% < " << SENSE_THRESHOLD << "%. Keep last candidate." << endl;
            x_seq_a = m_last_seq_a;
            x_seq_b = m_last_seq_b;
      }
      cout << std::setprecision(5);
   }
   return;
}