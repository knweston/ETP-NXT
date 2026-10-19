#include "index/entropy2.h"

void EntropyV2::move_cache_data(MEMORY *lower_level) {
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
            uint32_t dest_way = get_lru_victim(dest_index);
            // uint32_t dest_way = get_rrip_victim(dest_index); // if the cache is using RRIP-based replacement policy
            BLOCK *dest_block = &block_array[dest_index][dest_way];

            bool fill_ready = remap_tie_break(src_block, dest_block, lower_level);
            // overwrite destination block with source block
            // we need not to up-invalidate the data in the destination block since the cache is non-inclusive
            if (fill_ready) {
                if (dest_block->valid)
                    increment_eviction(dest_index);

                override_block(src_block, dest_block);
                lru_update(dest_index, dest_way, src_block->lru);
                // rrip_update(dest_index, dest_way, rrpv[map_ptr][way]);
            }

            // invalidate the source block
            src_block->valid = 0;
            // update extra read statistics
            num_read++;
        }
    }
    map_ptr++;
}

bool EntropyV2::remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level) {
    if (!dest->valid)
        return true;
    bool overwrite_dest = true;

    // prefetch -> dirty
    if (!dest->prefetch) {
        // dest NOT prefetch but the src is, drop the src, regardless of src dirty state
        if (src->prefetch) {
            num_drop++;
            overwrite_dest = false;
            if (src->dirty) {
                bool writeback_success = force_writeback(src, lower_level, FILL_LEVEL, CPU_ID);
                if (writeback_success) {
                    num_writeback++;
                }
            }
        }
    }
    else if (dest->dirty) {
        // dest is prefetch dirty
        if (!src->dirty) {
            // if the destination block is dirty but the source block is clean, drop the source block (do not fill)
            num_drop++;
            overwrite_dest = false;
        }
        else { // if both blocks are dirty, write back the destination block then move the source block to the destination
            bool writeback_success = force_writeback(dest, lower_level, FILL_LEVEL, CPU_ID);
            if (writeback_success) {
                num_writeback++;    // update extra writeback statistics
            }
        }
    }
    else {
        // dest prefetch clean -> just overwrite, no further action
    }
    return overwrite_dest;
}

// return the correlation counter between bit A and bit B in the correlation table
uint32_t EntropyV2::get_correl(uint32_t bit_a, uint32_t bit_b) {
    uint32_t index = get_correl_table_index(bit_a, bit_b);
    return correl_table[index].m_entropy;
}

uint32_t EntropyV2::get_correl_table_index(uint32_t bit_a, uint32_t bit_b) {
    if (bit_a > bit_b) {
        uint32_t index = bit_a*(bit_a-1);
        index = index/2 + bit_b;
        return index;
    }
    else {
        uint32_t index = bit_b*(bit_b-1);
        index = index/2 + bit_a;
        return index;
    }
}

// find the lowest correlated bit in the input sequence for the input bit
uint32_t EntropyV2::find_min_correl_bit_from_sequence(uint32_t bit_a, vector<uint32_t>& sequence) {
    uint32_t min_correl = get_correl(bit_a, sequence[0]);
    uint32_t min_correl_idx = 0;
    for (uint32_t i=1; i < sequence.size(); ++i) {
        uint32_t bitpair_correl = get_correl(bit_a, sequence[i]);
        if (bitpair_correl < min_correl) {
            min_correl = bitpair_correl;
            min_correl_idx = i;
        }
    }
    return min_correl_idx;
}

uint32_t EntropyV2::find_min_total_correl_bit_in_sequence(vector<uint32_t>& sequence) {
    assert(sequence.size() > 0);

    uint32_t min_idx = UINT32_MAX;
    uint32_t min_corr_val = UINT32_MAX;

    for (uint32_t i=0; i < sequence.size(); ++i) {
        uint32_t curr_bit = sequence[i];
        vector<uint32_t> tmp_seq = sequence;
        tmp_seq.erase(tmp_seq.begin()+i);
        uint32_t curr_corr_val = compute_sum_correl_bit_vs_sequence(curr_bit, tmp_seq);
        // cout << "curr_bit: " << curr_bit << ". sum_corr: " << curr_corr_val << endl;
        if (curr_corr_val < min_corr_val) {
            min_corr_val = curr_corr_val;
            min_idx = i;
        }
    }
    return min_idx;
}

uint32_t EntropyV2::compute_sum_correl_two_sequences(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    uint32_t sum_correl = 0;
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        sum_correl += get_correl(x_seq_a[i], x_seq_b[i]);
    }
    return sum_correl;
}

uint32_t EntropyV2::compute_sum_correl_bit_vs_sequence(uint32_t bit_a, vector<uint32_t>& sequence) {
    uint32_t sum_correl = 0;
    for (uint32_t i=0; i < sequence.size(); ++i) {
        sum_correl += get_correl(bit_a, sequence[i]);
    }
    return sum_correl;
}

