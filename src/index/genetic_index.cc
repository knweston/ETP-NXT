#include "index/genetic_index.h"

uint32_t GeneticIndex::get_set(uint64_t address) {
    uint32_t curr_index = compute_index(address, current_candidate_idx);
    uint32_t last_index = compute_index(address, last_candidate_idx);

    // if last index >= map_ptr, this set has not yet been moved
    if (last_index >= map_ptr)
        return last_index;
    else
        return curr_index;
}

uint32_t GeneticIndex::compute_index(uint64_t address, uint32_t candidate_idx) {
    if (candidate_idx == m_population.size())
        return address & ((1 << lg2(NUM_SET)) - 1);
    else {
        vector<Gene> candidate_genes = m_population[candidate_idx].genes();
        uint32_t hashed_index = 0;
        for (uint32_t i=0; i < candidate_genes.size(); ++i) {
            uint32_t selected_bit_a = (address >> candidate_genes[i].m_a) & 1;
            uint32_t selected_bit_b = (address >> candidate_genes[i].m_b) & 1;
            hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
        }
        return hashed_index;
    }
}

void GeneticIndex::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
    uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
    uint32_t hashed_index   = get_set(address);
    N_ACCESS[unhashed_index]++;
    PI_N_ACCESS[unhashed_index]++;
    G_ACCESS[hashed_index]++;
    PI_G_ACCESS[hashed_index]++;
    if (!is_hit) {
        MISS[hashed_index]++;
        PI_MISS[hashed_index]++;
    }
    access_ctr++;

    // should remap when last candidate != current candidate
    if (last_candidate_idx != current_candidate_idx) {
        // if all sets have been remapped, skip this step
        if ((REMAP_INTERVAL > 0) && (access_ctr % REMAP_INTERVAL == 0) && (map_ptr < NUM_SET)) 
            move_cache_data(dram);
    }

    // finish one interval
    if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
        // save current config
        last_candidate_idx = current_candidate_idx;

        // if not warmup
        if (num_interval >= m_num_warmup_interval) {
            // if evaluation is in process
            if (!m_evaluation_done) {
                // record miss count of current interval
                m_scoreboard[current_candidate_idx] = compute_sum(PI_MISS, NUM_SET);
                cout << "candidate:  " << m_population[current_candidate_idx].to_string() << endl;
                cout << "miss count: " << m_scoreboard[current_candidate_idx] << endl;

                // if this is the last candidate, finalize the evaluation
                if (current_candidate_idx == (m_population.size()-1)) {
                    m_evaluation_done = true;

                    // find the best candidate
                    uint32_t min_misscount = m_scoreboard[0];
                    current_candidate_idx = 0;
                    for (uint32_t cid=1; cid < m_scoreboard.size(); ++cid) {
                        if (m_scoreboard[cid] < min_misscount) {
                            min_misscount = m_scoreboard[cid];
                            current_candidate_idx = cid;
                        }
                    }
                    cout << "optimal candidate: " << m_population[current_candidate_idx].to_string() << endl;
                }
                else {
                    current_candidate_idx++;
                }
            }
        }
        else if (num_interval == (m_num_warmup_interval-1)) {
            current_candidate_idx = 0;
        }

        cout << endl << endl;
        cout << "========================================================================" << endl;
        cout << "End of interval # " << num_interval << " Summary" << endl;
        cout << "========================================================================" << endl;
        cout << "   map_ptr:    " << map_ptr << endl;
        cout << "   accesses:   " << access_ctr << endl;
        cout << "   curr_cand_idx:  " << current_candidate_idx << endl;
        cout << "   last_cand_idx:  " << last_candidate_idx << endl;
        cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
        cout << "   genetic_stdev:  " << std::fixed << compute_std_dev(PI_G_ACCESS, NUM_SET) << endl;
        cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
        cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
        cout << "========================================================================" << endl;
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_G_ACCESS, 0, NUM_SET * sizeof(*PI_G_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
        num_interval++;
        map_ptr    = 0;
        access_ctr = 0;

        if (m_evaluation_done && !idx_warmup_complete && (compute_sum(N_ACCESS, NUM_SET) >= num_warmup_access)) {
            cout << "total access: " << compute_sum(N_ACCESS, NUM_SET) << endl;
            finish_warmup();
            memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
            memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
            memset(MISS, 0, NUM_SET * sizeof(*MISS));
            memset(G_ACCESS, 0, NUM_SET * sizeof(*G_ACCESS));
            access_ctr = 0;
            idx_warmup_complete = true;   
        }
    }
}

