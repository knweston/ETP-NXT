#include "index/buddy.h"

uint32_t BuddyIndex::get_set(uint64_t address) {
   return compute_index(address);
}



void BuddyIndex::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
   // if ((type == WRITEBACK) && is_hit) {
   //    access_ctr++;
   //    return;
   // }

   uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
   uint32_t hashed_index   = get_set(address);
   N_ACCESS[unhashed_index]++;
   PI_N_ACCESS[unhashed_index]++;

   if ((type != WRITEBACK) || !is_hit) {
      B_ACCESS[hashed_index]++;
      PI_B_ACCESS[hashed_index]++;
      if (!is_hit) {
         MISS[hashed_index]++;
         PI_MISS[hashed_index]++;
      }
   }
   access_ctr++;

   if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
      double* MISS_RATE = new double [NUM_SET];
      normalize_array(PI_MISS, MISS_RATE, NUM_SET);

      double* ACCESS_RATE = new double [NUM_SET];
      normalize_array(PI_B_ACCESS, ACCESS_RATE, NUM_SET);

      uint64_t* ACCESS_MISS_CROSSRATE = new uint64_t [NUM_SET];
      for (uint32_t set=0; set < NUM_SET; ++set) {
         ACCESS_MISS_CROSSRATE[set] = (uint64_t) (ACCESS_RATE[set]*MISS_RATE[set]*MISS_RATE[set]*10000);
      }

      cout << endl << endl;
      cout << "========================================================================" << endl;
      cout << "End of interval # " << num_interval << " Summary" << endl;
      cout << "========================================================================" << endl;
      cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
      cout << "   buddy_stdev:    " << std::fixed << compute_std_dev(PI_B_ACCESS, NUM_SET) << endl;
      cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
      cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
      cout << "========================================================================" << endl;
      vector<uint32_t> replaced_sets = buddy_table_interval_update(ACCESS_MISS_CROSSRATE);
      cout << "========================================================================" << endl;
      cout << "Accesses:" << endl;
      print_buddy_interval_stats(PI_B_ACCESS);
      cout << "Misses:" << endl;
      print_buddy_interval_stats(PI_MISS);
      cout << "Access-Miss Cross-rate:" << endl;
      print_buddy_interval_stats(ACCESS_MISS_CROSSRATE);
      cout << "========================================================================" << endl;
      printf("Num replaced sets: %d. Aggregated evictions: %d, writebacks: %d. Aggregated buddy hits: %d, buddy spills: %d\n", replaced_sets.size(), num_drop, num_writeback, num_buddy_hit, num_spills);
      m_table.print();

      // flush the cache lines of the old mappings
      for (uint32_t i=0; i < replaced_sets.size(); ++i) {
         uint32_t replaced_idx = replaced_sets[i];
         for (uint32_t way=NUM_WAY-NUM_SHARE_WAY; way < NUM_WAY; ++way) {
            BLOCK* replaced_block = &block_array[replaced_idx][way];
            if (replaced_block->valid) {
               if (replaced_block->dirty) {
                  bool writeback_success = force_writeback(replaced_block, dram, FILL_LEVEL, CPU_ID);
                  num_writeback++;
               }
               replaced_block->valid = 0;
               num_drop++;
            }
         }
      }

      for (uint32_t entry=0; entry < LKUP_TABLE_SIZE; ++entry) {
         int32_t dst_id = m_table.get_entry(entry).dst_id();
         if (dst_id < 0) { // unused entry
            continue;
         }

         for (uint32_t i=NUM_WAY-NUM_SHARE_WAY; i<NUM_WAY; ++i) {
            BLOCK* shared_way = &block_array[dst_id][i];

            // demote the shared way
            for (uint32_t j=0; j<NUM_WAY; ++j) {
               if (block_array[dst_id][j].lru > shared_way->lru) {
                  block_array[dst_id][j].lru--;
               }
            }
            shared_way->lru = NUM_WAY-1;

            // evict the shared way
            if (shared_way->valid) {
               if (shared_way->dirty) {
                  bool writeback_success = force_writeback(shared_way, dram, FILL_LEVEL, CPU_ID);
                  num_writeback++;
               }
               shared_way->valid = 0;
               num_drop++;
            }
         }
      }

      memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
      memset(PI_B_ACCESS, 0, NUM_SET * sizeof(*PI_B_ACCESS));
      memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
      memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

      delete[] MISS_RATE;
      delete[] ACCESS_RATE;
      delete[] ACCESS_MISS_CROSSRATE;
      
      
      num_interval++;
      map_ptr    = 0;
      access_ctr = 0;
   }
   
   if (!idx_warmup_complete) {
      if (compute_sum(N_ACCESS, NUM_SET) >= num_warmup_access) {
         cout << "total access: " << compute_sum(N_ACCESS, NUM_SET) << endl;
         finish_warmup();
         memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
         memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
         memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
         memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
         memset(MISS, 0, NUM_SET * sizeof(*MISS));
         memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
         memset(B_ACCESS, 0, NUM_SET * sizeof(*B_ACCESS));
         memset(PI_B_ACCESS, 0, NUM_SET * sizeof(*PI_B_ACCESS));
         access_ctr = 0;
         idx_warmup_complete = true;   
      }
   }
}