void EntropyV2::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) {
    uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
    uint32_t hashed_index   = get_set(address);
    N_ACCESS[unhashed_index]++;
    PI_N_ACCESS[unhashed_index]++;
    E_ACCESS[hashed_index]++;
    PI_E_ACCESS[hashed_index]++;
    if (!is_hit) {
        MISS[unhashed_index]++;
        PI_MISS[unhashed_index]++;
    }
    access_ctr++;

    if (should_update(is_hit, type))
        etp_tracker_access_update(address);
    // etp_tracker_access_update(is_hit, type, address);


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
        cout << "   entropy_stdev:  " << std::fixed << compute_std_dev(PI_E_ACCESS, NUM_SET) << endl;
        cout << "   eviction_count: " << std::fixed << compute_sum(PI_EVICTION, NUM_SET) << "(" 
             << compute_sum(PI_EVICTION, NUM_SET)*100/INTERVAL_SIZE 
             << "%)" << endl;
        cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
        cout << "   num_reset_itv:         " << num_reset_itv << endl;
        cout << "   num_reset_cor:         " << num_reset_cor << endl;
        cout << "   num_reset_bitpair_etp: " << num_reset_bitpair_etp << endl;
        cout << "   num_reset_bitpair_cor: " << num_reset_bitpair_correl << endl;
        cout << "   num_reset_bitpair_ptn: " << num_reset_bitpair_ptn << endl;
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
        memset(PI_E_ACCESS, 0, NUM_SET * sizeof(*PI_E_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

        num_interval++;
        map_ptr    = 0;
        access_ctr = 0;

        // for (uint32_t i=0; i < itv_entropy.size(); ++i) {
        //    itv_entropy[i].m_entropy = 0;
        // }
        // for (uint32_t i=0; i < correl_table.size(); ++i) {
        //    correl_table[i].m_entropy = 0;
        // }
        // for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
        //    bitpair_entropy[i].m_entropy = 0;
        // }
        // for (uint32_t i=0; i < bitpair_correl_table.size(); ++i) {
        //    bitpair_correl_table[i].m_entropy = 0;
        // }
    }

    warmup_check();
}

bool EntropyV2::should_update(bool is_hit, uint32_t type) {
    if ((UPDATE_HITMISS_SETTING == 1) && is_hit)
        return false;
    if ((UPDATE_HITMISS_SETTING == 2) && !is_hit)
        return false;
    if (SKIP_UPDATE_PF && (type == PREFETCH))
        return false;
    if (SKIP_UPDATE_WB && (type == WRITEBACK))
        return false;
    if (SKIP_UPDATE_WB_HIT && (type == WRITEBACK) && is_hit)
        return false;
    if (SKIP_UPDATE_DURING_REMAP && m_should_remap)
        return false;
    return true;
}

void EntropyV2::etp_tracker_access_update(uint64_t address) {
    /*
    * update correlation trackers
    * rules:
    * - if bit_i and bit_j move in the same fashion (both flip, both not flip): correl_i_j = 1
    * - else: correl_i_j = 0
    */ 

    // update correlation counters
    bool should_reset_correl = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if ((m_addr_hist[idx] == 0) || (address == m_addr_hist[idx]))
            continue;

        uint64_t bit_flips = address ^ m_addr_hist[idx];
        uint32_t correl_table_idx = 0;
        for (uint32_t i=1; i < MAX_ADDRESS_BITS; ++i) {
            for (uint32_t j=0; j < i; ++j) {
                uint32_t bit_i_j_flip_correl = ((bit_flips >> i) & 1) ^ ((bit_flips >> j) & 1);
                if (bit_i_j_flip_correl == 0) {
                    correl_table[correl_table_idx].m_entropy++;
                    if (correl_table[correl_table_idx].m_entropy >= CORREL_CTR_MAX) {
                        should_reset_correl = true;
                    }
                }
                correl_table_idx++;
            }
        }
    }

    // update entropy trackers
    bool should_reset_entropy = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if (address == m_addr_hist[idx])
            continue;

        // compare curr_addr vs. last_addr bit by bit
        for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
            if (((address >> i) & 1) ^ ((m_addr_hist[idx] >> i) & 1)) {    // A ^ B == 1 => A != B => high entropy => increment entropy
                itv_entropy[i].m_entropy++;
                if (itv_entropy[i].m_entropy >= ENTROPY_CTR_MAX) {
                    should_reset_entropy = true;
                }
            }
        }
    }

    // update bit-pair entropy trackers
    bool should_reset_bitpair_entropy = false;
    bool should_reset_bitpair_pattern = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if (address == m_addr_hist[idx])
            continue;

        for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
            uint32_t curr_addr_bitpair_xor_output = ((address >> bitpair_entropy[i].m_segid[0]) & 1) ^ ((address >> bitpair_entropy[i].m_segid[1]) & 1);
            uint32_t last_addr_bitpair_xor_output = ((m_addr_hist[idx] >> bitpair_entropy[i].m_segid[0]) & 1) ^ ((m_addr_hist[idx] >> bitpair_entropy[i].m_segid[1]) & 1);
            bool bitpair_flip = (bool) (curr_addr_bitpair_xor_output ^ last_addr_bitpair_xor_output);
            if (bitpair_flip) {
                bitpair_entropy[i].m_entropy++;
                if (bitpair_entropy[i].m_entropy >= ENTROPY_CTR_MAX) {
                    should_reset_bitpair_entropy = true;
                }
            }
            bitpair_pattern_hist[i].m_entropy += (bitpair_flip ^ last_access_flip_behavior[i]);
            last_access_flip_behavior[i] = bitpair_flip;
            if (bitpair_pattern_hist[i].m_entropy >= ENTROPY_CTR_MAX) {
                should_reset_bitpair_pattern = true;
            }
        }
    }

    // update bit-pair correlation counters
    bool should_reset_bitpair_correl = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if ((m_addr_hist[idx] == 0) || (address == m_addr_hist[idx]))
            continue;

        uint32_t correl_table_idx = 0;
        for (uint32_t i=1; i < bitpair_entropy.size(); ++i) {
            uint32_t curr_addr_bitpair_i_xor_output = ((address >> bitpair_entropy[i].m_segid[0]) & 1) ^ ((address >> bitpair_entropy[i].m_segid[1]) & 1);
            uint32_t last_addr_bitpair_i_xor_output = ((m_addr_hist[idx] >> bitpair_entropy[i].m_segid[0]) & 1) ^ ((m_addr_hist[idx] >> bitpair_entropy[i].m_segid[1]) & 1);
            bool bitpair_i_flip = (bool) (curr_addr_bitpair_i_xor_output ^ last_addr_bitpair_i_xor_output);

            for (uint32_t j=0; j < i; ++j) {
                uint32_t curr_addr_bitpair_j_xor_output = ((address >> bitpair_entropy[j].m_segid[0]) & 1) ^ ((address >> bitpair_entropy[j].m_segid[1]) & 1);
                uint32_t last_addr_bitpair_j_xor_output = ((m_addr_hist[idx] >> bitpair_entropy[j].m_segid[0]) & 1) ^ ((m_addr_hist[idx] >> bitpair_entropy[j].m_segid[1]) & 1);
                bool bitpair_j_flip = (bool) (curr_addr_bitpair_j_xor_output ^ last_addr_bitpair_j_xor_output);

                if (bitpair_i_flip == bitpair_j_flip) {
                    bitpair_correl_table[correl_table_idx].m_entropy++;
                    if (bitpair_correl_table[correl_table_idx].m_entropy >= CORREL_CTR_MAX) {
                        should_reset_bitpair_correl = true;
                    }
                }
                correl_table_idx++;
            }
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

    if (should_reset_entropy) {
        num_reset_itv++;
        for (uint32_t i=0; i < itv_entropy.size(); ++i) {
            itv_entropy[i].m_entropy /= 4;
        }
    }

    if (should_reset_correl) {
        num_reset_cor++;
        for (uint32_t i=0; i < correl_table.size(); ++i) {
            correl_table[i].m_entropy /= 4;
        }
    }

    if (should_reset_bitpair_entropy) {
        num_reset_bitpair_etp++;
        for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
            bitpair_entropy[i].m_entropy /= 4;
        }
    }

    if (should_reset_bitpair_correl) {
        num_reset_bitpair_correl++;
        for (uint32_t i=0; i < bitpair_correl_table.size(); ++i) {
            bitpair_correl_table[i].m_entropy /= 4;
        }
    }

    if (should_reset_bitpair_pattern) {
        num_reset_bitpair_ptn++;
        for (uint32_t i=0; i < bitpair_pattern_hist.size(); ++i) {
            bitpair_pattern_hist[i].m_entropy /= 2;
        }
    }
}

