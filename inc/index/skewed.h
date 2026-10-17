#ifndef SKEWED_INDEX_H
#define SKEWED_INDEX_H

#include "index/index.h"

// 0: H XOR perfshuff^way(L)
// 1: L XOR (perfshuff^(way/2)(H) AND mask), with T on even ways
//    and its index-width complement on odd ways.
#ifndef SKEWED_INDEX_FORMULA
#define SKEWED_INDEX_FORMULA 1
#endif

#if SKEWED_INDEX_FORMULA != 0 && SKEWED_INDEX_FORMULA != 1
#error "SKEWED_INDEX_FORMULA must be 0 (old) or 1 (paired masks)"
#endif

class SkewedIndex : public Index {
public:
    SkewedIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length, BLOCK **arr)
        :Index(x_numset, x_numway, x_interval_length), block_array(arr), m_mask_t(0),
        m_tie_break_state(0x9e3779b9u), m_last_victim_valid(false),
        m_last_victim_address(0), m_last_invalid_mask(0), m_last_ru0_mask(0),
        m_last_clean_mask(0), m_last_dirty_mask(0), m_last_victim_set(0), m_last_victim_way(0)
    {
        cout << "SKEWED_INDEX_FORMULA: " << (SKEWED_INDEX_FORMULA ? "new" : "old") << endl;

#if SKEWED_INDEX_FORMULA == 1
        assert(NUM_SET && !(NUM_SET & (NUM_SET - 1)) &&
            "Paired skewed hashing requires a power-of-two number of sets");
        for (uint32_t bit=0; bit < (uint32_t)lg2(NUM_SET); bit+=2)
            m_mask_t |= uint32_t{1} << bit;
        cout << "Skewed T: " << m_mask_t
            << ". complement: " << ((NUM_SET - 1) ^ m_mask_t) << endl;

        // The common half is L; each adjacent pair permutes H identically.
        m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), 0));
        reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
        for (uint32_t way=0; way < NUM_WAY; ++way) {
            m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), way/2, lg2(NUM_SET)));
            reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
        }
#else
        // The original formula keeps H fixed and permutes L once per way.
        m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), 0, lg2(NUM_SET)));
        reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
        for (uint32_t way=0; way < NUM_WAY; ++way) {
            m_shuffled_bit_sels.push_back(perfect_shuffle(lg2(NUM_SET), way));
            reverse(m_shuffled_bit_sels.back().begin(), m_shuffled_bit_sels.back().end());
        }
#endif

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

    virtual uint32_t compute_index(uint64_t address, uint32_t way);
    virtual bool     lookup(uint64_t address, uint32_t* set_id, uint32_t* way_id);
    virtual void     get_victim(uint64_t address, uint32_t* set_id, uint32_t* way_id);

protected:
    BLOCK**  block_array; // ptr to the cache set data array
    vector<vector<uint32_t>> m_shuffled_bit_sels;
    uint32_t m_mask_t;

    // Cache the last selection because get_set() and llc_find_victim() both
    // ask for a victim for the same miss.
    uint32_t m_tie_break_state;
    bool m_last_victim_valid;
    uint64_t m_last_victim_address;
    uint64_t m_last_invalid_mask;
    uint64_t m_last_ru0_mask;
    uint64_t m_last_clean_mask;
    uint64_t m_last_dirty_mask;
    uint32_t m_last_victim_set;
    uint32_t m_last_victim_way;
};

#endif // SKEWED_INDEX_H