void BuddyIndex::print_stats() {
   cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(B_ACCESS, NUM_SET) / compute_mean(B_ACCESS, NUM_SET) << endl;
   cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
   cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
   cout << "Ovr. Writebacks:          " << num_writeback << endl;
   cout << "Ovr. Buddy hits:          " << num_buddy_hit << endl;
   cout << "PER-SET EVICTION STATS:   " << arr_to_string(EVICTION, NUM_SET, ", ") << endl;
   cout << "PER-SET ACCESS STATS:     " << arr_to_string(B_ACCESS, NUM_SET, ", ") << endl;
}

void BuddyIndex::print_buddy_interval_stats(uint64_t* counter_arr) {
   vector<uint64_t> src_stats, dest_stats, src_idx, dst_idx;
   if (LKUP_TABLE_SIZE == 0) {
      return;
   }
   for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      int32_t src_id = m_table.get_entry(i).src_id();
      int32_t dst_id = m_table.get_entry(i).dst_id();
      if (src_id >= 0 && dst_id >= 0) {
         src_stats.push_back(counter_arr[src_id]);
         dest_stats.push_back(counter_arr[dst_id]);
      }
      src_idx.push_back(src_id);
      dst_idx.push_back(dst_id);
   }
   if (src_stats.size() > 0 && dest_stats.size() > 0) {
      cout << "src-set counters: " << vec_to_string(src_stats) << endl;
      cout << "bud-set counters: " << vec_to_string(dest_stats) << endl;
   }
}

uint32_t BuddyIndex::compute_index(uint64_t address) {
   // uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
   // uint32_t bitmask        = (address >> lg2(NUM_SET)) & ((1 << lg2(NUM_SET)) - 1);
   // return unhashed_index ^ bitmask;
   return (uint32_t) (address & ((1 << lg2(NUM_SET)) - 1));
}

uint32_t BuddyIndex::get_lru_victim(uint32_t set, uint32_t num_exclude) {
   uint32_t way = 0;

   // fill invalid line first
   for (way=0; way<(NUM_WAY-num_exclude); way++) {
      if (block_array[set][way].valid == false)
         break;
   }
   
   // LRU victim
   if (way == (NUM_WAY-num_exclude)) {
      for (way=0; way<NUM_WAY-num_exclude; way++) {
         if (block_array[set][way].lru == NUM_WAY-1-num_exclude)
            break;
      }
      increment_eviction(set);
   }
   
   if (way >= NUM_WAY) {
      cout << "ERROR: way_id out of bound = " << way << endl;
      assert(false);
   }
   return way;
}

