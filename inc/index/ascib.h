#ifndef ASCIB_INDEX_H
#define ASCIB_INDEX_H

#include "index/index.h"

// ASCIB phase lengths from the paper (measured in cache accesses/cycles here).
#define ASCIB_VICTIMIZATION_CYCLES 100000
#define ASCIB_SELECTION_CYCLES 100000
#define ASCIB_IDLE_CYCLES 400000
#define ASCIB_CANDIDATE_BITS 16

class AscibIndex : public Index {
public:
    AscibIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length,
        BLOCK **arr, uint32_t x_fill_lvl, uint32_t x_cpu,
        uint32_t x_candidate_bits=ASCIB_CANDIDATE_BITS)
        :Index(x_numset, x_numway, x_interval_length), block_array(arr),
        FILL_LEVEL(x_fill_lvl), CPU_ID(x_cpu)
    {
        IDX_SIZE = lg2(NUM_SET);
        CANDIDATE_BITS = std::min(x_candidate_bits, (uint32_t)MAX_ADDRESS_BITS);
        CANDIDATE_BITS = std::max(CANDIDATE_BITS, IDX_SIZE);
        for (uint32_t i=0; i < IDX_SIZE; ++i)
            index_bits.push_back(i);
        cout << "ASCIB index: " << NUM_SET << " sets, " << IDX_SIZE
            << " index bits, tracking " << CANDIDATE_BITS << " address bits" << endl;
    }

    ~AscibIndex() {}

    uint32_t get_set(uint64_t address) override;
    void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) override;
    void print_stats() override;

protected:
    enum Phase { BIT_VICTIMIZATION, BIT_SELECTION, IDLE };

    uint32_t compute_index(uint64_t address, const vector<uint32_t>& bits) const;
    void update_victimization(uint64_t address);
    void begin_selection();
    void update_selection(uint64_t address);
    void finish_selection(MEMORY *lower_level);
    bool install_index_bits(const vector<uint32_t>& new_bits, MEMORY *lower_level);
    uint32_t selected_index_bit(uint64_t address, uint32_t skip_position) const;
    uint32_t candidate_signature(uint64_t address) const;

    BLOCK **block_array;
    uint32_t FILL_LEVEL;
    uint32_t CPU_ID;
    uint32_t IDX_SIZE;
    uint32_t CANDIDATE_BITS;
    vector<uint32_t> index_bits;
    Phase phase{BIT_VICTIMIZATION};
    uint32_t phase_cycles{0};
    uint32_t victim_position{0};
    uint32_t selection_set_count{0};
    uint32_t candidate_count{0};
    uint64_t phase_misses{0};
    vector<uint64_t> bit_ones;
    vector<uint64_t> pair_ones;
    vector<uint32_t> selection_tags;
    vector<uint8_t> selection_valid;
    vector<uint64_t> candidate_transitions;
};

#endif // ASCIB_INDEX_H