void EntropyV2::compute_new_config(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    // sort the index bits based on their entropy, from lowest to highest
    sort(entropy.begin(), entropy.end());
    cout << "Sorted ITV:   " << etp_vector_to_string(entropy) << endl;

    vector<BitEntropy> tmp_ovr = ovr_entropy;
    sort(tmp_ovr.begin(), tmp_ovr.end());
    cout << "Sorted OVR:   " << etp_vector_to_string(tmp_ovr) << endl;

    vector<BitEntropy> tmp_bitpair_etp = bitpair_entropy;
    sort(tmp_bitpair_etp.begin(), tmp_bitpair_etp.end());
    std::reverse(tmp_bitpair_etp.begin(), tmp_bitpair_etp.end());
    cout << "Sorted BPE:   " << etp_vector_to_string(tmp_bitpair_etp) << endl;

    vector<BitEntropy> tmp_bitpair_pattern = bitpair_pattern_hist;
    sort(tmp_bitpair_pattern.begin(), tmp_bitpair_pattern.end());
    std::reverse(tmp_bitpair_pattern.begin(), tmp_bitpair_pattern.end());
    cout << "Sorted BPP:   " << etp_vector_to_string(tmp_bitpair_pattern) << endl;
    cout << endl;

    // ETP-C1
    vector<uint32_t> c1_seq_a = x_seq_a;
    vector<uint32_t> c1_seq_b = x_seq_b;
    get_cfg_1(entropy, x_seq_a, x_seq_b, c1_seq_a, c1_seq_b);
    cout << "c1_seq_a:  " << vec_to_string(c1_seq_a) << endl;
    cout << "c1_seq_b:  " << vec_to_string(c1_seq_b) << endl;
    uint32_t c1_corr = compute_sum_correl_two_sequences(c1_seq_a, c1_seq_b);
    cout << "CORR c1:   " << c1_corr << endl << endl;



    // ETP2-C2 (ETP2r7)
    vector<uint32_t> c2_seq_a(x_seq_a.size(), 0);
    vector<uint32_t> c2_seq_b(x_seq_b.size(), 0);
    get_cfg_2(entropy, x_seq_a, x_seq_b, c2_seq_a, c2_seq_b);
    cout << "c2_seq_a:  " << vec_to_string(c2_seq_a) << endl;
    cout << "c2_seq_b:  " << vec_to_string(c2_seq_b) << endl;
    uint32_t c2_corr = compute_sum_correl_two_sequences(c2_seq_a, c2_seq_b);
    cout << "CORR c2:   " << c2_corr << endl << endl;



    // ETP-C3 (Entropy-V1)
    vector<uint32_t> c3_seq_a = x_seq_a;
    vector<uint32_t> c3_seq_b = x_seq_b;
    get_cfg_3(entropy, x_seq_a, x_seq_b, c3_seq_a, c3_seq_b);
    cout << "c3_seq_a:  " << vec_to_string(c3_seq_a) << endl;
    cout << "c3_seq_b:  " << vec_to_string(c3_seq_b) << endl;
    uint32_t c3_corr = compute_sum_correl_two_sequences(c3_seq_a, c3_seq_b);
    cout << "CORR c3:   " << c3_corr << endl << endl;



    // ETP2c4 candidate
    vector<uint32_t> c4_seq_a(x_seq_a.size(), 0);
    vector<uint32_t> c4_seq_b(x_seq_b.size(), 0);

    // ETP2c4 candidate: gather the highest entropy bits
    vector<uint32_t> max_etp_bits;
    for (uint32_t i=entropy.size()-x_seq_a.size()*2; i < entropy.size(); ++i) {
        max_etp_bits.push_back(entropy[i].m_id);
    }
    cout << "max_etp_bits: " << vec_to_string(max_etp_bits) << endl;

    for (uint32_t idx=0; idx < c4_seq_a.size(); ++idx) {
        // get the highest entropy bit and and its lowest correlated pair.
        c4_seq_a[idx] = max_etp_bits[0];             // save it in sequence A
        max_etp_bits.erase(max_etp_bits.begin());   // erase it in max_etp_bits

        // find its corresponding lowest correlation bit and save in B
        uint32_t lowest_corr_idx = find_min_correl_bit_from_sequence(c4_seq_a[idx], max_etp_bits);
        c4_seq_b[idx] = max_etp_bits[lowest_corr_idx];
        max_etp_bits.erase(max_etp_bits.begin()+lowest_corr_idx);   // erase it in max_etp_bits
    }
    cout << "c4_seq_a:  " << vec_to_string(c4_seq_a) << endl;
    cout << "c4_seq_b:  " << vec_to_string(c4_seq_b) << endl;
    uint32_t c4_corr = compute_sum_correl_two_sequences(c4_seq_a, c4_seq_b);
    cout << "CORR c4:   " << c4_corr << endl << endl;



    // ETP2c5 candidate
    vector<uint32_t> c5_seq_a(x_seq_a.size(), 0);
    vector<uint32_t> c5_seq_b(x_seq_b.size(), 0);

    // ETP2c5 candidate: gather the highest entropy bits
    max_etp_bits.resize(0);
    for (uint32_t i=0; i < entropy.size(); ++i) {
        max_etp_bits.push_back(entropy[entropy.size()-1-i].m_id);
    }
    cout << "max_etp_bits: " << vec_to_string(max_etp_bits) << endl;

    for (uint32_t idx=0; idx < c5_seq_a.size(); ++idx) {
        // get the highest entropy bit and and its lowest correlated pair.
        c5_seq_a[idx] = max_etp_bits[0];             // save it in sequence A
        max_etp_bits.erase(max_etp_bits.begin());   // erase it in max_etp_bits

        // find its corresponding lowest correlation bit and save in B
        uint32_t lowest_corr_idx = find_min_correl_bit_from_sequence(c5_seq_a[idx], max_etp_bits);
        c5_seq_b[idx] = max_etp_bits[lowest_corr_idx];
        max_etp_bits.erase(max_etp_bits.begin()+lowest_corr_idx);   // erase it in max_etp_bits
    }
    cout << "c5_seq_a:  " << vec_to_string(c5_seq_a) << endl;
    cout << "c5_seq_b:  " << vec_to_string(c5_seq_b) << endl;
    uint32_t c5_corr = compute_sum_correl_two_sequences(c5_seq_a, c5_seq_b);
    cout << "CORR c5:   " << c5_corr << endl << endl;



    // etpc6 candidate: always select bit0 -> bit10 (nohash index) + 11 highest entropy bits
    vector<uint32_t> c6_seq_a(x_seq_a.size(), 0);
    vector<uint32_t> c6_seq_b(x_seq_b.size(), 0);
    max_etp_bits.resize(0);
    for (uint32_t i=0; i < entropy.size(); ++i) {
        max_etp_bits.push_back(entropy[entropy.size()-1-i].m_id);
    }
    uint32_t idx = max_etp_bits.size()-1;
    while (max_etp_bits.size() > x_seq_a.size()*2) {
        if (max_etp_bits[idx] > IDX_SIZE)
            max_etp_bits.erase(max_etp_bits.begin()+idx);
        --idx;
    }
    // sort(max_etp_bits.begin(), max_etp_bits.end());
    cout << "max_etp_bits: " << vec_to_string(max_etp_bits) << endl;

    // for (uint32_t idx=0; idx < c6_seq_a.size(); ++idx) {
    //    // get the highest entropy bit and and its lowest correlated pair.
    //    c6_seq_a[idx] = max_etp_bits[0];             // save it in sequence A
    //    max_etp_bits.erase(max_etp_bits.begin());   // erase it in max_etp_bits

    //    // find its corresponding lowest correlation bit and save in B
    //    uint32_t lowest_corr_idx = find_min_correl_bit_from_sequence(c6_seq_a[idx], max_etp_bits);
    //    c6_seq_b[idx] = max_etp_bits[lowest_corr_idx];
    //    max_etp_bits.erase(max_etp_bits.begin()+lowest_corr_idx);   // erase it in max_etp_bits
    // }

    for (uint32_t idx=0; idx < c6_seq_a.size(); ++idx) {
        c6_seq_a[idx] = max_etp_bits[idx];
        c6_seq_b[idx] = max_etp_bits[idx+c6_seq_a.size()];
    }
    // sort(c6_seq_a.begin(), c6_seq_a.end(), std::greater<uint32_t>());
    std::reverse(c6_seq_a.begin(), c6_seq_a.end());

    // etpc6 candidate: re-order sequence B based on the bitwise correlation counters
    vector<uint32_t> reordered_indices = {};
    for (uint32_t i=0; i < c6_seq_a.size(); ++i) {
        vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
        for (uint32_t j=0; j < c6_seq_b.size(); ++j) {
            seq_a_bit_correl.push_back(get_correl(c6_seq_a[i], c6_seq_b[j]));
        }
        uint32_t min_correl_idx = find_min_index(seq_a_bit_correl, reordered_indices);
        reordered_indices.push_back(i);

        // pair this bit in A with the least correlated bit in B
        if (min_correl_idx != i)
            swap(c6_seq_b[i], c6_seq_b[min_correl_idx]);
    }
    cout << "c6_seq_a:  " << vec_to_string(c6_seq_a) << endl;
    cout << "c6_seq_b:  " << vec_to_string(c6_seq_b) << endl;
    uint32_t c6_corr = compute_sum_correl_two_sequences(c6_seq_a, c6_seq_b);
    cout << "CORR c6:   " << c6_corr << endl << endl;

    vector<uint32_t> c9_seq_a;
    vector<uint32_t> c9_seq_b;
    vector<uint32_t> used_bits;
    vector<uint32_t> used_pair_ids;
    get_cfg_9b(entropy, x_seq_a, x_seq_b, c9_seq_a, c9_seq_b, used_bits, used_pair_ids);

    // hard-coded config
    x_seq_a = c9_seq_a;
    x_seq_b = c9_seq_b;
    cout << "Choose C9" << endl << endl;

    cout << "c9_seq_a:  " << vec_to_string(c9_seq_a) << endl;
    cout << "c9_seq_b:  " << vec_to_string(c9_seq_b) << endl;
    uint32_t c9_corr = compute_sum_correl_two_sequences(c9_seq_a, c9_seq_b);
    cout << "CORR c9:   " << c9_corr << endl << endl;

    double bp_etp_last = 0;
    double bp_cor_last = 0;
    double bp_etp_curr = 0;
    double bp_cor_curr = 0;
    for (uint32_t i=0; i < last_interval_bpe_ids.size(); ++i) {
        bp_etp_last += bitpair_entropy[last_interval_bpe_ids[i]].m_entropy;
    }
    for (uint32_t i=0; i < used_pair_ids.size(); ++i) {
        bp_etp_curr += bitpair_entropy[used_pair_ids[i]].m_entropy;
    }
    for (uint32_t i=1; i < last_interval_bpe_ids.size(); ++i) {
        for (uint32_t j=0; j < i; j++) {
            uint32_t bpc_index = get_correl_table_index(last_interval_bpe_ids[i], last_interval_bpe_ids[j]);
            bp_cor_last += bitpair_correl_table[bpc_index].m_entropy;
        }
    }
    for (uint32_t i=1; i < used_pair_ids.size(); ++i) {
        for (uint32_t j=0; j < i; j++) {
            uint32_t bpc_index = get_correl_table_index(used_pair_ids[i], used_pair_ids[j]);
            bp_cor_curr += bitpair_correl_table[bpc_index].m_entropy;
        }
    }

    double bp_pattern_last = 0;
    double bp_pattern_curr = 0;
    for (uint32_t i=0; i < last_interval_bpe_ids.size(); ++i) {
        bp_pattern_last += bitpair_pattern_hist[last_interval_bpe_ids[i]].m_entropy;
    }
    for (uint32_t i=0; i < used_pair_ids.size(); ++i) {
        bp_pattern_curr += bitpair_pattern_hist[used_pair_ids[i]].m_entropy;
    }
    double bp_pattern_diff = (num_interval>0)? (bp_pattern_curr-bp_pattern_last)*100/bp_pattern_last : 0;

    last_interval_bpe_ids = used_pair_ids;

    double bp_cor_diff = (num_interval>0)? (bp_cor_last-bp_cor_curr)*100/bp_cor_last : 0;
    double bp_etp_diff = (num_interval>0)? (bp_etp_curr-bp_etp_last)*100/bp_etp_last : 0;

    cout << "BPCOR Last Cfg:    " << std::setprecision(0) << bp_cor_last << endl;
    cout << "BPCOR Next Cfg:    " << std::setprecision(0) << bp_cor_curr << endl;
    cout << "BPCOR reduction:   " << std::setprecision(2) << bp_cor_diff << "%" << std::setprecision(5) << endl << endl;

    cout << "BPETP Last Cfg:    " << std::setprecision(0) << bp_etp_last << endl;
    cout << "BPETP Next Cfg:    " << std::setprecision(0) << bp_etp_curr << endl;
    cout << "BPETP improvement: " << std::setprecision(2) << bp_etp_diff << "%" << std::setprecision(5) << endl << endl;

    cout << "BPPAT Last Cfg:    " << std::setprecision(0) << bp_pattern_last << endl;
    cout << "BPPAT Next Cfg:    " << std::setprecision(0) << bp_pattern_curr << endl;
    cout << "BPPAT improvement:   " << std::setprecision(2) << bp_pattern_diff << "%" << std::setprecision(5) << endl << endl;


    // compute next_etp
    double etp_diff = 0;
    double next_etp = 0;
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        next_etp += itv_entropy[x_seq_a[i]].m_entropy;
    }

    for (uint32_t i=0; i < x_seq_b.size(); ++i) {
        next_etp += itv_entropy[x_seq_b[i]].m_entropy;
    }

    // compute last_etp
    double last_etp = 0;
    if (RECONFIG_NO_DELAY) {
        if (num_interval>0) {
            for (uint32_t i=0; i < m_last_seq_a.size(); ++i) {
                last_etp += itv_entropy[m_last_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < m_last_seq_b.size(); ++i) {
                last_etp += itv_entropy[m_last_seq_b[i]].m_entropy;
            }
            etp_diff = (next_etp-last_etp)*100/abs(last_etp);  // compute etp difference
        }
        cout << "seq_A last:      " << vec_to_string(m_last_seq_a) << endl;
        cout << "seq_B last:      " << vec_to_string(m_last_seq_b) << endl;
        cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
        cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
        cout << std::setprecision(0) << "ETP Last Cfg: " << last_etp << std::setprecision(5) << endl;
        cout << std::setprecision(0) << "ETP Next Cfg: " << next_etp << std::setprecision(5) << endl;
        cout << std::setprecision(2) << "ETP diff:     " << etp_diff << "%" << std::setprecision(5) << endl;
    }

    // compute curr_etp
    double curr_etp = 0;
    if (!RECONFIG_NO_DELAY) {
        if (num_interval>0) {
            for (uint32_t i=0; i < m_curr_seq_a.size(); ++i) {
                curr_etp += itv_entropy[m_curr_seq_a[i]].m_entropy;
            }

            for (uint32_t i=0; i < m_curr_seq_b.size(); ++i) {
                curr_etp += itv_entropy[m_curr_seq_b[i]].m_entropy;
            }
            etp_diff = (next_etp-curr_etp)*100/abs(curr_etp);  // compute etp difference
        }
        cout << "seq_A current:   " << vec_to_string(m_curr_seq_a) << endl;
        cout << "seq_B current:   " << vec_to_string(m_curr_seq_b) << endl;
        cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
        cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl;
        cout << std::setprecision(0) << "ETP Curr Cfg: " << curr_etp << std::setprecision(5) << endl;
        cout << std::setprecision(0) << "ETP Next Cfg: " << next_etp << std::setprecision(5) << endl;
        cout << std::setprecision(2) << "ETP diff:     " << etp_diff << std::setprecision(5) << endl;
    }

    double next_corr = (double)compute_sum_correl_two_sequences(x_seq_a, x_seq_b);
    double last_corr = (num_interval>0)? (double)compute_sum_correl_two_sequences(m_last_seq_a, m_last_seq_b) : 0;
    double corr_diff = (num_interval>0)? (last_corr-next_corr)*100/last_corr : 0;

    cout << "CORR Last Cfg: " << std::setprecision(0) << last_corr << endl;
    cout << "CORR Next Cfg: " << std::setprecision(0) << next_corr << endl;
    cout << "CORR diff:     " << std::setprecision(2) << corr_diff << "%" << std::setprecision(5) << endl << endl;

    cout << endl;
    if (num_interval < WARMUP_INTERVALS) {

    }

    // exceed the threshold + not decided to revert to nohash + not previously nohash (is_zero_segment(m_curr_seq_b))
    else if (!RECONFIG_NO_DELAY && (SENSE_THRESHOLD >= 0) && !is_zero_segment(m_curr_seq_b)) {      
        // compare with threshold
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

    else if (RECONFIG_NO_DELAY && (SENSE_THRESHOLD >= 0) && !is_zero_segment(m_last_seq_b)) {
        if (bp_pattern_diff > SENSE_THRESHOLD) {
            cout << "Switch to new candidate.\n";
        }
        else {
            cout << "Keep last candidate.\n";
            x_seq_a = m_last_seq_a;
            x_seq_b = m_last_seq_b;
        }
        cout << std::setprecision(5);
    }

    return;
}

void EntropyV2::get_cfg_1(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c1_seq_a, vector<uint32_t>& c1_seq_b) 
{
    // ETP2 candidate: gather the highest and next highest entropy bits to form sequence A and B
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        c1_seq_a[i] = entropy[entropy.size()-1-i].m_id;
        c1_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
    }

    // ETP2 candidate: re-order sequence B based on the bitwise correlation counters
    vector<uint32_t> reordered_indices = {};
    for (uint32_t i=0; i < c1_seq_a.size(); ++i) {
        vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
        for (uint32_t j=0; j < c1_seq_b.size(); ++j) {
            seq_a_bit_correl.push_back(get_correl(c1_seq_a[i], c1_seq_b[j]));
        }
        uint32_t min_correl_idx = find_min_index(seq_a_bit_correl, reordered_indices);
        reordered_indices.push_back(i);

        // pair this bit in A with the least correlated bit in B
        if (min_correl_idx != i)
            swap(c1_seq_b[i], c1_seq_b[min_correl_idx]);
    }
    return;
}

