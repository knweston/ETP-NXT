#include "index/buddy.h"

uint32_t BuddyIndex::get_set(uint64_t address) {
   /*
      search:
         - iteratively run the perfect shuffle function, find the index of the input address for each way, stop if hit
      return:
         - hit set if hit
         - victim set if missed (victim way is decided here)
   */ 
   uint32_t set_id = 0;
   uint32_t way_id = 0;
   bool is_hit = lookup(address, &set_id, &way_id);
   return set_id;
}

void BuddyIndex::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
   uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
   uint32_t hashed_index   = get_set(address);
   N_ACCESS[unhashed_index]++;
   PI_N_ACCESS[unhashed_index]++;
   B_ACCESS[hashed_index]++;
   PI_B_ACCESS[hashed_index]++;
   if (!is_hit) {
      MISS[unhashed_index]++;
      PI_MISS[unhashed_index]++;
   }
   access_ctr++;

   if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
      cout << endl << endl;
      cout << "========================================================================" << endl;
      cout << "End of interval # " << num_interval << " Summary" << endl;
      cout << "========================================================================" << endl;
      cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
      cout << "   buddy_stdev:    " << std::fixed << compute_std_dev(PI_B_ACCESS, NUM_SET) << endl;
      cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
      cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
      cout << "========================================================================" << endl;
      print_buddy_interval_stats(PI_EVICTION);
      cout << "========================================================================" << endl;
      vector<uint32_t> replaced_sets = buddy_table_interval_update(PI_EVICTION);

      // one-shot cache flush
      for (uint32_t i=0; i < replaced_sets.size(); ++i) {
         uint32_t replaced_idx = replaced_sets[i];
         for (uint32_t way=0; way < NUM_WAY; ++way) {
            BLOCK* replaced_block = &block_array[replaced_idx][way];
            if (replaced_block->valid && (compute_index(replaced_block->address) == replaced_idx)) {
               continue;
            }

            if (replaced_block->valid) {
               if (replaced_block->dirty) {
                  bool writeback_success = force_writeback(replaced_block, dram, FILL_LEVEL, CPU_ID);
                  num_writeback++;
               }
               else {
                  replaced_block->valid = 0;
               }
               // increment_eviction(i);  // this will change the eviction distribution, affecting future reconfiguration
            }
         }
      }
      printf("Num replaced sets: %d. Aggregated writebacks: %d. Aggregated buddy hits: %d. Aggregated buddy spills: %d\n", replaced_sets.size(), num_writeback, num_buddy_hit, num_spills);
      m_table.print();

      memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
      memset(PI_B_ACCESS, 0, NUM_SET * sizeof(*PI_B_ACCESS));
      memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
      memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
      
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
   vector<uint64_t> src_stats, dest_stats;
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
   }
   cout << "src-set evictions: " << vec_to_string(src_stats) << endl;
   cout << "bud-set evictions: " << vec_to_string(dest_stats) << endl;
}

vector<uint32_t> BuddyIndex::buddy_table_interval_update(uint64_t* counter_arr) {
   vector<uint32_t> replaced_indices;
   vector<uint32_t> high_indices, low_indices;
   vector<uint64_t> high_evictions, low_evictions;
   if (LKUP_TABLE_SIZE == 0) {
      return replaced_indices;
   }

   // find indices with highest evictions
   for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      uint32_t high_idx = find_max_index(counter_arr, NUM_SET, high_indices);
      high_indices.push_back(high_idx);
      high_evictions.push_back(counter_arr[high_idx]);
   }

   // find indices with lowest evictions
   for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
      uint32_t low_idx = find_min_index(counter_arr, NUM_SET, low_indices);
      low_indices.push_back(low_idx);
      low_evictions.push_back(counter_arr[low_idx]);
   }

   if (!ONESHOT_PAIRING || (ONESHOT_PAIRING && !table_loaded)) {
      for (uint32_t i=0; i < LKUP_TABLE_SIZE; ++i) {
         if (((low_evictions[i] > 0) && ((high_evictions[i]*100/low_evictions[i]-100) > MATCHING_MIN_DIFF))
            || ((low_evictions[i] == 0) && ((high_evictions[i]-low_evictions[i]) > MATCHING_MIN_DIFF))) 
         {
            int32_t replaced_index = m_table.update_entry(i, high_indices[i], low_indices[i]);
            if (replaced_index >= 0)
               replaced_indices.push_back(replaced_index);
         }
         else {
            printf("Skipped. Eviction diff < %d(%): %4d:%d -> %4d:%d\n", MATCHING_MIN_DIFF, high_indices[i],high_evictions[i],low_indices[i],low_evictions[i]);
         }
      }
      table_loaded = true;
   }

   cout << "high-idx evictions: " << vec_to_string(high_evictions) << endl;
   cout << "low-idx evictions:  " << vec_to_string(low_evictions) << endl;
   return replaced_indices;
}

bool BuddyIndex::lookup(uint64_t address, uint32_t* set_id, uint32_t* way_id) {
   /*
      search:
         - search in the source set
         - if hit, return hit index, hit way
         - if miss, search buddy set
         - if hit buddy set, swap hit way with the LRU way in source set.
         - if miss buddy set, return miss.
         - currently the function params set_id, way_id not used.
   */
   bool is_hit = false;
   uint32_t src_index = compute_index(address);

   for (uint32_t way=0; way < NUM_WAY; ++way) {
      is_hit = (block_array[src_index][way].valid && (block_array[src_index][way].tag == address));
      if (is_hit) {  // hit
         break;
      }
   }

   if (!is_hit) {    // miss
      int32_t bud_index = m_table.find(src_index);
      if (bud_index >= 0) {
         for (uint32_t way=0; way < NUM_WAY; ++way) {
            is_hit = (block_array[bud_index][way].valid && (block_array[bud_index][way].tag == address));
            if (is_hit) {  // hit
               num_buddy_hit++;
               uint32_t src_lru_way = get_lru_victim(src_index);
               swap_block(&block_array[bud_index][way], &block_array[src_index][src_lru_way]);
               lru_update(bud_index, way);   // move swapped block to the LRU head of the buddy set
               break;
            }
         }
      }
   }

   *set_id = src_index;
   return is_hit;
}

uint32_t BuddyIndex::compute_index(uint64_t address) {
   return (uint32_t) (address & ((1 << lg2(NUM_SET)) - 1));
}

uint32_t BuddyIndex::get_lru_victim(uint32_t set) {
   uint32_t way = 0;

   // fill invalid line first
   for (way=0; way<NUM_WAY; way++) {
      if (block_array[set][way].valid == false)
         break;
   }
   
   // LRU victim
   if (way == NUM_WAY) {
      for (way=0; way<NUM_WAY; way++) {
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

void BuddyIndex::lru_update(uint32_t set, uint32_t way) {
   for (uint32_t i=0; i<NUM_WAY; i++) {
      if (block_array[set][i].lru < block_array[set][way].lru) {
         block_array[set][i].lru++;
      }
   }
   // insert to the LRU position that the block is currently at
   block_array[set][way].lru = 0;
}

void BuddyIndex::swap_block(BLOCK* src, BLOCK* des) {
   BLOCK* tmp_blk = new BLOCK();
   override_block(des, tmp_blk); // save dest to temp
   override_block(src, des);     // copy src to dest
   override_block(tmp_blk, src); // copy temp to src
   delete tmp_blk;
}