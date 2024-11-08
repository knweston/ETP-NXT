#ifndef SKEWED_INDEX_H
#define SKEWED_INDEX_H

#include "index/index.h"

class SkewedIndex : public Index {
public:
   SkewedIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length, BLOCK **arr)
      :Index(x_numset, x_numway, x_interval_length), block_array(arr)
   {
      m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), 0, lg2(NUM_SET)));
      reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
      

      for (uint32_t way=0; way < NUM_WAY; ++way) {
         m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), way));
         reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
      }

      for (uint32_t layer=0; layer < m_shuffled_bit_sels.size(); ++layer) {
         cout << "m_shuffled_bit_sels " << layer << ": ";
         for (uint32_t i=0; i < m_shuffled_bit_sels[layer].size(); ++i)
            cout << m_shuffled_bit_sels[layer][i] << " ";
         cout << endl;
      }
   }
   ~SkewedIndex() {}

   uint32_t get_set(uint64_t address);
   void     print_stats() {}
   vector<uint32_t> get_bitmask(uint32_t bitmask_id) { return m_shuffled_bit_sels[bitmask_id]; }

   virtual uint32_t compute_index(uint64_t address, vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b);
   virtual bool     lookup(uint64_t address, uint32_t* set_id, uint32_t* way_id);
   virtual void     get_victim(uint64_t address, uint32_t* set_id, uint32_t* way_id);

protected:
   BLOCK**  block_array; // ptr to the cache set data array
   vector<vector<uint32_t>> m_shuffled_bit_sels;
};

#endif // SKEWED_INDEX_H