void EntropyV2::get_cfg_2(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c2_seq_a, vector<uint32_t>& c2_seq_b) 
{
    // gather the highest entropy bits
    vector<uint32_t> max_etp_bits;
    for (uint32_t i=0; i < x_seq_a.size()*2; ++i) {
        max_etp_bits.push_back(entropy[entropy.size()-1-i].m_id);
    }
    cout << "max_etp_bits: " << vec_to_string(max_etp_bits) << endl;

    for (uint32_t idx=0; idx < c2_seq_a.size(); ++idx) {
        // get the highest entropy bit and and its lowest correlated pair.
        c2_seq_a[idx] = max_etp_bits[0];             // save it in sequence A
        max_etp_bits.erase(max_etp_bits.begin());    // erase it in max_etp_bits

        // find its corresponding lowest correlation bit and save in B
        uint32_t lowest_corr_idx = find_min_correl_bit_from_sequence(c2_seq_a[idx], max_etp_bits);
        c2_seq_b[idx] = max_etp_bits[lowest_corr_idx];
        max_etp_bits.erase(max_etp_bits.begin()+lowest_corr_idx);   // erase it in max_etp_bits
    }
    return;
}

void EntropyV2::get_cfg_3(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c3_seq_a, vector<uint32_t>& c3_seq_b)
{
    // ETP candidate: gather the highest and next highest entropy bits to form sequence A and B
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        c3_seq_a[i] = entropy[entropy.size()-1-i].m_id;
        c3_seq_b[i] = entropy[entropy.size()-1-IDX_SIZE-i].m_id;
    }
}


