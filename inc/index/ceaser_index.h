#ifndef CEASER_INDEX_H
#define CEASER_INDEX_H

#include "index/index.h"

#define NUM_STAGES 4
#define ENCRYPTING_ADDR_PART_SIZE 13
#define ADDR_PART_MASK ((1 << ENCRYPTING_ADDR_PART_SIZE) - 1)


class CeaserIndex : public Index {
public:
    CeaserIndex(uint32_t x_numset, uint32_t x_numway, 
        uint32_t x_interval_length, uint32_t remap_length, 
        uint32_t x_fill_lvl, BLOCK **arr, uint32_t cpu)
        :Index(x_numset, x_numway, x_interval_length)
    {
        REMAP_INTERVAL = remap_length;
        FILL_LEVEL     = x_fill_lvl;
        block_array    = arr;
        CPU_ID         = cpu;

        // init keys
        curr_key = key_gen();
        next_key = key_gen();
        last_key = curr_key;


        cout << endl;
        cout << "========================================================================" << endl;
        cout << "CEASER INDEX CONFIGURATION" << endl;
        cout << "========================================================================" << endl;
        cout << "Remap interval:       " << REMAP_INTERVAL << endl;
        cout << "Interval size:        " << INTERVAL_SIZE << endl;
        cout << "Num sets:             " << NUM_SET << endl;
        cout << "Num ways:             " << NUM_WAY << endl;
        cout << "Num stages:           " << NUM_STAGES << endl;
        cout << "Encr. addr part size: " << ENCRYPTING_ADDR_PART_SIZE << endl;
        cout << "Addr part mask:       " << ADDR_PART_MASK << endl;
        cout << "========================================================================" << endl;
        cout << endl;
    }
    ~CeaserIndex();

    uint32_t get_set(uint64_t address);
    void     update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address);
    void     print_stats();

    uint32_t compute_index(uint64_t address, uint64_t full_key);
    void     move_cache_data(MEMORY *lower_level);   // move data from old set to new set

    // Feistel Encryption
    uint64_t encrypt_addr(uint64_t address, uint64_t full_key);
    uint32_t F(uint64_t input, uint32_t partial_key);
    uint32_t s_box(uint64_t input, uint32_t partial_key);
    uint32_t p_box(uint64_t input);
    uint64_t key_gen();

    // replacement policy
    uint32_t get_lru_victim(uint32_t set);
    void     lru_update(uint32_t set, uint32_t way);

protected:

    // runtime stats
    uint32_t map_ptr       = 0;
    uint64_t num_interval  = 0;
    uint64_t num_writeback = 0;
    uint64_t num_drop      = 0;
    uint64_t num_read      = 0;

    // preset configurations
    uint32_t REMAP_INTERVAL;
    uint32_t FILL_LEVEL;
    uint32_t CPU_ID;

    // 52-bit keys
    uint64_t curr_key;
    uint64_t next_key;
    uint64_t last_key;

    // substitution-permutation network (SPN) config
    uint32_t P_BOX_WIRING[ENCRYPTING_ADDR_PART_SIZE] = {1, 12, 3, 9, 4, 5, 7, 6, 11, 2, 10, 8, 0};
    uint32_t S_BOX_XORING[ENCRYPTING_ADDR_PART_SIZE] = {12, 3, 5, 9, 4, 2, 11, 7, 10, 1, 8, 6, 0};

    BLOCK**  block_array; // ptr to the cache set data array

    /*
        4-stage Feistel-Network, each F function use a 13-bit key => total 52 bits
    */
};

#endif  // CEASER_INDEX_H