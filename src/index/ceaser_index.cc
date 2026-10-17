#include "index/ceaser_index.h"

//========================================================================//
uint32_t CeaserIndex::get_set(uint64_t address) {
    uint32_t curr_index = compute_index(address, curr_key);
    uint32_t last_index = compute_index(address, last_key);

    // if last index >= map_ptr, this set has not yet been moved
    if (last_index >= map_ptr)
        return last_index;
    else
        return curr_index;
}

//========================================================================//
void CeaserIndex::update(uint64_t address, MEMORY *lower_level, bool is_hit, uint32_t type) {
    uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
    uint32_t hashed_index   = get_set(address);
    N_ACCESS[unhashed_index]++;
    C_ACCESS[hashed_index]++;
    PI_N_ACCESS[unhashed_index]++;
    PI_C_ACCESS[hashed_index]++;
    if (!is_hit) {
        MISS[unhashed_index]++;
        PI_MISS[unhashed_index]++;
    }
    access_ctr++;

    if ((REMAP_INTERVAL > 0) && (access_ctr % REMAP_INTERVAL == 0) && (map_ptr < NUM_SET)) // if all sets have been remapped, skip this step
        move_cache_data(lower_level);

    if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
        cout << endl << endl;
        cout << "========================================================================" << endl;
        cout << "End of interval # " << num_interval << " Summary" << endl;
        cout << "========================================================================" << endl;
        cout << "   map_ptr:    " << map_ptr << endl;
        cout << "   accesses:   " << access_ctr << endl;
        cout << "   last_key:   " << last_key << endl;
        cout << "   curr_key:   " << curr_key << endl;
        cout << "   next_key:   " << next_key << endl;

        last_key = curr_key;
        curr_key = next_key;
        next_key = key_gen();

        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_C_ACCESS, 0, NUM_SET * sizeof(*PI_C_ACCESS));
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
            memset(C_ACCESS, 0, NUM_SET * sizeof(*C_ACCESS));
            memset(PI_C_ACCESS, 0, NUM_SET * sizeof(*PI_C_ACCESS));
            access_ctr = 0;
            idx_warmup_complete = true;   
        }
    }
}

//========================================================================//
void CeaserIndex::print_stats() {
    cout << "========================================================================" << endl;
    cout << "CEASER INDEX" << endl;
    cout << "Extra WB:   " << num_writeback << endl;
    cout << "Extra READ: " << num_read << endl;
    cout << "Total DROP: " << num_drop << endl;
    cout << "========================================================================" << endl;
    cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(C_ACCESS, NUM_SET) / compute_mean(C_ACCESS, NUM_SET) << endl;
    cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
    cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
    cout << "PER-SET EVICTION STATS: " << arr_to_string(EVICTION, NUM_SET, ", ") << endl;
    cout << "PER-SET ACCESS STATS:   " << arr_to_string(C_ACCESS, NUM_SET, ", ") << endl;
}

//========================================================================//
uint64_t CeaserIndex::encrypt_addr(uint64_t address, uint64_t full_key) {
    uint32_t xor_input = address & ADDR_PART_MASK;                                  // rhs
    uint32_t f_input   = (address >> ENCRYPTING_ADDR_PART_SIZE) & ADDR_PART_MASK;   // lhs

    for (uint32_t i=0; i < NUM_STAGES; ++i) {
        uint32_t partial_key = (full_key >> (i*ENCRYPTING_ADDR_PART_SIZE)) & ADDR_PART_MASK;

        uint32_t f_output    = F(f_input, partial_key);
        uint32_t xor_output  = xor_input ^ f_output;

        xor_input = f_input;
        f_input   = xor_output;
    }

    // xor_input = L', f_input = R'
    return ((xor_input << ENCRYPTING_ADDR_PART_SIZE) | f_input); 
}

//========================================================================//
uint32_t CeaserIndex::F(uint64_t input, uint32_t partial_key) {
    uint32_t output = p_box(s_box(input, partial_key));
    return output;
}

//========================================================================//
uint32_t CeaserIndex::s_box(uint64_t input, uint32_t partial_key) {
    // XOR each input bit with the corresponding bit of the key following S_BOX_XORING shuffled order.
    // input[0] ^ partial_key[s_box_xoring[0]] (bit0 = LSB)
    uint32_t output = 0;
    for (uint32_t i=0; i < ENCRYPTING_ADDR_PART_SIZE; ++i) {
        output = (output << 1) | (((input >> i) & 1) ^ (partial_key >> (S_BOX_XORING[i]) & 1));
    }
    return output;
}

