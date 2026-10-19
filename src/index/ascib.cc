#include "index/ascib.h"
#include "dram_controller.h"

uint32_t AscibIndex::compute_index(uint64_t address, const vector<uint32_t>& bits) const {
    uint32_t set = 0;
    for (uint32_t i=0; i < bits.size(); ++i)
        set = (set << 1) | ((address >> bits[i]) & 1);
    return set;
}

uint32_t AscibIndex::get_set(uint64_t address) {
    return compute_index(address, index_bits);
}

uint32_t AscibIndex::selected_index_bit(uint64_t address, uint32_t skip_position) const {
    uint32_t value = 0;
    for (uint32_t i=0; i < index_bits.size(); ++i) {
        if (i != skip_position)
            value = (value << 1) | ((address >> index_bits[i]) & 1);
    }
    return value;
}

uint32_t AscibIndex::candidate_signature(uint64_t address) const {
    uint32_t signature = 0;
    for (uint32_t bit=0; bit < CANDIDATE_BITS; ++bit) {
        bool selected = false;
        for (uint32_t i=0; i < index_bits.size(); ++i)
            selected = selected || (i != victim_position && index_bits[i] == bit);
        if (!selected)
            signature = (signature << 1) | ((address >> bit) & 1);
    }
    return signature;
}

void AscibIndex::update_victimization(uint64_t address) {
    ++phase_misses;
    if (bit_ones.empty()) {
        bit_ones.assign(IDX_SIZE, 0);
        pair_ones.assign(IDX_SIZE * IDX_SIZE, 0);
    }

    for (uint32_t i=0; i < IDX_SIZE; ++i) {
        bit_ones[i] += (address >> index_bits[i]) & 1;
        for (uint32_t j=i+1; j < IDX_SIZE; ++j) {
            uint32_t pair_id = i*IDX_SIZE+j;
            pair_ones[pair_id] += ((address >> index_bits[i]) ^ (address >> index_bits[j])) & 1;
        }
    }
}

void AscibIndex::begin_selection() {
    phase = BIT_SELECTION;
    phase_cycles = 0;
    selection_set_count = 1u << (IDX_SIZE-1);
    candidate_count = CANDIDATE_BITS - (IDX_SIZE-1);
    selection_tags.assign(selection_set_count, 0);
    selection_valid.assign(selection_set_count, 0);
    candidate_transitions.assign(CANDIDATE_BITS, 0);
}

void AscibIndex::update_selection(uint64_t address) {
    uint32_t set = selected_index_bit(address, victim_position);
    uint32_t signature = candidate_signature(address);
    if (selection_valid[set] && selection_tags[set] != signature) {
        uint32_t changed = selection_tags[set] ^ signature;
        uint32_t candidate = 0;
        for (uint32_t bit=0; bit < CANDIDATE_BITS; ++bit) {
            bool selected = false;
            for (uint32_t i=0; i < index_bits.size(); ++i)
                selected = selected || (i != victim_position && index_bits[i] == bit);
            if (!selected) {
                candidate_transitions[bit] += (changed >> (candidate_count-1-candidate)) & 1;
                ++candidate;
            }
        }
    }
    selection_tags[set] = signature;
    selection_valid[set] = 1;
}

bool AscibIndex::install_index_bits(const vector<uint32_t>& new_bits, MEMORY *lower_level) {
    if (new_bits == index_bits)
        return false;

    // ASCIB's basic consistency policy flushes lines that would move to a
    // different set under the new mask. Preflight dirty writebacks by DRAM
    // channel so force_writeback cannot encounter a full queue mid-remap.
    vector<uint32_t> dirty_per_channel(DRAM_CHANNELS, 0);
    vector<uint64_t> channel_address(DRAM_CHANNELS, 0);
    if (lower_level != nullptr) {
        MEMORY_CONTROLLER *dram = dynamic_cast<MEMORY_CONTROLLER *>(lower_level);
        if (dram == nullptr)
            return false;
        for (uint32_t set=0; set < NUM_SET; ++set) {
            for (uint32_t way=0; way < NUM_WAY; ++way) {
                BLOCK *line = &block_array[set][way];
                if (line->valid && line->dirty && compute_index(line->address, new_bits) != set) {
                    uint32_t channel = dram->dram_get_channel(line->address);
                    ++dirty_per_channel[channel];
                    channel_address[channel] = line->address;
                }
            }
        }
        for (uint32_t channel=0; channel < DRAM_CHANNELS; ++channel) {
            if (dirty_per_channel[channel] == 0)
                continue;
            uint32_t occupancy = lower_level->get_occupancy(2, channel_address[channel]);
            uint32_t capacity = lower_level->get_size(2, channel_address[channel]);
            if (dirty_per_channel[channel] > capacity - occupancy)
                return false;
        }
    }
    else {
        for (uint32_t set=0; set < NUM_SET; ++set) {
            for (uint32_t way=0; way < NUM_WAY; ++way) {
                BLOCK *line = &block_array[set][way];
                if (line->valid && line->dirty && compute_index(line->address, new_bits) != set)
                    return false;
            }
        }
    }

    for (uint32_t set=0; set < NUM_SET; ++set) {
        for (uint32_t way=0; way < NUM_WAY; ++way) {
            BLOCK *line = &block_array[set][way];
            if (!line->valid || compute_index(line->address, new_bits) == set)
                continue;
            if (line->dirty) {
                force_writeback(line, lower_level, FILL_LEVEL, CPU_ID);
            }
            line->valid = 0;
            increment_eviction(set);
        }
    }
    index_bits = new_bits;
    return true;
}