void BuddyIndex::swap_block(BLOCK* src, BLOCK* des) {
   BLOCK* tmp_blk = new BLOCK();
   override_block(des, tmp_blk); // save dest to temp
   override_block(src, des);     // copy src to dest
   override_block(tmp_blk, src); // copy temp to src
   delete tmp_blk;
}

int BuddyIndex::check_hit(uint64_t address) {
   uint32_t set = compute_index(address);
   int src_index = m_table.is_buddy(set);

   for (uint32_t way=0; way < NUM_WAY; way++) {
      if (block_array[set][way].valid && (block_array[set][way].tag == address)) {
         return (int)way;
      }
   }

   int match_way = -1;
   int bud_index = m_table.find(set);
   bool hit_buddy_set = false;
   if (bud_index >= 0) {
      for (uint32_t way=NUM_WAY-NUM_SHARE_WAY; way < NUM_WAY; ++way) {
         if (block_array[bud_index][way].valid && (block_array[bud_index][way].tag == address)) {
            match_way = way;
            hit_buddy_set = true;
            break;
         }
      }
   }

   if (hit_buddy_set) {
      num_buddy_hit++;
      uint32_t src_lru_way = get_lru_victim(set);
      swap_block(&block_array[bud_index][match_way], &block_array[set][src_lru_way]);
      lru_update_buddy_region(bud_index, match_way);
      match_way = src_lru_way;
   }

   return match_way;
}

void BuddyIndex::lru_update_buddy_region(uint32_t set, uint32_t way) {
   for (uint32_t i=NUM_WAY-NUM_SHARE_WAY; i < NUM_WAY; ++i) {
      if (block_array[set][i].lru < block_array[set][way].lru) {
         block_array[set][i].lru++;
      }
   }
   block_array[set][way].lru = NUM_WAY-NUM_SHARE_WAY;
}

uint32_t BuddyIndex::get_buddy_victim(uint32_t set) {
   uint32_t way = 0;

   // fill invalid line first
   for (way=NUM_WAY-NUM_SHARE_WAY; way<NUM_WAY; way++) {
      if (block_array[set][way].valid == false)
         break;
   }

   if (way == NUM_WAY) {
      for (way=NUM_WAY-NUM_SHARE_WAY; way<NUM_WAY; way++) { 
         if (block_array[set][way].lru == NUM_WAY-1)
            break;
      }
   }
   if (way >= NUM_WAY) {
      cout << "ERROR: way_id out of bound = " << way << endl;
      assert(false);
   }
   return way;
}

uint32_t BuddyIndex::get_victim(uint32_t set) {
   int bud_index = m_table.find(set);
   int src_index = m_table.is_buddy(set);

   // this set does not have a buddy set and is not a buddy of any other set
   if ((bud_index < 0) && (src_index < 0)) { 
      return get_lru_victim(set);
   }

   // this set is a buddy of another set
   else if (src_index >= 0) {   
      return get_lru_victim(set, NUM_SHARE_WAY);
   }

   // this set has a buddy set
   else if (bud_index >= 0) {   
      uint32_t target_set_victim = get_lru_victim(set);
      if (block_array[set][target_set_victim].valid) {
         uint32_t buddy_set_victim = get_buddy_victim(bud_index);
         if (block_array[bud_index][buddy_set_victim].valid) {
            swap_block(&block_array[bud_index][buddy_set_victim], &block_array[set][target_set_victim]);
         }
         else {
            override_block(&block_array[set][target_set_victim], &block_array[bud_index][buddy_set_victim]);
            block_array[set][target_set_victim].valid = 0;
         }
         num_spills++;
         lru_update_buddy_region(bud_index, buddy_set_victim);

         increment_eviction(set);
      }
      return target_set_victim;
   }
   else {
      // we should not reach here
      assert(false);
   }
}