//========================================================================//
uint32_t CeaserIndex::p_box(uint64_t input) {
    uint32_t output = 0;
    for (uint32_t i=0; i < ENCRYPTING_ADDR_PART_SIZE; ++i) {
        output = (output << 1) | ((input >> P_BOX_WIRING[i]) & 1); // bit0 = LSB
    }
    return output;
}

//========================================================================//
uint64_t CeaserIndex::key_gen() {
    uint64_t new_key = 0;
    for (uint32_t i=0; i < NUM_STAGES; ++i) {
        new_key = (new_key << ENCRYPTING_ADDR_PART_SIZE) | (rand() & ADDR_PART_MASK);
    }
    return new_key;
}

//========================================================================//
void CeaserIndex::move_cache_data(MEMORY *lower_level) {
    for (uint32_t way=0; way < NUM_WAY; ++way) {
        BLOCK *block = &block_array[map_ptr][way];

        // initiate a move if the source block is valid
        if (block->valid) {

            // compute new index
            uint32_t dest_index = compute_index(block->address, curr_key);
            uint32_t last_index = compute_index(block->address, last_key);

            // if new index == map_ptr => this block is already in the new position => skip
            if (dest_index == map_ptr)
                continue;

            // sanity check
            uint32_t curr_index = get_set(block->address);

            if (curr_index != map_ptr) {
                cout << "ERROR: curr_index != map_ptr" << endl;
                cout << "address:  " << block->address << endl;
                cout << "cpu:      " << CPU_ID << endl;
                cout << "curr_idx: " << curr_index << endl;
                cout << "map_ptr:  " << map_ptr << endl;
                cout << "curr_key: " << curr_key << endl;
                cout << "last_key: " << last_key << endl;
                cout << "dest_idx: " << dest_index << endl;
                cout << "last_idx: " << last_index << endl;
                assert(false);
            }

            bool fill_ready = true;
            uint32_t dest_way = get_lru_victim(dest_index);
            // if the destination block is dirty, write it back before overriding the data
            if (block_array[dest_index][dest_way].valid && block_array[dest_index][dest_way].dirty) {
                if (!block->dirty) {
                    // if the destination block is dirty but the source block is clean, drop the source block (do not fill)
                    num_drop++;
                    fill_ready = false;
                }
                else { // if both blocks are dirty, write back the destination block then move the source block to the destination
                    // if the lower level memory is full
                    if (lower_level->get_occupancy(2, block_array[dest_index][dest_way].address) == lower_level->get_size(2, block_array[dest_index][dest_way].address)) {
                        // lower level WQ is full, cannot replace this victim
                        lower_level->increment_WQ_FULL(block_array[dest_index][dest_way].address);
                        fill_ready = false;
                        assert(false);
                    }
                    else {
                        PACKET writeback_packet;
                        writeback_packet.fill_level = FILL_LEVEL << 1;
                        writeback_packet.cpu = CPU_ID;
                        writeback_packet.address = block_array[dest_index][dest_way].address;
                        writeback_packet.full_addr = block_array[dest_index][dest_way].full_addr;
                        writeback_packet.data = block_array[dest_index][dest_way].data;
                        writeback_packet.instr_id = block->instr_id;
                        writeback_packet.ip = 0; // writeback does not have ip
                        writeback_packet.type = WRITEBACK;
                        writeback_packet.event_cycle = current_core_cycle[CPU_ID];
                        lower_level->add_wq(&writeback_packet);

                        // update extra writeback statistics
                        num_writeback++;
                    }
                }
            }

            // override destination block with source block
            // we need not to up-invalidate the data in the destination block since the cache is non-inclusive
            if (fill_ready) {
                override_block(block, &block_array[dest_index][dest_way]);
                lru_update(dest_index, dest_way);
            }

            // invalidate the source block
            block->valid   = 0;
            block->address = 0;

            // update extra read statistics
            num_read++;
        }
    }
    map_ptr++;
}


uint32_t CeaserIndex::compute_index(uint64_t address, uint64_t full_key) {
    uint64_t encrypted_addr = encrypt_addr(address, full_key);
    return ((uint32_t) (encrypted_addr & ((1 << lg2(NUM_SET)) - 1)));
}

//========================================================================//
// REPLACEMENT FUNCTIONS
//========================================================================//
uint32_t CeaserIndex::get_lru_victim(uint32_t set) {
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

void CeaserIndex::lru_update(uint32_t set, uint32_t way) {
    // update lru replacement state
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block_array[set][i].lru < block_array[set][way].lru) {
            block_array[set][i].lru++;
        }
    }
    block_array[set][way].lru = 0; // promote to the MRU position
}