void EntropyV2::get_cfg_9b(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c9_seq_a, vector<uint32_t>& c9_seq_b,
    vector<uint32_t>& used_bits, vector<uint32_t>& used_pair_ids) 
{
    vector<BitEntropy> tmp_bitpair_pattern = bitpair_pattern_hist;
    sort(tmp_bitpair_pattern.begin(), tmp_bitpair_pattern.end());
    std::reverse(tmp_bitpair_pattern.begin(), tmp_bitpair_pattern.end());
    double TOLERANCE_RATE = 0.98;

    // pattern-based
    c9_seq_a.push_back(tmp_bitpair_pattern[0].m_segid[0]);
    c9_seq_b.push_back(tmp_bitpair_pattern[0].m_segid[1]);
    used_bits.push_back(tmp_bitpair_pattern[0].m_segid[0]);
    used_bits.push_back(tmp_bitpair_pattern[0].m_segid[1]);
    used_pair_ids.push_back(get_correl_table_index(tmp_bitpair_pattern[0].m_segid[0],tmp_bitpair_pattern[0].m_segid[1]));
    cout << "used_bits: " << vec_to_string(used_bits) << endl;
    cout << "used_pair_ids: " << vec_to_string(used_pair_ids) << endl;
    cout << "c9_seq_a:  " << vec_to_string(c9_seq_a) << endl;
    cout << "c9_seq_b:  " << vec_to_string(c9_seq_b) << endl;
    cout << endl;

    for (uint32_t n=c9_seq_a.size(); n < x_seq_a.size(); ++n) {
        vector<BitEntropy> bpp_candidate_list;
        int32_t min_tolerated_bpp = -1;
        for (uint32_t i=1; i < tmp_bitpair_pattern.size(); ++i) {
            if (!is_existed(tmp_bitpair_pattern[i].m_segid[0], used_bits) && !is_existed(tmp_bitpair_pattern[i].m_segid[1], used_bits)) {
                if (min_tolerated_bpp < 0) {
                    min_tolerated_bpp = tmp_bitpair_pattern[i].m_entropy*TOLERANCE_RATE;
                }
                if (tmp_bitpair_pattern[i].m_entropy < min_tolerated_bpp) {
                    break;
                }
                uint32_t sum_corr = 0;
                BitEntropy bpp_candidate;
                uint32_t bpp_index = get_correl_table_index(tmp_bitpair_pattern[i].m_segid[0],tmp_bitpair_pattern[i].m_segid[1]);
                for (uint32_t j=0; j < used_pair_ids.size(); ++j) {
                    uint32_t bpc_index = get_correl_table_index(bpp_index, used_pair_ids[j]);
                    sum_corr += bitpair_correl_table[bpc_index].m_entropy;
                }
                bpp_candidate.m_id = i; // i-th element in tmp_bitpair_pattern
                bpp_candidate.m_entropy = sum_corr;
                bpp_candidate_list.push_back(bpp_candidate);
            }
        }
        sort(bpp_candidate_list.begin(), bpp_candidate_list.end());
        c9_seq_a.push_back(tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[0]);
        c9_seq_b.push_back(tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[1]);
        used_bits.push_back(tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[0]);
        used_bits.push_back(tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[1]);
        used_pair_ids.push_back(get_correl_table_index(tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[0],tmp_bitpair_pattern[bpp_candidate_list[0].m_id].m_segid[1]));
        cout << "min_tolerated_bpp: " << min_tolerated_bpp << endl;
        cout << "candidates:        " << etp_vector_to_string(bpp_candidate_list) << endl;
        cout << "used_bits:         " << vec_to_string(used_bits) << endl;
        cout << "used_pair_ids:     " << vec_to_string(used_pair_ids) << endl;
        cout << "c9_seq_a:          " << vec_to_string(c9_seq_a) << endl;
        cout << "c9_seq_b:          " << vec_to_string(c9_seq_b) << endl;
        cout << endl;
    }
    return;
}