vector<uint32_t> BuddyIndex::buddy_table_interval_update(uint64_t* counter_arr) {
   vector<uint32_t> replaced_indices;
   vector<uint32_t> high_indices, low_indices;
   vector<uint64_t> high_counters, low_counters;
   if (LKUP_TABLE_SIZE == 0) {
      return replaced_indices;
   }

   // find indices with highest counters
   for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      uint32_t high_idx = find_max_index(counter_arr, NUM_SET, high_indices);
      high_indices.push_back(high_idx);
      high_counters.push_back(counter_arr[high_idx]);
   }

   // find indices with lowest counters
   for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      uint32_t low_idx = find_min_index(counter_arr, NUM_SET, low_indices);
      low_indices.push_back(low_idx);
      low_counters.push_back(counter_arr[low_idx]);
   }

   if (!ONETIME_PAIRING || (ONETIME_PAIRING && !table_loaded)) {
      uint32_t num_exists = 0;

      for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
         int32_t dst_id = m_table.get_entry(i).dst_id();
         int32_t src_id = m_table.get_entry(i).src_id();
         int32_t dst_found_idx = linear_search((uint32_t)dst_id, low_indices);
         int32_t src_found_idx = linear_search((uint32_t)src_id, high_indices);
         bool table_entry_found = ((dst_found_idx > 0) && (src_found_idx > 0));

         if (table_entry_found && (((low_counters[dst_found_idx] > 0) && ((high_counters[src_found_idx]*100/low_counters[dst_found_idx]-100) > MATCHING_MIN_DIFF)) 
                                || ((low_counters[dst_found_idx] == 0) && ((high_counters[src_found_idx]-low_counters[dst_found_idx]) > MATCHING_MIN_DIFF)))) 
         {
            printf("Existing entry found in table. Counter diff > %d(%). Skipping %4d:%d -> %4d:%d\n", 
                     MATCHING_MIN_DIFF, 
                     high_indices[src_found_idx], 
                     high_counters[src_found_idx],
                     low_indices[dst_found_idx],
                     low_counters[dst_found_idx]);

            high_indices.erase(high_indices.begin()+src_found_idx);
            low_indices.erase(low_indices.begin()+dst_found_idx);
            high_counters.erase(high_counters.begin()+src_found_idx);
            low_counters.erase(low_counters.begin()+dst_found_idx);

            num_exists++;
            m_table.swap_entry(i, LKUP_TABLE_SIZE-num_exists);
         }
      }

      uint32_t num_remaining = LKUP_TABLE_SIZE-num_exists;

      for (uint32_t i=0; i < num_remaining; ++i) {
         if (((low_counters[i] > 0) && ((high_counters[i]*100/low_counters[i]-100) > MATCHING_MIN_DIFF))
            || ((low_counters[i] == 0) && ((high_counters[i]-low_counters[i]) > MATCHING_MIN_DIFF))) 
         {
            int32_t replaced_index = m_table.update_entry(i, high_indices[i], low_indices[i]);
            if (replaced_index >= 0)
               replaced_indices.push_back(replaced_index);
         }

         else {
            printf("Skipped. Counter diff <= %d(%): %4d:%d -> %4d:%d\n", 
                     MATCHING_MIN_DIFF, 
                     high_indices[i], high_counters[i],
                     low_indices[i], low_counters[i]);
         }
         table_loaded = true;
      }

      // for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      //    if (((low_counters[i] > 0) && ((high_counters[i]*100/low_counters[i]-100) > MATCHING_MIN_DIFF))
      //       || ((low_counters[i] == 0) && ((high_counters[i]-low_counters[i]) > MATCHING_MIN_DIFF))) 
      //    {
      //       int32_t replaced_index = m_table.update_entry(i, high_indices[i], low_indices[i]);
      //       if (replaced_index >= 0)
      //          replaced_indices.push_back(replaced_index);
      //    }

      //    else {
      //       printf("Skipped. Counter diff <= %d(%): %4d:%d -> %4d:%d\n", 
      //                MATCHING_MIN_DIFF, 
      //                high_indices[i], high_counters[i],
      //                low_indices[i], low_counters[i]);
      //    }
      //    table_loaded = true;
      // }
   }

   return replaced_indices;
}