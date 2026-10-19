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
        uint32_t hashed_index = compute_index(address, way);
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

uint32_t SkewedIndex::compute_index(uint64_t address, uint32_t way) {
#if SKEWED_INDEX_FORMULA == 2
    // Ways 0-9 use H; ways 10-15 use the next higher chunk H2.
    const vector<uint32_t>& x_seq_a = way < 10 ? m_shuffled_bit_sels[0] : m_shuffled_bit_sels.back();
#else
    const vector<uint32_t>& x_seq_a = m_shuffled_bit_sels[0];
#endif
    const vector<uint32_t>& x_seq_b = m_shuffled_bit_sels[way+1];
    uint32_t index_a = 0;
    uint32_t index_b = 0;
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        index_a = (index_a << 1) | ((address >> x_seq_a[i]) & 1);
        index_b = (index_b << 1) | ((address >> x_seq_b[i]) & 1);
    }

#if SKEWED_INDEX_FORMULA == 1
    // index_a is L; index_b is perfshuff^(way/2)(H).
    uint32_t mask = (way & 1) ? ((NUM_SET - 1) ^ m_mask_t) : m_mask_t;
    return index_a ^ (index_b & mask);
#else
    // Formula 0 uses H and perfshuff^way(L); formula 2 uses H/H2 with its per-way L permutation.
    return index_a ^ index_b;
#endif
}
