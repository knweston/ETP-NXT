#include "index/entropy.h"

void EntropyIndex::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) {
    // Note: to reproduce MICRO24 results, skip update pref hit (both etp access update and interval access ctr), but keep updating pref miss
    // if ((type == PREFETCH) && is_hit)
    //     return;

    uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
    uint32_t hashed_index   = get_set(address);
    N_ACCESS[unhashed_index]++;
    PI_N_ACCESS[unhashed_index]++;
    H_ACCESS[hashed_index]++;
    PI_H_ACCESS[hashed_index]++;
    if (!is_hit) {
        MISS[hashed_index]++;
        PI_MISS[hashed_index]++;
    }
    access_ctr++;

    if (!UPDATE_MISS_ONLY)
        etp_tracker_access_update(address);
    else
        if (!is_hit)
            etp_tracker_access_update(address);


    // should remap when last index config != current index config
    if (m_should_remap) {
        // if all sets have been remapped, skip this step
        if ((REMAP_INTERVAL > 0) && (access_ctr % REMAP_INTERVAL == 0) && (map_ptr < NUM_SET)) {
            move_cache_data(dram);
            if (map_ptr == NUM_SET)
                m_should_remap = false; // remapping done
        }
    }

    if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
        cout << endl << endl;
        cout << "========================================================================" << endl;
        cout << "End of interval # " << num_interval << " Summary" << endl;
        cout << "========================================================================" << endl;
        cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
        cout << "   entropy_stdev:  " << std::fixed << compute_std_dev(PI_H_ACCESS, NUM_SET) << endl;
        cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
        cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
        cout << "   map_ptr:        " << map_ptr << endl;
        cout << "========================================================================" << endl;

        // save the current XOR-hash index config
        m_last_seq_a = m_curr_seq_a;
        m_last_seq_b = m_curr_seq_b;

        // compute new XOR-hash index config for new interval
        if (RECONFIG_NO_DELAY) {
            // update trackers
            etp_tracker_interval_update();
            compute_new_config(itv_entropy, m_curr_seq_a, m_curr_seq_b);
        }
        else {
            m_curr_seq_a = m_next_seq_a;
            m_curr_seq_b = m_next_seq_b;

            // update trackers
            etp_tracker_interval_update();
            compute_new_config(itv_entropy, m_next_seq_a, m_next_seq_b);
        }

        m_should_remap = check_remap();

        cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
        cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
        cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
        cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
        if (!RECONFIG_NO_DELAY) {
            cout << "m_next_seq_a: " << vec_to_string(m_next_seq_a) << endl;
            cout << "m_next_seq_b: " << vec_to_string(m_next_seq_b) << endl;
        }
        cout << "should_remap: " << (m_should_remap ? "Yes" : "No") << endl << endl;

        memcpy(PAST_PI_EVICTION, PI_EVICTION, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_H_ACCESS, 0, NUM_SET * sizeof(*PI_H_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

        num_interval++;
        map_ptr    = 0;
        access_ctr = 0;

        // reset interval bit entropy
        for (uint32_t i=0; i < itv_entropy.size(); ++i) {
            itv_entropy[i].m_entropy = 0;
        } 
    }

    warmup_check();
}

void EntropyIndex::warmup_check() {
    if (!idx_warmup_complete) {
        if (compute_sum(N_ACCESS, NUM_SET) >= num_warmup_access) {
            cout << "total access: " << compute_sum(N_ACCESS, NUM_SET) << endl;
            finish_warmup();
            memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
            memset(H_ACCESS, 0, NUM_SET * sizeof(*H_ACCESS));
            memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
            memset(PI_H_ACCESS, 0, NUM_SET * sizeof(*PI_H_ACCESS));
            memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
            memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
            memset(MISS, 0, NUM_SET * sizeof(*MISS));
            memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
            access_ctr = 0;
            idx_warmup_complete = true;   
        }
    }
}

uint32_t EntropyIndex::get_set(uint64_t address) {
    uint32_t curr_index = compute_index(address, m_curr_seq_a, m_curr_seq_b);
    uint32_t last_index = compute_index(address, m_last_seq_a, m_last_seq_b);

    // if last index >= map_ptr, this set has not yet been moved
    if (last_index >= map_ptr)
        return last_index;
    else
        return curr_index;
}

void EntropyIndex::move_cache_data(MEMORY *lower_level) {
    for (uint32_t way=0; way < NUM_WAY; ++way) {
        BLOCK *src_block = &block_array[map_ptr][way];

        // initiate a move if the source block is valid
        if (src_block->valid) {

            // compute new index
            uint32_t dest_index = compute_index(src_block->address, m_curr_seq_a, m_curr_seq_b);
            uint32_t last_index = compute_index(src_block->address, m_last_seq_a, m_last_seq_b);

            // if new index == map_ptr => this src_block is already in the new position => skip
            if (dest_index == map_ptr)
                continue;

            // sanity check
            uint32_t curr_index = get_set(src_block->address);
            if (curr_index != map_ptr) {
                cout << "ERROR: curr_index != map_ptr" << endl;
                cout << "address:  " << src_block->address << endl;
                cout << "cpu:      " << CPU_ID << endl;
                cout << "curr_idx: " << curr_index << endl;
                cout << "map_ptr:  " << map_ptr << endl;
                cout << "dest_idx: " << dest_index << endl;
                cout << "last_idx: " << last_index << endl;
                assert(false);
            }

            bool fill_ready = true;
            // uint32_t dest_way = get_lru_victim(dest_index); // if the cache is using LRU replacement policy
            uint32_t dest_way = get_rrip_victim(dest_index); // if the cache is using RRIP-based replacement policy
            // uint32_t dest_way = get_rlr_victim(dest_index);

            BLOCK *dest_block = &block_array[dest_index][dest_way];

            /*
                tie-breaking rules: 
                    1. dest_block->dirty && !src_block->dirty   => dest_block > src_block
                    2. dest_block->dirty && src_block->dirty    => dest_block < src_block
                    3. !dest_block->dirty && src_block->dirty   => dest_block < src_block
                    4. !dest_block->dirty && !src_block->dirty  => dest_block < src_block

                additional rules:
                    1. dest_block->pf_useless
            */ 
            // if the destination block is dirty, write it back before overriding the data
            if (dest_block->valid && dest_block->dirty) {
                if (!src_block->dirty) {
                    // if the destination block is dirty but the source block is clean, drop the source block (do not fill)
                    num_drop++;
                    fill_ready = false;
                }
                else { // if both blocks are dirty, write back the destination block then move the source block to the destination
                    bool writeback_success = force_writeback(dest_block, lower_level, FILL_LEVEL, CPU_ID);
                    if (writeback_success) {
                        num_writeback++;    // update extra writeback statistics
                    }
                }
            }

            // override destination block with source block
            // we need not to up-invalidate the data in the destination block since the cache is non-inclusive
            if (fill_ready) {
                if (dest_block->valid)
                    increment_eviction(dest_index);

                override_block(src_block, dest_block);
                // lru_update(dest_index, dest_way, src_block->lru);
                rrip_update(dest_index, dest_way, rrpv[map_ptr][way]);
                // rlr_update(dest_index, dest_way, map_ptr, way);
            }

            // A dropped source line leaves its current physical set.
            if (!fill_ready)
                increment_eviction(map_ptr);

            // invalidate the source block
            src_block->valid = 0;

            // update extra read statistics
            num_read++;
        }
    }
    map_ptr++;
}

uint32_t EntropyIndex::compute_index(uint64_t address, vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b) {
    uint32_t hashed_index = 0;
    bool     is_nohash    = is_zero_segment(x_seq_b);

    if (!is_nohash) {
        for (uint32_t i=0; i < x_seq_a.size(); ++i) {
            uint32_t selected_bit_a = (address >> x_seq_a[i]) & 1;
            uint32_t selected_bit_b = (address >> x_seq_b[i]) & 1;
            hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
        }   
    }
    else {
        for (uint32_t i=0; i < x_seq_a.size(); ++i)
            hashed_index = (hashed_index << 1) | ((address >> x_seq_a[i]) & 1);
    }
    return hashed_index;
}

void EntropyIndex::print_stats() {
    cout << "remapping_read:      " << num_read << endl;
    cout << "remapping_writeback: " << num_writeback << endl;
    cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(H_ACCESS, NUM_SET) / compute_mean(H_ACCESS, NUM_SET) << endl;
    cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
    cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
    cout << "PER-SET EVICTION STATS:   " << arr_to_string(EVICTION, NUM_SET, ", ") << endl;
    cout << "PER-SET ACCESS STATS:     " << arr_to_string(H_ACCESS, NUM_SET, ", ") << endl;
}

string EntropyIndex::etp_vector_to_string(vector<BitEntropy>& entropy) {
    string str = "";
    for (uint32_t i=0; i < entropy.size(); ++i) 
        str += entropy[i].to_string() + ", ";

    str.pop_back();
    str.pop_back();
    return str;
}

uint32_t EntropyIndex::get_rrip_victim(uint32_t set) {
    // Start from RRPV = 3
    uint32_t lookupRRPV=3;

    // victimize invalid lines if any
    for (int i=0; i<LLC_WAY; i++) {
        if (!block_array[set][i].valid)
            return i;
    }

    // look for the victim in the valid lines
    while (1) {
        // look for maxRRPV + RU==0
        for (int i=0; i<LLC_WAY; i++) {
            if ((rrpv[set][i] == lookupRRPV))
                return i;
        }

        // look for maxRRPV
        for (int i=0; i<LLC_WAY; i++) {
            if (rrpv[set][i] == lookupRRPV)
                return i;
        }

        // if none, increment the RRPV and repeat the loop
        for (int i=0; i<LLC_WAY; i++)
            rrpv[set][i]++;
    }

    // WE SHOULD NOT REACH HERE
    assert(0);
    return 0;
}

void EntropyIndex::rrip_update(uint32_t set, uint32_t way, uint32_t src_rrpv) {
    // rrpv[set][way] = src_rrpv;   // etp3r_new
    // rrpv[set][way] = 0; // etp3r_rmp0
    // rrpv[set][way] = min(src_rrpv, (uint32_t)1); // etp3r_rmpmax1

    // etp3r
    if (rrpv[set][way] > src_rrpv)
        rrpv[set][way] = src_rrpv;
}

uint32_t EntropyIndex::get_lru_victim(uint32_t set) {
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

void EntropyIndex::lru_update(uint32_t set, uint32_t way, uint32_t src_lru_pos) {
    if (block_array[set][way].lru <= src_lru_pos)
        return;

    else {
        for (uint32_t i=0; i<NUM_WAY; i++) {
            if ((block_array[set][i].lru >= src_lru_pos) && (block_array[set][i].lru < block_array[set][way].lru)) {
                block_array[set][i].lru++;
            }
        }
        // insert to the LRU position that the block is currently at
        block_array[set][way].lru = src_lru_pos;
    }
}

bool EntropyIndex::check_remap() {
    return ((m_last_seq_a != m_curr_seq_a) || (m_last_seq_b != m_curr_seq_b));
}

void EntropyIndex::compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    // size of the bit sequence being tracked for entropy must greater than the size of the index sequences
    // assert((entropy.size() > x_seq_a.size()) && (entropy.size() > x_seq_b.size()));

    // sort the index bits based on their entropy, from lowest to highest
    sort(entropy.begin(), entropy.end());
    cout << "Sorted ITV:       " << etp_vector_to_string(entropy) << endl;

    vector<BitEntropy> tmp_ovr = ovr_entropy;
    sort(tmp_ovr.begin(), tmp_ovr.end());
    cout << "Sorted OVR:       " << etp_vector_to_string(tmp_ovr) << endl;

    // A = unhashed_idx, B = maxETP
    if (FIX_SEQUENCE == 1) {
        uint32_t seq_b_idx = 0;
        for (uint32_t i=0; i < entropy.size(); ++i) {

            // get highest entropy bits as sequence B
            if (entropy[entropy.size()-1-i].m_id >= IDX_SIZE) {
                // x_seq_b[seq_b_idx] = entropy[entropy.size()-1-i].m_id;  // highest unhash_idx[i] -> lowest bitmask[i]
                x_seq_b[x_seq_b.size()-1-seq_b_idx] = entropy[entropy.size()-1-i].m_id;  // highest unhash_idx[i] -> highest bitmask[i]
                seq_b_idx++;
            }

            if (seq_b_idx == x_seq_a.size())
                break;
        }
    }

    // A = maxETP, B = zeros
    else if (FIX_SEQUENCE == 2) {
        for (uint32_t i=0; i < x_seq_a.size(); ++i) {
            x_seq_a[i] = entropy[entropy.size()-1-i].m_id;
        }
        sort(x_seq_a.begin(), x_seq_a.end(), greater<uint32_t>());
    }

    else {
        for (uint32_t i=0; i < x_seq_a.size(); ++i) {
            x_seq_a[i] = entropy[entropy.size()-1-i].m_id;

            if (FIX_SEQUENCE == 0)
                x_seq_b[i] = entropy[i].m_id;
            else if (FIX_SEQUENCE == 3)
                x_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
        }

        // set highest entropy bits as sequence A, set lowest entropy bits as B
        if (FIX_SEQUENCE == 0) {
            for (uint32_t i=0; i < x_seq_a.size(); ++i) {
                x_seq_a[i] = entropy[entropy.size()-1-i].m_id;
                x_seq_b[i] = entropy[i].m_id;
            }
        }

        // set highest entropy bits as sequence A, set next highest entropy bits as B
        else if (FIX_SEQUENCE == 3) {
            for (uint32_t i=0; i < x_seq_a.size(); ++i) {
                x_seq_a[i] = entropy[entropy.size()-1-i].m_id;
                x_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
            }
            // sort(x_seq_b.begin(), x_seq_b.end(), greater<uint32_t>());  // reverse B, so the maxE bit in A is XORed with the minE bit in B 
        }

        else if (FIX_SEQUENCE == 4) {
            // get the highest entropy 2*IDX_SIZE bits
            vector<uint32_t> selected_bits;
            for (uint32_t i=0; i < x_seq_a.size()*2; ++i) {
                selected_bits.push_back(entropy[entropy.size()-1-i].m_id);
            }

            // distribute to 2 sequences A,B
            sort(selected_bits.begin(), selected_bits.end());
            cout << "Sorted selected bits: " << vec_to_string(selected_bits) << endl;
            for (uint32_t i=0; i < x_seq_a.size(); ++i) {
                x_seq_a[i] = selected_bits[i];
                // x_seq_b[i] = selected_bits[selected_bits.size()-1-i];
                x_seq_b[i] = selected_bits[i+IDX_SIZE];
            }
        }
    }

    bool revert_to_nohash = false;
    if (NOHASH_IF_SEQ_IDX_HIGHEST_ETP) {
        vector<uint32_t> nohash_idx_segment;
        vector<uint32_t> maxetp_bits;
        vector<uint32_t> zeros_segment;
        for (uint32_t i=0; i < IDX_SIZE; ++i) {
            maxetp_bits.push_back(entropy[entropy.size()-1-i].m_id);
        }

        for (int32_t i=IDX_SIZE-1; i >= 0; --i) {
            nohash_idx_segment.push_back(i);
            zeros_segment.push_back(0);
        }
        if (is_same_sequence(maxetp_bits, nohash_idx_segment)) {
            x_seq_b = zeros_segment;
            revert_to_nohash = true;
            cout << "===> Unhashed index sequence achieves highest entropy. No hashing needed." << endl;
        }
    }

    if (is_same_sequence(x_seq_a, m_last_seq_a) && is_same_sequence(x_seq_b, m_last_seq_b) && RECONFIG_NO_DELAY && SKIP_SAME_BIT_CANDIDATE) {
        // if these sequences are the same, reverse the change
        x_seq_a = m_last_seq_a;
        x_seq_b = m_last_seq_b;
        cout << "===> Next sequences are the same as last" << endl;
    }
    else if (is_same_sequence(x_seq_a, m_curr_seq_a) && is_same_sequence(x_seq_b, m_curr_seq_b) && !RECONFIG_NO_DELAY && SKIP_SAME_BIT_CANDIDATE) {
        // if these sequences are the same, reverse the change
        x_seq_a = m_curr_seq_a;
        x_seq_b = m_curr_seq_b;
        cout << "===> Next sequences are the same as current" << endl;
    }
    else {
        if (num_interval < WARMUP_INTERVALS) {}
        // exceed the threshold + not decided to revert to nohash + not previously nohash (is_zero_segment(m_curr_seq_b))
        else if (!RECONFIG_NO_DELAY && !revert_to_nohash && (SENSE_THRESHOLD > 0) && (!is_zero_segment(m_curr_seq_b) || (FIX_SEQUENCE == 2))) {
            // compute next_etp
            double next_etp = 0;
            for (uint32_t i=0; i < x_seq_a.size(); ++i) {
                next_etp += itv_entropy[x_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < x_seq_b.size(); ++i) {
                next_etp += itv_entropy[x_seq_b[i]].m_entropy;
            }

            // compute curr_etp
            double curr_etp = 0;
            for (uint32_t i=0; i < m_curr_seq_a.size(); ++i) {
                curr_etp += itv_entropy[m_curr_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < m_curr_seq_b.size(); ++i) {
                curr_etp += itv_entropy[m_curr_seq_b[i]].m_entropy;
            }

            // compute etp difference
            double etp_diff = (next_etp-curr_etp)*100/abs(curr_etp);

            // compare with threshold
            cout << "seq_A current:   " << vec_to_string(m_curr_seq_a) << endl;
            cout << "seq_B current:   " << vec_to_string(m_curr_seq_b) << endl;
            cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
            cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
            cout << std::setprecision(0) << "Current Cfg ETP:  " << curr_etp << endl;
            cout << std::setprecision(0) << "Next Cfg ETP:     " << next_etp << endl;
            if (etp_diff >= SENSE_THRESHOLD) {
                cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% >= " << SENSE_THRESHOLD << "%. Switch to new candidate." << endl;
            }
            else {
                cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% < " << SENSE_THRESHOLD << "%. Keep current candidate." << endl;
                x_seq_a = m_curr_seq_a;
                x_seq_b = m_curr_seq_b;
            }
            cout << std::setprecision(5);
        }
        else if (RECONFIG_NO_DELAY && !revert_to_nohash && (SENSE_THRESHOLD > 0) && (!is_zero_segment(m_last_seq_b) || (FIX_SEQUENCE == 2))) {
            // compute next_etp
            double next_etp = 0;
            for (uint32_t i=0; i < x_seq_a.size(); ++i) {
                next_etp += itv_entropy[x_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < x_seq_b.size(); ++i) {
                next_etp += itv_entropy[x_seq_b[i]].m_entropy;
            }

            // compute last_etp
            double last_etp = 0;
            for (uint32_t i=0; i < m_last_seq_a.size(); ++i) {
                last_etp += itv_entropy[m_last_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < m_last_seq_b.size(); ++i) {
                last_etp += itv_entropy[m_last_seq_b[i]].m_entropy;
            }

            // compute etp difference
            double etp_diff = (next_etp-last_etp)*100/abs(last_etp);

            // compare with threshold
            cout << "seq_A last:   " << vec_to_string(m_last_seq_a) << endl;
            cout << "seq_B last:   " << vec_to_string(m_last_seq_b) << endl;
            cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
            cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
            cout << std::setprecision(0) << "Last Cfg ETP: " << last_etp << endl;
            cout << std::setprecision(0) << "Next Cfg ETP: " << next_etp << endl;
            if (etp_diff >= SENSE_THRESHOLD) {
                cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% >= " << SENSE_THRESHOLD << "%. Switch to new candidate." << endl;
            }
            else {
                cout << std::setprecision(2) << "ETP difference: " << etp_diff << "% < " << SENSE_THRESHOLD << "%. Keep last candidate." << endl;
                x_seq_a = m_last_seq_a;
                x_seq_b = m_last_seq_b;
            }
            cout << std::setprecision(5);
        }
    }

    return;
}

void EntropyIndex::etp_tracker_access_update(uint64_t address) {
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if (address == m_addr_hist[idx])
            continue;

        // compare curr_addr vs. last_addr bit by bit
        for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
            if (((address >> i) & 1) ^ ((m_addr_hist[idx] >> i) & 1))    // A ^ B == 1 => A != B => high entropy => increment entropy
                itv_entropy[i].m_entropy++;
        }
    }

    bool duplicate_addr = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        if (address == m_addr_hist[idx])
            duplicate_addr = true;
    }

    // save curr_addr
    if (!duplicate_addr) {
        m_addr_hist.push_back(address);
        while (m_addr_hist.size() > HISTORY_LENGTH)
            m_addr_hist.erase(m_addr_hist.begin());
    }
}

void EntropyIndex::etp_tracker_interval_update() {
    double sum_ovr_entropy = 0;
    double avg_ovr_entropy = 0;
    double var_ovr_entropy = 0;

    double sum_itv_entropy = 0;
    double avg_itv_entropy = 0;
    double var_itv_entropy = 0;

    cout << std::fixed;
    cout << std::setprecision(5);

    cout << "ITV tracker:      " << etp_vector_to_string(itv_entropy) << endl;
    for (uint32_t i=0; i < ovr_entropy.size(); ++i)
        ovr_entropy[i].m_entropy += itv_entropy[i].m_entropy;
    // cout << "OVR tracker:      " << etp_vector_to_string(ovr_entropy) << endl;

    // compute mean entropy
    for (uint32_t i=0; i < ovr_entropy.size(); ++i) {
        sum_ovr_entropy += ovr_entropy[i].m_entropy;
        sum_itv_entropy += itv_entropy[i].m_entropy;
    }
    avg_ovr_entropy = sum_ovr_entropy/ovr_entropy.size();
    avg_itv_entropy = sum_itv_entropy/itv_entropy.size();

    // compute stdev entropy
    for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
        var_ovr_entropy += pow(ovr_entropy[i].m_entropy - avg_ovr_entropy, 2);
        var_itv_entropy += pow(itv_entropy[i].m_entropy - avg_itv_entropy, 2);
    }
    var_ovr_entropy = sqrt(var_ovr_entropy/ovr_entropy.size());
    var_itv_entropy = sqrt(var_itv_entropy/itv_entropy.size());

    // cout << "Standardized OVR: ";
    // for (uint32_t i=0; i < ovr_entropy.size(); ++i) {
    //     cout << ovr_entropy[i].get_str_id() << (ovr_entropy[i].m_entropy - avg_ovr_entropy)/var_ovr_entropy << ", ";
    // }
    // cout << endl;

    cout << "Standardized ITV: ";
    for (uint32_t i=0; i < itv_entropy.size(); ++i) {
        cout << itv_entropy[i].get_str_id() << (itv_entropy[i].m_entropy - avg_itv_entropy)/var_itv_entropy << ", ";
    }
    cout << endl;
}

/*
 * Check whether this is a new sequence
 * The sequence is new only if it contain one or more new bits.
 * The sequence is NOT new if the bits are the same, regardless of the order of the bits.
 * This help reduces the sensitivity of the index, as well as the remapping cost
 */
bool EntropyIndex::is_same_sequence(vector<uint32_t> x_seq_a, vector<uint32_t> x_seq_b) {
    sort(x_seq_a.begin(), x_seq_a.end());
    sort(x_seq_b.begin(), x_seq_b.end());
    return (x_seq_a == x_seq_b);
}

bool EntropyIndex::is_zero_segment(vector<uint32_t>& x_seq) {
    for (uint32_t i=0; i < x_seq.size(); ++i) {
        if (x_seq[i] != 0)
            return false;
    }
    return true;
}

uint32_t EntropyIndex::global_idx(uint32_t set, uint32_t way) {
    return (set*NUM_WAY + way);
}

uint32_t EntropyIndex::get_rlr_victim(uint32_t set) {
    // victimize invalid lines if any
    for (int i=0; i<LLC_WAY; i++) {
        if (!block_array[set][i].valid)
            return i;
    }

    // compute P-score of every block in victim set, victimize the lowest p-score
    uint32_t min_score  = COEF_BLK_AGE + COEF_BLK_ISHIT + COEF_BLK_ISPF;
    uint32_t victim_way = 0;
    for (uint32_t way=0; way < NUM_WAY; ++way) {
        uint32_t glb_idx = global_idx(set,way);
        bool p_age    = (blk_age[glb_idx] > glb_avg_hitage)? 0 : 1;
        bool p_reused = blk_ishit[glb_idx]? 1 : 0;
        bool p_type   = blk_ispf[glb_idx]? 0 : 1;
        uint32_t p_score = COEF_BLK_AGE * p_age + COEF_BLK_ISHIT * p_reused + COEF_BLK_ISPF * p_type;
        if (p_score < min_score) {
            min_score = p_score;
            victim_way = way;
        }
        else if (p_score == min_score) {
            // tie-breaker 1: age, lower age is victimized
            if (blk_age[glb_idx] < blk_age[global_idx(set,victim_way)]) {
                victim_way = way;
            }
            // tie-breaker 2: way_id, lower way_id is victimized
            // do nothing
        }
    }

    return victim_way;
}

void EntropyIndex::rlr_update(uint32_t set, uint32_t way, uint32_t src_set, uint32_t src_way) {
    // replace destination block info with source block info
    blk_age[global_idx(set,way)]   = blk_age[global_idx(src_set,src_way)];   // set age for new line
    blk_ishit[global_idx(set,way)] = blk_ishit[global_idx(src_set,src_way)]; // set reused
    blk_ispf[global_idx(set,way)]  = blk_ispf[global_idx(src_set,src_way)];   // set prefetch
    return;
}