void AscibIndex::finish_selection(MEMORY *lower_level) {
    uint32_t best_bit = CANDIDATE_BITS;
    uint64_t best_transitions = 0;
    for (uint32_t bit=0; bit < CANDIDATE_BITS; ++bit) {
        bool selected = false;
        for (uint32_t i=0; i < index_bits.size(); ++i)
            selected = selected || (i != victim_position && index_bits[i] == bit);
        if (!selected && candidate_transitions[bit] > best_transitions) {
            best_transitions = candidate_transitions[bit];
            best_bit = bit;
        }
    }

    if (best_bit < CANDIDATE_BITS) {
        vector<uint32_t> next_bits = index_bits;
        next_bits[victim_position] = best_bit;
        install_index_bits(next_bits, lower_level);
    }
    phase = IDLE;
    phase_cycles = 0;
    bit_ones.clear();
    pair_ones.clear();
    selection_tags.clear();
    selection_valid.clear();
    candidate_transitions.clear();
}

void AscibIndex::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) {
    Index::update(address, dram, is_hit, type, eviction_address);
    ++phase_cycles;

    if (phase == BIT_VICTIMIZATION) {
        if (!is_hit)
            update_victimization(address);
        if (phase_cycles >= ASCIB_VICTIMIZATION_CYCLES) {
            uint64_t total = phase_misses;
            uint64_t lowest_usefulness = UINT64_MAX;
            victim_position = 0;
            for (uint32_t i=0; i < IDX_SIZE; ++i) {
                uint64_t ones = bit_ones.empty() ? 0 : bit_ones[i];
                uint64_t usefulness = std::min(ones, total-std::min(total, ones));
                if (usefulness < lowest_usefulness) {
                    lowest_usefulness = usefulness;
                    victim_position = i;
                }
            }
            // A highly correlated pair makes one bit redundant. Victimize
            // the less useful member of the least-useful pair.
            for (uint32_t i=0; i < IDX_SIZE; ++i) {
                for (uint32_t j=i+1; j < IDX_SIZE; ++j) {
                    uint64_t pair = pair_ones.empty() ? 0 : pair_ones[i*IDX_SIZE+j];
                    uint64_t usefulness = std::min(pair, total-std::min(total, pair));
                    if (usefulness < lowest_usefulness) {
                        uint64_t ones_i = bit_ones.empty() ? 0 : bit_ones[i];
                        uint64_t ones_j = bit_ones.empty() ? 0 : bit_ones[j];
                        victim_position = (std::min(ones_i, total-std::min(total, ones_i)) <=
                            std::min(ones_j, total-std::min(total, ones_j))) ? i : j;
                        lowest_usefulness = usefulness;
                    }
                }
            }
            begin_selection();
        }
    }
    else if (phase == BIT_SELECTION) {
        update_selection(address);
        if (phase_cycles >= ASCIB_SELECTION_CYCLES)
            finish_selection(dram);
    }
    else if (phase_cycles >= ASCIB_IDLE_CYCLES) {
        phase = BIT_VICTIMIZATION;
        phase_cycles = 0;
        phase_misses = 0;
        bit_ones.clear();
        pair_ones.clear();
    }
}

void AscibIndex::print_stats() {
    cout << "ASCIB index bits: ";
    for (uint32_t i=0; i < index_bits.size(); ++i)
        cout << index_bits[i] << (i+1 == index_bits.size() ? "" : ",");
    cout << endl;
    cout << "Ovr. Access Stdev/Mean: "
         << compute_std_dev(N_ACCESS, NUM_SET) / compute_mean(N_ACCESS, NUM_SET) << endl;
    cout << "Ovr. Evictions: " << compute_sum(EVICTION, NUM_SET) << endl;
}