/*
void EntropyV2::get_cfg_9a(vector<BitEntropy> entropy, vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
      // [c9] lowest correlation bit-pair
      vector<uint32_t> c9_seq_a;
      vector<uint32_t> c9_seq_b;
      vector<uint32_t> used_bits;
      vector<uint32_t> used_pair_ids;
      double TOLERANCE_RATE = 0.98;

      vector<BitEntropy> tmp_corr = bitpair_correl_table;
      sort(tmp_corr.begin(), tmp_corr.end());

      // uint32_t max_etp = bitpair_entropy[tmp_corr[0].m_segid[0]].m_entropy + bitpair_entropy[tmp_corr[0].m_segid[1]].m_entropy;
      // uint32_t max_etp_idx = 0;
      // uint32_t min_corr = tmp_corr[0].m_entropy*TOLERANCE_RATE;
      // for (uint32_t i=1; i<tmp_corr.size(); ++i) {
      //    if (tmp_corr[i].m_entropy > min_corr) {
      //       break;
      //    }
      //    if (((bitpair_entropy[tmp_corr[i].m_segid[0]].m_entropy + bitpair_entropy[tmp_corr[i].m_segid[1]].m_entropy) > max_etp)
      //       && !is_existed(bitpair_entropy[tmp_corr[i].m_segid[0]].m_segid[0],bitpair_entropy[tmp_corr[i].m_segid[1]].m_segid)
      //       && !is_existed(bitpair_entropy[tmp_corr[i].m_segid[0]].m_segid[1],bitpair_entropy[tmp_corr[i].m_segid[1]].m_segid))
      //    {
      //       max_etp_idx = i;
      //       max_etp = bitpair_entropy[tmp_corr[i].m_segid[0]].m_entropy + bitpair_entropy[tmp_corr[i].m_segid[1]].m_entropy;
      //    }
      // }
      // c9_seq_a.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[0]].m_segid[0]);
      // c9_seq_a.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[1]].m_segid[0]);
      // c9_seq_b.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[0]].m_segid[1]);
      // c9_seq_b.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[1]].m_segid[1]);

      // used_bits.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[0]].m_segid[0]);
      // used_bits.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[1]].m_segid[0]);
      // used_bits.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[0]].m_segid[1]);
      // used_bits.push_back(bitpair_entropy[tmp_corr[max_etp_idx].m_segid[1]].m_segid[1]);

      // used_pair_ids.push_back(tmp_corr[max_etp_idx].m_segid[0]);
      // used_pair_ids.push_back(tmp_corr[max_etp_idx].m_segid[1]);

      // cout << "max_etp: " << max_etp << ". max_etp_idx: " << max_etp_idx << ". min_corr: " << min_corr << endl;

      // cout << tmp_corr[max_etp_idx].to_string() << " (corr): " 
      //      << bitpair_entropy[tmp_corr[max_etp_idx].m_segid[0]].to_string() 
      //      << " (etp) vs. " 
      //      << bitpair_entropy[tmp_corr[max_etp_idx].m_segid[1]].to_string() << " (etp)" << endl;

      c9_seq_a.push_back(tmp_bitpair_etp[0].m_segid[0]);
      c9_seq_b.push_back(tmp_bitpair_etp[0].m_segid[1]);
      used_bits.push_back(tmp_bitpair_etp[0].m_segid[0]);
      used_bits.push_back(tmp_bitpair_etp[0].m_segid[1]);
      used_pair_ids.push_back(get_correl_table_index(tmp_bitpair_etp[0].m_segid[0],tmp_bitpair_etp[0].m_segid[1]));

      // for (uint32_t i=0;i<bitpair_entropy.size(); ++i) {
      //    if (bitpair_entropy[i].is_id_matched(tmp_bitpair_etp[0].m_segid))
      //       used_pair_ids.push_back(i);
      // }

      cout << "c9_seq_a:  " << vec_to_string(c9_seq_a) << endl;
      cout << "c9_seq_b:  " << vec_to_string(c9_seq_b) << endl;

      // for (uint32_t n=c9_seq_a.size(); n < x_seq_a.size(); ++n) {
      //    vector<BitEntropy> bitpair_candidate_list;
      //    for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
      //       if (!is_existed(bitpair_entropy[i].m_segid[0], used_bits) && !is_existed(bitpair_entropy[i].m_segid[1], used_bits)) {
      //          uint32_t sum_corr = 0;
      //          BitEntropy bitpair_candidate;
      //          for (uint32_t j=0; j < used_pair_ids.size(); ++j) {
      //             uint32_t bpc_index = get_correl_table_index(i, used_pair_ids[j]);
      //             sum_corr += bitpair_correl_table[bpc_index].m_entropy;
      //          }
      //          bitpair_candidate.m_id = i;
      //          bitpair_candidate.m_entropy = sum_corr;
      //          bitpair_candidate_list.push_back(bitpair_candidate);
      //       }
      //    }
      //    cout << "\nbitpair_candidate_list size: " << bitpair_candidate_list.size() << endl;
      //    sort(bitpair_candidate_list.begin(), bitpair_candidate_list.end());

      //    uint32_t selected_candidate_id  = 0;
      //    uint32_t selected_candidate_etp = bitpair_entropy[bitpair_candidate_list[0].m_id].m_entropy;
      //    uint32_t min_corr = bitpair_candidate_list[0].m_entropy*TOLERANCE_RATE;
      //    for (uint32_t i=1; i < bitpair_candidate_list.size(); ++i) {
      //       if (bitpair_candidate_list[i].m_entropy > min_corr) {
      //          break;
      //       }
      //       uint32_t candidate_etp = bitpair_entropy[bitpair_candidate_list[i].m_id].m_entropy;
      //       if (candidate_etp > selected_candidate_etp) {
      //          selected_candidate_id = i;
      //          selected_candidate_etp = candidate_etp;
      //       }
      //    }

      //    c9_seq_a.push_back(bitpair_entropy[bitpair_candidate_list[selected_candidate_id].m_id].m_segid[0]);
      //    c9_seq_b.push_back(bitpair_entropy[bitpair_candidate_list[selected_candidate_id].m_id].m_segid[1]);
      //    used_bits.push_back(bitpair_entropy[bitpair_candidate_list[selected_candidate_id].m_id].m_segid[0]);
      //    used_bits.push_back(bitpair_entropy[bitpair_candidate_list[selected_candidate_id].m_id].m_segid[1]);
      //    used_pair_ids.push_back(bitpair_candidate_list[selected_candidate_id].m_id);

      //    cout << "max_etp: " << selected_candidate_etp 
      //         << ". min_corr: " << bitpair_candidate_list[0].m_entropy
      //         << ". tolerated min_corr: " << min_corr
      //         << ". selected candidate rank: " << selected_candidate_id
      //         << endl;
      //    cout << bitpair_candidate_list[selected_candidate_id].to_string() << " (corr): " 
      //         << bitpair_entropy[bitpair_candidate_list[selected_candidate_id].m_id].to_string() << " (etp)" << endl;
      //    // cout << "used_bits: " << vec_to_string(used_bits) << endl;
      //    // cout << "used_pair_ids: " << vec_to_string(used_pair_ids) << endl;

      //    // cout << "Sorted BPC (top " << selected_candidate_id+1 << "):" << endl;
      //    // for (uint32_t i=0; i < selected_candidate_id+1; ++i) {
      //    //    cout << bitpair_candidate_list[i].to_string() << " (corr): " 
      //    //       << bitpair_entropy[bitpair_candidate_list[i].m_id].to_string() << " (etp)" << endl;
      //    // }

      //    // cout << "c9_seq_a:  " << vec_to_string(c9_seq_a) << endl;
      //    // cout << "c9_seq_b:  " << vec_to_string(c9_seq_b) << endl;
      // }
      // cout << endl;



      for (uint32_t i=1; i < tmp_bitpair_etp.size(); ++i) {
         if (!is_existed(tmp_bitpair_etp[i].m_segid[0], used_bits) && !is_existed(tmp_bitpair_etp[i].m_segid[1], used_bits)) {
            c9_seq_a.push_back(tmp_bitpair_etp[i].m_segid[0]);
            c9_seq_b.push_back(tmp_bitpair_etp[i].m_segid[1]);
            used_bits.push_back(tmp_bitpair_etp[i].m_segid[0]);
            used_bits.push_back(tmp_bitpair_etp[i].m_segid[1]);
            used_pair_ids.push_back(get_correl_table_index(tmp_bitpair_etp[i].m_segid[0],tmp_bitpair_etp[i].m_segid[1]));
         }
         if (c9_seq_a.size() == x_seq_a.size()) {
            break;
         }
      }
      cout << endl;

      cout << "used_bits: " << vec_to_string(used_bits) << endl;
      cout << "used_pair_ids: " << vec_to_string(used_pair_ids) << endl;

      cout << "c9_seq_a:  " << vec_to_string(c9_seq_a) << endl;
      cout << "c9_seq_b:  " << vec_to_string(c9_seq_b) << endl;
      uint32_t c9_corr = compute_sum_correl_two_sequences(c9_seq_a, c9_seq_b);
      cout << "CORR c9:   " << c9_corr << endl << endl;

      double bp_etp_last = 0;
      double bp_cor_last = 0;
      double bp_etp_curr = 0;
      double bp_cor_curr = 0;
      for (uint32_t i=0; i < last_interval_bpe_ids.size(); ++i) {
         bp_etp_last += bitpair_entropy[last_interval_bpe_ids[i]].m_entropy;
      }
      for (uint32_t i=0; i < used_pair_ids.size(); ++i) {
         bp_etp_curr += bitpair_entropy[used_pair_ids[i]].m_entropy;
      }
      for (uint32_t i=1; i < last_interval_bpe_ids.size(); ++i) {
         for (uint32_t j=0; j < i; j++) {
            uint32_t bpc_index = get_correl_table_index(last_interval_bpe_ids[i], last_interval_bpe_ids[j]);
            bp_cor_last += bitpair_correl_table[bpc_index].m_entropy;
         }
      }
      for (uint32_t i=1; i < used_pair_ids.size(); ++i) {
         for (uint32_t j=0; j < i; j++) {
            uint32_t bpc_index = get_correl_table_index(used_pair_ids[i], used_pair_ids[j]);
            bp_cor_curr += bitpair_correl_table[bpc_index].m_entropy;
         }
      }

      double bp_pattern_last = 0;
      double bp_pattern_curr = 0;
      for (uint32_t i=0; i < last_interval_bpe_ids.size(); ++i) {
         bp_pattern_last += bitpair_pattern_hist[last_interval_bpe_ids[i]].m_entropy;
      }
      for (uint32_t i=0; i < used_pair_ids.size(); ++i) {
         bp_pattern_curr += bitpair_pattern_hist[used_pair_ids[i]].m_entropy;
      }
      double bp_pattern_diff = (num_interval>0)? (bp_pattern_curr-bp_pattern_last)*100/bp_pattern_last : 0;

      last_interval_bpe_ids = used_pair_ids;

      double bp_cor_diff = (num_interval>0)? (bp_cor_last-bp_cor_curr)*100/bp_cor_last : 0;
      double bp_etp_diff = (num_interval>0)? (bp_etp_curr-bp_etp_last)*100/bp_etp_last : 0;

      // hard-coded config
      x_seq_a = c9_seq_a;
      x_seq_b = c9_seq_b;
      cout << "Choose C9" << endl << endl;

      cout << "BPCOR Last Cfg:    " << std::setprecision(0) << bp_cor_last << endl;
      cout << "BPCOR Next Cfg:    " << std::setprecision(0) << bp_cor_curr << endl;
      cout << "BPCOR reduction:   " << std::setprecision(2) << bp_cor_diff << "%" << std::setprecision(5) << endl << endl;
      cout << "BPETP Last Cfg:    " << std::setprecision(0) << bp_etp_last << endl;
      cout << "BPETP Next Cfg:    " << std::setprecision(0) << bp_etp_curr << endl;
      cout << "BPETP improvement: " << std::setprecision(2) << bp_etp_diff << "%" << std::setprecision(5) << endl << endl;

      cout << "BPPAT Last Cfg:    " << std::setprecision(0) << bp_pattern_last << endl;
      cout << "BPPAT Next Cfg:    " << std::setprecision(0) << bp_pattern_curr << endl;
      cout << "BPPAT improvement:   " << std::setprecision(2) << bp_pattern_diff << "%" << std::setprecision(5) << endl << endl;

      // // hardcoded index function
      // // cout << "Hardcoded Cfg:\n";
      // double hardcoded_bp_etp = 0;
      // double hardcoded_bp_cor = 0;
      // double hardcoded_bp_pat = 0;
      // for (uint32_t i=0; i < hard_coded_bpe_ids.size(); ++i) {
      //    hardcoded_bp_etp += bitpair_entropy[hard_coded_bpe_ids[i]].m_entropy;
      //    hardcoded_bp_pat += bitpair_pattern_hist[hard_coded_bpe_ids[i]].m_entropy;
      // }
      // // cout << endl;
      // for (uint32_t i=1; i < hard_coded_bpe_ids.size(); ++i) {
      //    for (uint32_t j=0; j < i; j++) {
      //       uint32_t bpc_index = get_correl_table_index(hard_coded_bpe_ids[i], hard_coded_bpe_ids[j]);
      //       hardcoded_bp_cor += bitpair_correl_table[bpc_index].m_entropy;
      //       // cout << bitpair_correl_table[bpc_index].to_string() << " (corr): "
      //       //      << bitpair_entropy[bitpair_correl_table[bpc_index].m_segid[0]].to_string()
      //       //      << " (etp) vs. "
      //       //      << bitpair_entropy[bitpair_correl_table[bpc_index].m_segid[1]].to_string() << " (etp)" << endl;
      //    }
      // }
      // cout << "BPCOR Hardcoded Cfg: " << std::setprecision(0) << hardcoded_bp_cor << endl;
      // cout << "BPETP Hardcoded Cfg: " << std::setprecision(0) << hardcoded_bp_etp << endl;
      // cout << "BPPAT Hardcoded Cfg: " << std::setprecision(0) << hardcoded_bp_pat << endl;
      // cout << endl;
}
*/