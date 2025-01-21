#ifndef BUDDY_INDEX_H
#define BUDDY_INDEX_H

#include "index/index.h"

#define LKUP_TABLE_SIZE 16
#define MATCHING_MIN_DIFF 20
#define ONETIME_PAIRING 0

#define NUM_SHARE_WAY 4

class LkupEntry {
public:
   LkupEntry()
      :src_index(-1), dst_index(-1) {}

   LkupEntry(uint32_t x_src, uint32_t x_dst)
      :src_index(x_src), dst_index(x_dst) {}
   
   void src_id(int32_t x_id) { this->src_index = x_id; }
   int32_t src_id() { return this->src_index; }
   void dst_id(int32_t x_id) { this->dst_index = x_id; }
   int32_t dst_id() { return this->dst_index; }
   void print() { printf("set: %4d -> %4d\n", this->src_index, this->dst_index); }

protected:
   int32_t src_index;
   int32_t dst_index;
};

class LkupTable {
public:
   LkupTable() {}
   void init(uint32_t x_size) {
      for (uint32_t i=0; i<x_size; ++i) {
         m_entries.push_back(LkupEntry());
      }
   }

   void flush() {
      for (uint32_t i=0; i < m_entries.size(); ++i) {
         m_entries[i].src_id(-1);
         m_entries[i].dst_id(-1);
      }
   }

   int32_t update_entry(uint32_t x_idx, int32_t x_src, int32_t x_dst) {
      int32_t replaced_index = m_entries[x_idx].dst_id();
      m_entries[x_idx].src_id(x_src);
      m_entries[x_idx].dst_id(x_dst);
      return replaced_index;
   }

   void print(int32_t x_id=-1) {
      if (x_id >= 0) {
         m_entries[x_id].print();
      }
      else {
         for (uint32_t i=0; i < m_entries.size(); ++i) {
            m_entries[i].print();
         }
      }
   }

   int32_t find(uint32_t x_src) {
      for (uint32_t i=0; i < m_entries.size(); ++i) {
         if (m_entries[i].src_id() == x_src) {
            return m_entries[i].dst_id();
         }
      }
      return -1;
   }

   int32_t is_buddy(uint32_t x_dst) {
      for (uint32_t i=0; i < m_entries.size(); ++i) {
         if (m_entries[i].dst_id() == x_dst) {
            return m_entries[i].src_id();
         }
      }
      return -1;
   }

   LkupEntry get_entry(uint32_t x_id) { return m_entries[x_id]; }
   
   void swap_entry(uint32_t x_id_a, uint32_t x_id_b) {
      int32_t src_idx_a = m_entries[x_id_a].src_id();
      int32_t dst_idx_a = m_entries[x_id_a].dst_id();
      m_entries[x_id_a].src_id(m_entries[x_id_b].src_id());
      m_entries[x_id_a].dst_id(m_entries[x_id_b].dst_id());
      m_entries[x_id_b].src_id(src_idx_a);
      m_entries[x_id_b].dst_id(dst_idx_a);
      return;
   }

protected:
   vector<LkupEntry> m_entries;
};

class BuddyIndex : public Index {
public:
   BuddyIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
              uint32_t remap_length, uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu)
      :Index(x_numset, x_numway, x_interval_length),
       REMAP_INTERVAL(remap_length), FILL_LEVEL(x_fill_lvl), block_array(arr), CPU_ID(cpu)
   {
      m_table.init(LKUP_TABLE_SIZE);
      IDX_SIZE = lg2(NUM_SET);
      custom_checkhit = true;
      custom_getvictim = true;

      cout << endl;
      cout << "========================================================================" << endl;
      cout << "BUDDY INDEX CONFIGURATION" << endl;
      cout << "========================================================================" << endl;
      cout << "Remap interval:      " << REMAP_INTERVAL << endl;
      cout << "Interval size:       " << INTERVAL_SIZE << endl;
      cout << "Match min diff:      " << MATCHING_MIN_DIFF << " (%)" << endl;
      cout << "Table size:          " << LKUP_TABLE_SIZE << endl;
      cout << "Onetime pairing:     " << ONETIME_PAIRING << endl;
      cout << "Custom Check Hit:    " << custom_checkhit << endl;
      cout << "Custom Get Victim:   " << custom_getvictim << endl;
      cout << "NUM_SHARE_WAY:       " << NUM_SHARE_WAY << endl;
      cout << "========================================================================" << endl;

      B_ACCESS    = new uint64_t [NUM_SET];
      PI_B_ACCESS = new uint64_t [NUM_SET];
      memset(B_ACCESS, 0, NUM_SET * sizeof(*B_ACCESS));
      memset(PI_B_ACCESS, 0, NUM_SET * sizeof(*PI_B_ACCESS));
   }
   
   ~BuddyIndex() {}

   uint32_t         get_set(uint64_t address);
   void             update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
   void             print_stats();
   vector<uint32_t> buddy_table_interval_update(uint64_t* counter_arr);
   void             print_buddy_interval_stats(uint64_t* counter_arr);
   void             swap_block(BLOCK* src, BLOCK* des);
   uint32_t         get_lru_victim(uint32_t set, uint32_t num_exclude=0);
   uint32_t         compute_index(uint64_t address);
   int              check_hit(uint64_t address);
   uint32_t         get_victim(uint32_t set);

   uint32_t         get_buddy_victim(uint32_t set);
   void             lru_update_buddy_region(uint32_t set, uint32_t way);

public:
   LkupTable m_table;

   uint32_t IDX_SIZE;
   uint32_t REMAP_INTERVAL;
   uint32_t FILL_LEVEL;
   uint32_t CPU_ID;
   BLOCK**  block_array; // ptr to the cache set data array

   uint32_t access_ctr    = 0;
   uint32_t map_ptr       = 0;
   uint64_t num_writeback = 0;
   uint64_t num_drop      = 0;
   uint64_t num_read      = 0;
   uint64_t num_buddy_hit = 0;   // num hits in buddy set
   uint64_t num_spills    = 0;   // num victim spilled over to buddy set
   bool     table_loaded  = false;

   uint64_t* B_ACCESS;     
   uint64_t* PI_B_ACCESS;
};

#endif // BUDDY_INDEX_H