void GeneticIndex::print_stats() {
    cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(G_ACCESS, NUM_SET) / compute_mean(G_ACCESS, NUM_SET) << endl;
    cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
    cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
    cout << "PER-SET EVICTION STATS:" << endl;
    for (uint32_t i=0; i < NUM_SET; ++i)
        cout << EVICTION[i] << endl;

    cout << "PER-SET ACCESS STATS:" << endl;
    for (uint32_t i=0; i < NUM_SET; ++i)
        cout << G_ACCESS[i] << endl;
}

vector<uint32_t> GeneticIndex::read_dna_config(string app_name) {
    ifstream conf_file(CONFIG_FILE);
    string line;
    if (conf_file.is_open()) {
        while(getline(conf_file, line)) {
            if (line.find(app_name+" = ") != string::npos) {
                return parse_config(line);
            }
        }
    }
}

vector<uint32_t> GeneticIndex::parse_config(string input_line) {
    // input format: [app_name] = [dna_sequence in space-separated-numbers format]
    vector<uint32_t> read_sequence;
    uint32_t element=0;
    uint64_t split_index=0;
    for (; split_index < input_line.size(); ++split_index) {
        if (input_line[split_index] == '=') {
            split_index++;
            break;
        }
    }

    string str_sequence = input_line.substr(split_index, input_line.size());
    stringstream ss(str_sequence);
    while (ss >> element)
        read_sequence.push_back(element);
    
    return read_sequence;
}

void GeneticIndex::move_cache_data(MEMORY *lower_level) {
    for (uint32_t way=0; way < NUM_WAY; ++way) {
        BLOCK *block = &block_array[map_ptr][way];

        // initiate a move if the source block is valid
        if (block->valid) {
            
            // compute new index
            uint32_t dest_index = compute_index(block->address, current_candidate_idx);
            uint32_t last_index = compute_index(block->address, last_candidate_idx);

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
                cout << "dest_idx: " << dest_index << endl;
                cout << "last_idx: " << last_index << endl;
                assert(false);
            }

            bool fill_ready = true;
            // uint32_t dest_way = get_drrip_victim(dest_index);
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
                // drrip_update(dest_index, dest_way, rrpv[map_ptr][way]);
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

//========================================================================//
// REPLACEMENT FUNCTIONS
//========================================================================//
uint32_t GeneticIndex::get_lru_victim(uint32_t set) {
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

void GeneticIndex::lru_update(uint32_t set, uint32_t way) {
    // update lru replacement state
    for (uint32_t i=0; i<NUM_WAY; i++) {
        if (block_array[set][i].lru < block_array[set][way].lru) {
            block_array[set][i].lru++;
        }
    }
    block_array[set][way].lru = 0; // promote to the MRU position
}

uint32_t GeneticIndex::get_drrip_victim(uint32_t set) {
    // Assume maxRRPV = 3 for now
    uint32_t maxRRPV=3;

    // victimize invalid lines if any
    for (int i=0; i<LLC_WAY; i++) {
        if (!block_array[set][i].valid)
            return i;
    }

    // look for the victim in the valid lines
    while (1)
    {
        for (int i=0; i<LLC_WAY; i++)
            if (rrpv[set][i] == maxRRPV) {
                if (block_array[set][i].valid) {
                    increment_eviction(set);
                }
                return i;
            }

        for (int i=0; i<LLC_WAY; i++)
            rrpv[set][i]++;
    }

    // WE SHOULD NOT REACH HERE
    assert(0);
    return 0;
}

void GeneticIndex::drrip_update(uint32_t set, uint32_t way, uint32_t src_age) {
    if (rrpv[set][way] > src_age)
        rrpv[set][way] = src_age;
}