#include "index/skewed.h"

uint32_t SkewedIndex::get_set(uint64_t address) {
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

bool SkewedIndex::lookup(uint64_t address, uint32_t* set_id, uint32_t* way_id) {
   /*
      search:
         - iteratively run the perfect shuffle function, find the index of the input address for each way, stop if hit
      return:
         - hit:  hit set + hit way
         - miss: victim set + victim way (victim way is decided here)
         - result: hit or miss
   */
   bool is_hit = false;

   for (uint32_t way=0; way < NUM_WAY; ++way) {
      uint32_t hashed_index = compute_index(address, m_shuffled_bit_sels[0], m_shuffled_bit_sels[way+1]);
      is_hit = (block_array[hashed_index][way].valid && (block_array[hashed_index][way].tag == address));
      if (is_hit) {  // hit
         *set_id = hashed_index;
         *way_id = way;
         break;
      }
   }

   if (!is_hit) {    // miss
      // find victim
      get_victim(address, set_id, way_id);
   }

   return is_hit;
}

uint32_t SkewedIndex::compute_index(uint64_t address, vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b) {
   uint32_t hashed_index = 0;
   for (uint32_t i=0; i < x_seq_a.size(); ++i) {
      uint32_t selected_bit_a = (address >> x_seq_a[i]) & 1;
      uint32_t selected_bit_b = (address >> x_seq_b[i]) & 1;
      hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
   }
   return hashed_index;
}