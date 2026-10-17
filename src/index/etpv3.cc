#include "index/etpv3.h"

void EntropyV3::move_cache_data(MEMORY *lower_level) {
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
            uint32_t dest_way = replacement_getvictim(dest_index);
            BLOCK *dest_block = &block_array[dest_index][dest_way];

            bool fill_ready = remap_tie_break(src_block, dest_block, lower_level);
            // overwrite destination block with source block
            // we need not to up-invalidate the data in the destination block since the cache is non-inclusive
            if (fill_ready) {
                if (dest_block->valid)
                    increment_eviction(dest_index);
                override_block(src_block, dest_block);
                replacement_update(dest_index, dest_way, map_ptr, way);  // etp3c: no update, etp3d: original SRRIP update
            }

            // invalidate the source block
            src_block->valid = 0;
            // update extra read statistics
            num_read++;
        }
    }
    map_ptr++;
}

bool EntropyV3::remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level) {
    if (!dest->valid)
        return true;
    bool overwrite_dest = true;

    // prefetch -> dirty
    if (!dest->prefetch && src->prefetch) {
        // dest NOT prefetch but the src is, drop the src, regardless of src dirty state
        num_drop++;
        overwrite_dest = false;
        if (src->dirty) {
            bool writeback_success = force_writeback(src, lower_level, FILL_LEVEL, CPU_ID);
            if (writeback_success) {
                num_writeback++;
            }
        }
    }
    else if (dest->dirty) {
        // destination is dirty, regardless of its prefetch state
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
        // destination is clean -> just overwrite, no further action
    }
    return overwrite_dest;
}

bool EntropyV3::should_update(bool is_hit, uint32_t type) {
    if ((V3_UPDATE_HITMISS_SETTING == 1) && is_hit)                         // return false for all hits
        return false;
    if ((V3_UPDATE_HITMISS_SETTING == 2) && !is_hit)                        // return false for all misses
        return false;
    if ((V3_UPDATE_HITMISS_SETTING == 3) && is_hit && (type != PF_USEFUL))  // return false for all hits except PF_USEFUL
        return false;
    if (V3_SKIP_UPDATE_PF && (type == PREFETCH))                            // return false for all prefetches
        return false;
    if (V3_SKIP_UPDATE_WB && (type == WRITEBACK))                           // return false for all writebacks
        return false;
    if (V3_SKIP_UPDATE_WB_HIT && (type == WRITEBACK) && is_hit)             // return false for all writeback hits
        return false;
    return true;
}

uint32_t EntropyV3::replacement_getvictim(uint32_t index) {
    if (REPLACEMENT_POLICY == 0)
        return get_lru_victim(index);
    else if (REPLACEMENT_POLICY == 1)
        return get_rrip_victim(index);
    else if (REPLACEMENT_POLICY == 2)
        return get_rlr_victim(index);
    else
        assert(false);
}

void EntropyV3::replacement_update(uint32_t index, uint32_t way, uint32_t remapping_index, uint32_t remapping_way) {
    if (REPLACEMENT_POLICY == 0) {
        lru_update(index, way, block_array[remapping_index][remapping_way].lru);
    }
    else if (REPLACEMENT_POLICY == 1) {
        rrip_update(index, way, rrpv[remapping_index][remapping_way]);
    }
    else if (REPLACEMENT_POLICY == 2) {
        rlr_update(index, way, remapping_index, remapping_way);
    }
    else {
        assert(false);
    }
}

// return the correlation counter between bit A and bit B in the correlation table
uint32_t EntropyV3::get_correl(uint32_t bit_a, uint32_t bit_b) {
    uint32_t index = compute_pair_table_idx(bit_a, bit_b);
    return correl_table[index].m_entropy;
}

uint32_t EntropyV3::compute_sum_correl_two_sequences(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    uint32_t sum_correl = 0;
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        sum_correl += get_correl(x_seq_a[i], x_seq_b[i]);
    }
    return sum_correl;
}

uint32_t EntropyV3::compute_pair_table_idx(uint32_t bit_a, uint32_t bit_b) {
    if (bit_a > bit_b) {
        return (bit_b*(MAX_ADDRESS_BITS-IDX_SIZE) + (bit_a-IDX_SIZE));
    }
    else {
        return (bit_a*(MAX_ADDRESS_BITS-IDX_SIZE) + (bit_b-IDX_SIZE));
    }
}

void EntropyV3::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
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

    if (should_update(is_hit, type)) {
        etp_tracker_access_update(address);
        // m_etpv3_addr_hist.push_back(address);
        num_updates++;
    }


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
        cout << "   num_reset_itv_ptn:     " << num_reset_itv_ptn << endl;
        cout << "   num_reset_bitpair_etp: " << num_reset_bitpair_etp << endl;
        cout << "   num_reset_bitpair_ptn: " << num_reset_bitpair_ptn << endl;
        cout << "   num_total_updates:     " << num_updates << endl;
        cout << "========================================================================" << endl;

        // save the current XOR-hash index config
        m_last_seq_a = m_curr_seq_a;
        m_last_seq_b = m_curr_seq_b;

        // compute new XOR-hash index config for new interval
        // etp_tracker_interval_update();
        compute_new_config(m_curr_seq_a, m_curr_seq_b);

        m_should_remap = check_remap();

        cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
        cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
        cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
        cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
        cout << "should_remap: " << (m_should_remap ? "Yes" : "No") << endl << endl;

        memcpy(PAST_PI_EVICTION, PI_EVICTION, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_E_ACCESS, 0, NUM_SET * sizeof(*PI_E_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

        num_interval++;
        map_ptr    = 0;
        access_ctr = 0;
        num_updates= 0;
    }
    warmup_check();
}

void EntropyV3::etp_tracker_access_update(uint64_t address) {
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

        for (uint32_t i=0; i < IDX_SIZE; ++i) {
            for (uint32_t j=IDX_SIZE; j < MAX_ADDRESS_BITS; ++j) {
                uint32_t bit_i_j_flip_correl = ((bit_flips >> i) & 1) ^ ((bit_flips >> j) & 1);
                if (bit_i_j_flip_correl == 0) {
                    correl_table[correl_table_idx].m_entropy++;
                    if (correl_table[correl_table_idx].m_entropy >= V3_COR_CTR_MAX) {
                        should_reset_correl = true;
                    }
                }
                correl_table_idx++;
            }
        }
    }

    // update entropy trackers
    bool should_reset_entropy = false;
    bool should_reset_ptn_entropy = false;
    for (uint32_t idx=0; idx < m_addr_hist.size(); ++idx) {
        // do not update the trackers if curr_addr == last_addr
        if (address == m_addr_hist[idx])
            continue;

        // compare curr_addr vs. last_addr bit by bit
        for (uint32_t i=0; i < MAX_ADDRESS_BITS; ++i) {
            bool is_flipped = (bool) ((address >> i) & 1) ^ ((m_addr_hist[idx] >> i) & 1);
            if (is_flipped) {    // A ^ B == 1 => A != B => high entropy => increment entropy
                itv_entropy[i].m_entropy++;
                if (itv_entropy[i].m_entropy >= V3_ETP_CTR_MAX) {
                    should_reset_entropy = true;
                }
            }
            itv_ptn_entropy[i].m_entropy += (is_flipped ^ last_access_flip_behavior[i]);
            last_access_flip_behavior[i] = is_flipped;
            if (itv_ptn_entropy[i].m_entropy >= V3_ETP_CTR_MAX) {
                should_reset_ptn_entropy = true;
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
                if (bitpair_entropy[i].m_entropy >= V3_ETP_CTR_MAX) {
                    should_reset_bitpair_entropy = true;
                }
            }
            bitpair_ptn_ctr[i].m_entropy += (bitpair_flip ^ last_access_bitpair_flip_behavior[i]);
            last_access_bitpair_flip_behavior[i] = bitpair_flip;
            if (bitpair_ptn_ctr[i].m_entropy >= V3_ETP_CTR_MAX) {
                should_reset_bitpair_pattern = true;
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
            itv_entropy[i].m_entropy /= 2;
        }
    }

    if (should_reset_ptn_entropy) {
        num_reset_itv_ptn++;
        for (uint32_t i=0; i < itv_ptn_entropy.size(); ++i) {
            itv_ptn_entropy[i].m_entropy /= 2;
        }
    }

    if (should_reset_correl) {
        num_reset_cor++;
        for (uint32_t i=0; i < correl_table.size(); ++i) {
            correl_table[i].m_entropy /= 2;
        }
    }

    if (should_reset_bitpair_entropy) {
        num_reset_bitpair_etp++;
        for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
            bitpair_entropy[i].m_entropy /= 2;
        }
    }

    if (should_reset_bitpair_pattern) {
        num_reset_bitpair_ptn++;
        for (uint32_t i=0; i < bitpair_ptn_ctr.size(); ++i) {
            bitpair_ptn_ctr[i].m_entropy /= 2;
        }
    }
}

void EntropyV3::compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    // sort the index bits based on their entropy, from lowest to highest
    // vector<BitEntropy> sorted_ovr = ovr_entropy;
    // sort(sorted_ovr.begin(), sorted_ovr.end());
    // std::reverse(sorted_ovr.begin(), sorted_ovr.end());
    // cout << "Sorted OVR:   " << etp_vector_to_string(sorted_ovr) << endl;
    cout << endl;
    vector<BitEntropy> sorted_itv = itv_entropy;
    sort(sorted_itv.begin(), sorted_itv.end());
    std::reverse(sorted_itv.begin(), sorted_itv.end());
    cout << "Sorted ITV:   " << etp_vector_to_string(sorted_itv) << endl;

    vector<BitEntropy> sorted_itv_ptn = itv_ptn_entropy;
    sort(sorted_itv_ptn.begin(), sorted_itv_ptn.end());
    std::reverse(sorted_itv_ptn.begin(), sorted_itv_ptn.end());
    cout << "Sorted PTN:   " << etp_vector_to_string(sorted_itv_ptn) << endl;

    vector<BitEntropy> sorted_bitpair_etp = bitpair_entropy;
    sort(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    std::reverse(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    cout << "Sorted BPE:   " << etp_vector_to_string(sorted_bitpair_etp) << endl;

    vector<BitEntropy> sorted_bitpair_pattern = bitpair_ptn_ctr;
    sort(sorted_bitpair_pattern.begin(), sorted_bitpair_pattern.end());
    std::reverse(sorted_bitpair_pattern.begin(), sorted_bitpair_pattern.end());
    cout << "Sorted BPP:   " << etp_vector_to_string(sorted_bitpair_pattern) << endl;

    vector<BitEntropy> sorted_corr = correl_table;
    sort(sorted_corr.begin(), sorted_corr.end());
    cout << "Sorted COR:   " << etp_vector_to_string(sorted_corr) << endl;
    cout << endl;

    // vector<uint32_t> c1_seq_a;
    // vector<uint32_t> c1_seq_b;
    // get_cfg_1(x_seq_a, x_seq_b, c1_seq_a, c1_seq_b);
    // cout << "c1_seq_a:  " << vec_to_string(c1_seq_a) << endl;
    // cout << "c1_seq_b:  " << vec_to_string(c1_seq_b) << endl;
    // x_seq_b = c1_seq_b;

    // vector<uint32_t> c2_seq_a;
    // vector<uint32_t> c2_seq_b;
    // get_cfg_2(x_seq_a, x_seq_b, c2_seq_a, c2_seq_b);
    // cout << "c2_seq_a:  " << vec_to_string(c2_seq_a) << endl;
    // cout << "c2_seq_b:  " << vec_to_string(c2_seq_b) << endl;
    // x_seq_b = c2_seq_b;

    // vector<uint32_t> c3_seq_a;
    // vector<uint32_t> c3_seq_b;
    // get_cfg_3(x_seq_a, x_seq_b, c3_seq_a, c3_seq_b);
    // cout << "c3_seq_a:  " << vec_to_string(c3_seq_a) << endl;
    // cout << "c3_seq_b:  " << vec_to_string(c3_seq_b) << endl;
    // x_seq_b = c3_seq_b;

    // vector<uint32_t> cetp_seq_a;
    // vector<uint32_t> cetp_seq_b;
    // get_cfg_etp(x_seq_a, x_seq_b, cetp_seq_a, cetp_seq_b);
    // cout << "cetp_seq_a:  " << vec_to_string(cetp_seq_a) << endl;
    // cout << "cetp_seq_b:  " << vec_to_string(cetp_seq_b) << endl;
    // x_seq_b = cetp_seq_b;

    // vector<uint32_t> c4_seq_a;
    // vector<uint32_t> c4_seq_b;
    // get_cfg_4(x_seq_a, x_seq_b, c4_seq_a, c4_seq_b);
    // cout << "c4_seq_a:  " << vec_to_string(c4_seq_a) << endl;
    // cout << "c4_seq_b:  " << vec_to_string(c4_seq_b) << endl;
    // x_seq_b = c4_seq_b;

    vector<uint32_t> c5_seq_a;
    vector<uint32_t> c5_seq_b;
    get_cfg_5(x_seq_a, x_seq_b, c5_seq_a, c5_seq_b);
    cout << "c5_seq_a:  " << vec_to_string(c5_seq_a) << endl;
    cout << "c5_seq_b:  " << vec_to_string(c5_seq_b) << endl;
    x_seq_a = c5_seq_a;
    x_seq_b = c5_seq_b;

    // vector<uint32_t> c6_seq_a;
    // vector<uint32_t> c6_seq_b;
    // get_cfg_6(x_seq_a, x_seq_b, c6_seq_a, c6_seq_b);
    // cout << "c6_seq_a:  " << vec_to_string(c6_seq_a) << endl;
    // cout << "c6_seq_b:  " << vec_to_string(c6_seq_b) << endl;
    // x_seq_b = c6_seq_b;

    // vector<uint32_t> c7_seq_a;
    // vector<uint32_t> c7_seq_b;
    // get_cfg_7(x_seq_a, x_seq_b, c7_seq_a, c7_seq_b);
    // cout << "c7_seq_a:  " << vec_to_string(c7_seq_a) << endl;
    // cout << "c7_seq_b:  " << vec_to_string(c7_seq_b) << endl;
    // x_seq_b = c7_seq_b;

    // vector<uint32_t> c8_seq_a;
    // vector<uint32_t> c8_seq_b;
    // get_cfg_8(x_seq_a, x_seq_b, c8_seq_a, c8_seq_b);
    // cout << "c8_seq_a:  " << vec_to_string(c8_seq_a) << endl;
    // cout << "c8_seq_b:  " << vec_to_string(c8_seq_b) << endl;
    // x_seq_a = c8_seq_a;
    // x_seq_b = c8_seq_b;

    // vector<uint32_t> cgen_seq_a;
    // vector<uint32_t> cgen_seq_b;
    // get_cfg_genetic(x_seq_a, x_seq_b, cgen_seq_a, cgen_seq_b);
    // cout << "cgen_seq_a:  " << vec_to_string(cgen_seq_a) << endl;
    // cout << "cgen_seq_b:  " << vec_to_string(cgen_seq_b) << endl;
    // x_seq_a = cgen_seq_a;
    // x_seq_b = cgen_seq_b;

    // compute ETP
    double etp_diff = 0;
    double next_etp = 0;
    double last_etp = 0;
    for (uint32_t i=0; i < x_seq_b.size(); ++i) {
        next_etp += itv_entropy[x_seq_b[i]].m_entropy;
    }
    if (num_interval>0) {
        for (uint32_t i=0; i < m_last_seq_b.size(); ++i) {
            last_etp += itv_entropy[m_last_seq_b[i]].m_entropy;
        }
        etp_diff = (next_etp-last_etp)*100/abs(last_etp);  // compute etp difference
    }
    cout << endl;

    // compute COR
    double next_corr = (double)compute_sum_correl_two_sequences(x_seq_a, x_seq_b);
    double last_corr = (num_interval>0)? (double)compute_sum_correl_two_sequences(m_last_seq_a, m_last_seq_b) : 0;
    double corr_diff = (num_interval>0)? (last_corr-next_corr)*100/last_corr : 0;

    // compute BPE
    double bp_etp_last = 0;
    double bp_etp_next = 0;
    for (uint32_t i=0; i < x_seq_a.size(); ++i) {
        uint32_t index = compute_pair_table_idx(x_seq_a[i], x_seq_b[i]);
        bp_etp_next += bitpair_entropy[index].m_entropy;
    }
    if (num_interval>0) {
        cout << endl;
        for (uint32_t i=0; i < m_last_seq_b.size(); ++i) {
            uint32_t index = compute_pair_table_idx(m_last_seq_a[i], m_last_seq_b[i]);
            bp_etp_last += bitpair_entropy[index].m_entropy;
        }
    }
    double bp_etp_diff = (num_interval>0)? (bp_etp_next-bp_etp_last)*100/bp_etp_last : 0;
    cout << endl;

    cout << "seq_A last:      " << vec_to_string(m_last_seq_a) << endl;
    cout << "seq_B last:      " << vec_to_string(m_last_seq_b) << endl;
    cout << "seq_A candidate: " << vec_to_string(x_seq_a) << endl;
    cout << "seq_B candidate: " << vec_to_string(x_seq_b) << endl << endl;
    cout << "ETP Last Cfg:    " << std::setprecision(0) << last_etp << endl;
    cout << "ETP Next Cfg:    " << next_etp << endl;
    cout << "ETP diff:        " << std::setprecision(2) << etp_diff << std::setprecision(5) << "%" << endl << endl;
    cout << "CORR Last Cfg:   " << std::setprecision(0) << last_corr << endl;
    cout << "CORR Next Cfg:   " << next_corr << endl;
    cout << "CORR diff:       " << std::setprecision(2) << corr_diff << std::setprecision(5) << "%" << endl << endl;
    cout << "BPETP Last Cfg:  " << std::setprecision(0) << bp_etp_last << endl;
    cout << "BPETP Next Cfg:  " << bp_etp_next << endl;
    cout << "BPETP imprv:     " << std::setprecision(2) << bp_etp_diff << std::setprecision(5) << "%" << endl << endl;

    if (num_interval < V3_WARMUP_INTERVALS) {
        cout << "Reset counters on warmup intervals\n";
        num_reset_itv++;
        for (uint32_t i=0; i < itv_entropy.size(); ++i) {
            itv_entropy[i].m_entropy /= 2;
        }

        num_reset_cor++;
        for (uint32_t i=0; i < correl_table.size(); ++i) {
            correl_table[i].m_entropy /= 2;
        }

        num_reset_bitpair_etp++;
        for (uint32_t i=0; i < bitpair_entropy.size(); ++i) {
            bitpair_entropy[i].m_entropy /= 2;
        }

        num_reset_bitpair_ptn++;
        for (uint32_t i=0; i < bitpair_ptn_ctr.size(); ++i) {
            bitpair_ptn_ctr[i].m_entropy /= 2;
        }

        num_reset_itv_ptn++;
        for (uint32_t i=0; i < itv_ptn_entropy.size(); ++i) {
            itv_ptn_entropy[i].m_entropy /= 2;
        }

        if ((num_interval > 0) && (bp_etp_diff <= 0)) {   // etp3b11/etp3b12/etp3tst14
            x_seq_a = m_last_seq_a;
            x_seq_b = m_last_seq_b;
            cout << "Keep current candidate." << endl;
        }
    }
    else if ((V3_SENSE_THRESHOLD >= 0) && !is_zero_segment(m_last_seq_b)) {
        // if ((etp_diff+corr_diff) > V3_SENSE_THRESHOLD) {
        // if ((bp_etp_diff > V3_SENSE_THRESHOLD) || (etp_diff > V3_SENSE_THRESHOLD)) {
        if (bp_etp_diff > V3_SENSE_THRESHOLD) {
            cout << "Switch to new candidate." << endl;
        }
        else {
            x_seq_a = m_last_seq_a;
            x_seq_b = m_last_seq_b;
            cout << "Keep current candidate." << endl;
        }
    }
    return;
}

/*
 * get 11 highest entropy bits (seq_b), pair with the nohash index sequence (seq_a). 
 * swap the bits in seq_b based on its correlation with seq_a corresponding bits.
 */
void EntropyV3::get_cfg_1(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_itv = itv_entropy;
    sort(sorted_itv.begin(), sorted_itv.end());
    std::reverse(sorted_itv.begin(), sorted_itv.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    uint32_t i=0;
    while (c_seq_b.size() < c_seq_a.size()) {
        if (sorted_itv[i].m_id >= IDX_SIZE) {
            c_seq_b.push_back(sorted_itv[i].m_id);
        }
        ++i;
    }
    cout << "c_seq_b og:  " << vec_to_string(c_seq_b) << endl;

    vector<uint32_t> reordered_indices = {};
    for (int32_t i=c_seq_a.size()-1; i >= 0; --i) {
        vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
        for (uint32_t j=0; j < c_seq_b.size(); ++j) {
            seq_a_bit_correl.push_back(get_correl(c_seq_a[i], c_seq_b[j]));
        }
        uint32_t min_correl_idx = find_min_index(seq_a_bit_correl, reordered_indices);
        reordered_indices.push_back(i);

        // pair this bit in A with the least correlated bit in B
        if (min_correl_idx != i) {
            swap(c_seq_b[i], c_seq_b[min_correl_idx]);
        }
    }
    return;
}

/*
 * nohash index sequence (seq_a).
 * get the lowest correlation bit to pair with each bit in A. 
 */
void EntropyV3::get_cfg_7(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_itv = itv_entropy;
    sort(sorted_itv.begin(), sorted_itv.end());
    std::reverse(sorted_itv.begin(), sorted_itv.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    vector<uint32_t> avail_bits;
    for (uint32_t i=0; i < sorted_itv.size(); ++i) {
        if ((sorted_itv[i].m_id >= IDX_SIZE) && (avail_bits.size() < ETP_CUTOFF_STARTRATE)) {
            // if ((sorted_itv[i].m_id >= IDX_SIZE) && (sorted_itv[i].m_entropy > ETP_CUTOFF_STARTRATE)) {
            avail_bits.push_back(sorted_itv[i].m_id);
        }
    }
    cout << "available_bits:  " << vec_to_string(avail_bits) << endl;

    for (int32_t i=c_seq_a.size()-1; i >= 0; --i) {
        vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
        for (uint32_t j=0; j < avail_bits.size(); ++j) {
            seq_a_bit_correl.push_back(get_correl(c_seq_a[i], avail_bits[j]));
        }
        uint32_t min_correl_idx = find_min_index(seq_a_bit_correl);
        c_seq_b.push_back(avail_bits[min_correl_idx]);
        avail_bits.erase(avail_bits.begin()+min_correl_idx);
    }
    reverse(c_seq_b.begin(), c_seq_b.end());
    return;
}

/*
 * seq_a = nohash index sequence.
 * for each seq_a bit, find the corresponding bit that has the highest BPP with it and put it in seq_b 
 */
void EntropyV3::get_cfg_2(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_bitpair_pattern = bitpair_ptn_ctr;
    sort(sorted_bitpair_pattern.begin(), sorted_bitpair_pattern.end());
    std::reverse(sorted_bitpair_pattern.begin(), sorted_bitpair_pattern.end());

    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    vector<uint32_t> used_bits;
    for (int32_t i=c_seq_a.size()-1; i >= 0; --i) {
        for (uint32_t j=0; j < sorted_bitpair_pattern.size(); ++j) {
            if ((sorted_bitpair_pattern[j].m_segid[0] == c_seq_a[i]) && !is_existed(sorted_bitpair_pattern[j].m_segid[1], used_bits)) {
                c_seq_b.push_back(sorted_bitpair_pattern[j].m_segid[1]);
                used_bits.push_back(sorted_bitpair_pattern[j].m_segid[1]);
                break;
            }
        }
    }
    std::reverse(c_seq_b.begin(), c_seq_b.end());
    return;
}

void EntropyV3::get_cfg_etp(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_itv = itv_entropy;
    sort(sorted_itv.begin(), sorted_itv.end());
    std::reverse(sorted_itv.begin(), sorted_itv.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    uint32_t i=0;
    while (c_seq_b.size() < c_seq_a.size()) {
        if (sorted_itv[i].m_id >= IDX_SIZE) {
            c_seq_b.push_back(sorted_itv[i].m_id);
        }
        ++i;
    }
    std::reverse(c_seq_b.begin(), c_seq_b.end());

    cout << "FIXED_IDX ENTROPY Candidate Stats:\n";
    for (int32_t i=0; i < c_seq_b.size(); ++i) {
        uint32_t index = compute_pair_table_idx(i, c_seq_b[c_seq_b.size()-1-i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << i << "]: " << itv_entropy[i].m_entropy
             << ". ETP["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << ". PTN["  << i << "]: " << itv_ptn_entropy[i].m_entropy
             << ". PTN["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_ptn_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << endl;
    }
    cout << endl;

    return;
}

/*
 * seq_a = nohash index sequence.
 * find the lowest correl bit pair, put them in A and B, use as seed.
 * for every next bit in A, find the corresponding bit in B that has the correl score around 
 * CUTOFF_PRORATE * correl of the previous selected bitpair in A/B
 */
void EntropyV3::get_cfg_3(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_corr = correl_table;
    sort(sorted_corr.begin(), sorted_corr.end());

    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    vector<uint32_t> sorted_nohash_idx_sequence_corr_based;
    for (uint32_t i=0; i < sorted_corr.size(); ++i) {
        if ((sorted_corr[i].m_segid[0] < IDX_SIZE) && !is_existed(sorted_corr[i].m_segid[0], sorted_nohash_idx_sequence_corr_based)) {
            sorted_nohash_idx_sequence_corr_based.push_back(sorted_corr[i].m_segid[0]);
        }
    }
    // cout << "sorted_nohash_idx_sequence_corr_based: " << vec_to_string(sorted_nohash_idx_sequence_corr_based) << endl;

    vector<uint32_t> used_bits;
    uint32_t next_cutoff_corr=sorted_corr[0].m_entropy;  // start from max correlation score
    for (uint32_t i=0; i < sorted_nohash_idx_sequence_corr_based.size(); ++i) {
        bool found = false;
        uint32_t start_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_corr_based[i],IDX_SIZE);
        uint32_t end_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_corr_based[i],MAX_ADDRESS_BITS-1)+1;
        cout << "next_cutoff_corr: " << next_cutoff_corr << endl;

        vector<BitEntropy> extracted_bit_correl_table;
        double ETP_CUTOFF_RATE = ETP_CUTOFF_STARTRATE;

        cout << "\nStats list of bit: " << sorted_nohash_idx_sequence_corr_based[i] << endl;
        for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
            cout << "CORR: " << correl_table[j].to_string()
                 << ". BPE: " << bitpair_entropy[j].to_string() 
                 << ". BPP: " << bitpair_ptn_ctr[j].to_string()
                 << ". ETP[" << sorted_nohash_idx_sequence_corr_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy
                 << ". ETP[" << correl_table[j].m_segid[1] << "]: " << itv_entropy[correl_table[j].m_segid[1]].m_entropy
                 << ". PTN[" << sorted_nohash_idx_sequence_corr_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy
                 << ". PTN[" << correl_table[j].m_segid[1] << "]: " << itv_ptn_entropy[correl_table[j].m_segid[1]].m_entropy
                 << endl;
        }
        cout << endl;

        while (!found) {
            for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
                if ((correl_table[j].m_entropy <= next_cutoff_corr) && !is_existed(correl_table[j].m_segid[1], used_bits)
                    && (itv_entropy[correl_table[j].m_segid[1]].m_entropy > itv_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy*ETP_CUTOFF_RATE))
                    extracted_bit_correl_table.push_back(correl_table[j]);
            }
            sort(extracted_bit_correl_table.begin(), extracted_bit_correl_table.end());

            if (extracted_bit_correl_table.size() > 0) {
                used_bits.push_back(extracted_bit_correl_table[extracted_bit_correl_table.size()-1].m_segid[1]);
                next_cutoff_corr = extracted_bit_correl_table[extracted_bit_correl_table.size()-1].m_entropy * CUTOFF_PRORATE;
                found = true;
            }

            if (!found) {
                extracted_bit_correl_table.resize(0);
                for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
                    if ((correl_table[j].m_entropy > next_cutoff_corr) && !is_existed(correl_table[j].m_segid[1], used_bits)
                        && (itv_entropy[correl_table[j].m_segid[1]].m_entropy > itv_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy*ETP_CUTOFF_RATE))
                        extracted_bit_correl_table.push_back(correl_table[j]);
                }
                sort(extracted_bit_correl_table.begin(), extracted_bit_correl_table.end());

                if (extracted_bit_correl_table.size() > 0) {
                    used_bits.push_back(extracted_bit_correl_table[0].m_segid[1]);
                    next_cutoff_corr = extracted_bit_correl_table[0].m_entropy * CUTOFF_PRORATE;
                    found = true;
                }

                if (!found) {
                    ETP_CUTOFF_RATE -= 0.01;
                    cout << "extracted_bit_correl_table empty. Reduce cutoff rate to: " << ETP_CUTOFF_RATE << endl;

                    if (ETP_CUTOFF_RATE < 0) {
                        cout << "CORR: " << endl;
                        for (uint32_t i=start_search_idx; i < end_search_idx; ++i) {
                            cout << correl_table[i].to_string() << endl;
                        }
                        assert(false);  // we should not reach here
                    }
                }
            }
        }

        cout << "extracted_bit_correl_table: " << endl;
        for (uint32_t i=0; i < extracted_bit_correl_table.size(); ++i) {
            uint32_t index = compute_pair_table_idx(extracted_bit_correl_table[i].m_segid[0], extracted_bit_correl_table[i].m_segid[1]);
            cout << extracted_bit_correl_table[i].to_string() 
                 << ". BPE: " << bitpair_entropy[index].to_string() 
                 << ". BPP: " << bitpair_ptn_ctr[index].to_string()
                 << ". ETP[" << extracted_bit_correl_table[i].m_segid[0] << "]: " << itv_entropy[extracted_bit_correl_table[i].m_segid[0]].m_entropy
                 << ". ETP[" << extracted_bit_correl_table[i].m_segid[1] << "]: " << itv_entropy[extracted_bit_correl_table[i].m_segid[1]].m_entropy
                 << endl;
        }
        cout << "used_bits: " << vec_to_string(used_bits) << endl;
    }

    // cout << "CORR: " << endl;
    // for (uint32_t i=0; i < correl_table.size(); ++i) {
    //     cout << correl_table[i].to_string() << endl;
    // }

    cout << "\nGETCFG3 Candidate Stats:\n";
    for (uint32_t i=0; i < used_bits.size(); ++i) {
        uint32_t index = compute_pair_table_idx(sorted_nohash_idx_sequence_corr_based[i],used_bits[i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << sorted_nohash_idx_sequence_corr_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy
             << ". ETP["  << used_bits[i] << "]: " << itv_entropy[used_bits[i]].m_entropy
             << ". PTN["  << sorted_nohash_idx_sequence_corr_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_corr_based[i]].m_entropy
             << ". PTN["  << used_bits[i] << "]: " << itv_ptn_entropy[used_bits[i]].m_entropy
             << endl;
    }
    cout << endl;

    c_seq_b = {15,31,26,30,23,16,11,13,20,12,14};   // streamcluster
    // c_seq_b = {15,17,26,30,18,16,11,13,20,12,14};   // streamcluster test
    // c_seq_b = {30,29,20,26,18,16,21,17,15,12,14};   // streamcluster test
    // c_seq_b = {11,19,21,28,31,29,26,22,16,12,14};   // bodytrack

    cout << "FIXED_IDX_GENETIC Candidate Stats:\n";
    for (int32_t i=0; i < c_seq_b.size(); ++i) {
        uint32_t index = compute_pair_table_idx(i, c_seq_b[c_seq_b.size()-1-i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << i << "]: " << itv_entropy[i].m_entropy
             << ". ETP["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << ". PTN["  << i << "]: " << itv_ptn_entropy[i].m_entropy
             << ". PTN["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_ptn_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << endl;
    }

    cout << endl;
    // for (uint32_t i=0; i < c_seq_a.size(); ++i) {
    //     for (uint32_t j=0; j < sorted_nohash_idx_sequence_corr_based.size(); ++j) {
    //         if (sorted_nohash_idx_sequence_corr_based[j] == c_seq_a[i])
    //             c_seq_b.push_back(used_bits[j]);
    //     }
    // }
    return;
}

/*
 * get 11 highest PATTERN entropy bits (seq_b), pair with the nohash index sequence (seq_a). 
 * swap the bits in seq_b based on its correlation with seq_a corresponding bits.
 * equivalent to get_cfg_1 (ETP)
 */
void EntropyV3::get_cfg_4(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_itv_ptn = itv_ptn_entropy;
    sort(sorted_itv_ptn.begin(), sorted_itv_ptn.end());
    std::reverse(sorted_itv_ptn.begin(), sorted_itv_ptn.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    uint32_t i=0;
    while (c_seq_b.size() < c_seq_a.size()) {
        if (sorted_itv_ptn[i].m_id >= IDX_SIZE) {
            c_seq_b.push_back(sorted_itv_ptn[i].m_id);
        }
        ++i;
    }
    // std::reverse(c_seq_b.begin(), c_seq_b.end());
    cout << "c_seq_b og:  " << vec_to_string(c_seq_b) << endl;

    vector<uint32_t> reordered_indices = {};
    for (int32_t i=c_seq_a.size()-1; i >= 0; --i) {
        vector<uint32_t> seq_a_bit_correl = {};   // correl btw this bit in A vs every bit in B
        for (uint32_t j=0; j < c_seq_b.size(); ++j) {
            seq_a_bit_correl.push_back(get_correl(c_seq_a[i], c_seq_b[j]));
        }
        uint32_t min_correl_idx = find_min_index(seq_a_bit_correl, reordered_indices);
        reordered_indices.push_back(i);

        // pair this bit in A with the least correlated bit in B
        if (min_correl_idx != i) {
            swap(c_seq_b[i], c_seq_b[min_correl_idx]);
        }
    }

    cout << "GETCFG4 PATTERN ENTROPY Candidate Stats:\n";
    for (int32_t i=0; i < c_seq_b.size(); ++i) {
        uint32_t index = compute_pair_table_idx(i, c_seq_b[c_seq_b.size()-1-i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << i << "]: " << itv_entropy[i].m_entropy
             << ". ETP["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << ". PTN["  << i << "]: " << itv_ptn_entropy[i].m_entropy
             << ". PTN["  << c_seq_b[c_seq_b.size()-1-i] << "]: " << itv_ptn_entropy[c_seq_b[c_seq_b.size()-1-i]].m_entropy
             << endl;
    }
    cout << endl;
    return;
}

/*
 * seq_a = nohash index sequence.
 * pair each bit in A with a bit in B in a way that the bitpair entropy (BPE) 
 * decreases by a fixed rate from one pair to the next
 * equivalent to get_cfg_3 (CORR)
 */
void EntropyV3::get_cfg_5(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_bitpair_etp = bitpair_entropy;
    sort(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    std::reverse(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    vector<uint32_t> sorted_nohash_idx_sequence_bpe_based;
    for (uint32_t i=0; i < sorted_bitpair_etp.size(); ++i) {
        if (!is_existed(sorted_bitpair_etp[i].m_segid[0], sorted_nohash_idx_sequence_bpe_based)) {
            sorted_nohash_idx_sequence_bpe_based.push_back(sorted_bitpair_etp[i].m_segid[0]);
        }
    }
    cout << "sorted_nohash_idx_sequence_bpe_based: " << vec_to_string(sorted_nohash_idx_sequence_bpe_based) << endl;

    vector<uint32_t> used_bits;
    uint32_t next_cutoff_bpe=sorted_bitpair_etp[0].m_entropy;  // start from max BPE

    for (uint32_t i=0; i < sorted_nohash_idx_sequence_bpe_based.size(); ++i) {
        bool found = false;
        uint32_t start_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],IDX_SIZE);
        uint32_t end_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],MAX_ADDRESS_BITS-1)+1;
        // cout << "next_cutoff_bpe: " << next_cutoff_bpe << endl;
        vector<BitEntropy> extracted_bpe_table;
        int32_t ETP_CUTOFF_PT = ETP_CUTOFF_STARTRATE*num_updates/100;
        // cout << "starting ETP_CUTOFF_PT: " << ETP_CUTOFF_PT << endl;

        // cout << "\nStats list of bit: " << sorted_nohash_idx_sequence_bpe_based[i] << endl;
        // for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
        //     cout << "CORR: " << correl_table[j].to_string()
        //          << ". BPE: " << bitpair_entropy[j].to_string() 
        //          << ". BPP: " << bitpair_ptn_ctr[j].to_string()
        //          << ". ETP[" << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
        //          << ". ETP[" << correl_table[j].m_segid[1] << "]: " << itv_entropy[correl_table[j].m_segid[1]].m_entropy
        //          << ". PTN[" << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
        //          << ". PTN[" << correl_table[j].m_segid[1] << "]: " << itv_ptn_entropy[correl_table[j].m_segid[1]].m_entropy
        //          << endl;
        // }
        // cout << endl;

        while (!found) {
            for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
                if ((bitpair_entropy[j].m_entropy <= next_cutoff_bpe) && !is_existed(bitpair_entropy[j].m_segid[1], used_bits)
                    && (itv_entropy[bitpair_entropy[j].m_segid[1]].m_entropy > ETP_CUTOFF_PT)) {
                    extracted_bpe_table.push_back(bitpair_entropy[j]);
                }
            }
            sort(extracted_bpe_table.begin(), extracted_bpe_table.end());

            if (extracted_bpe_table.size() > 0) {
                used_bits.push_back(extracted_bpe_table[extracted_bpe_table.size()-1].m_segid[1]);
                next_cutoff_bpe = extracted_bpe_table[extracted_bpe_table.size()-1].m_entropy * CUTOFF_PRORATE;
                found = true;
            }
            if (!found) {
                for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
                    if ((bitpair_entropy[j].m_entropy > next_cutoff_bpe) && !is_existed(bitpair_entropy[j].m_segid[1], used_bits)
                        && (itv_entropy[bitpair_entropy[j].m_segid[1]].m_entropy > ETP_CUTOFF_PT)) {
                        extracted_bpe_table.push_back(bitpair_entropy[j]);
                    }
                }
                sort(extracted_bpe_table.begin(), extracted_bpe_table.end());

                if (extracted_bpe_table.size() > 0) {
                    used_bits.push_back(extracted_bpe_table[0].m_segid[1]);
                    next_cutoff_bpe = extracted_bpe_table[0].m_entropy * CUTOFF_PRORATE;
                    found = true;
                }

                if (!found) {
                    if (ETP_CUTOFF_PT > 0)
                        ETP_CUTOFF_PT -= (((double)num_updates)*0.005);
                    else
                        ETP_CUTOFF_PT = -1;
                    cout << "extracted_bit_correl_table empty. Reduce cutoff ETP to: " << ETP_CUTOFF_PT << endl;
                }
            }
        }

        // cout << "extracted_bpe_table: " << endl;
        // for (uint32_t i=0; i < extracted_bpe_table.size(); ++i) {
        //     uint32_t index = compute_pair_table_idx(extracted_bpe_table[i].m_segid[0], extracted_bpe_table[i].m_segid[1]);
        //     cout << "BPE: " << bitpair_entropy[index].to_string() 
        //          << ". BPP: " << bitpair_ptn_ctr[index].to_string()
        //          << ". ETP["  << extracted_bpe_table[i].m_segid[0] << "]: " << itv_entropy[extracted_bpe_table[i].m_segid[0]].m_entropy
        //          << ". ETP["  << extracted_bpe_table[i].m_segid[1] << "]: " << itv_entropy[extracted_bpe_table[i].m_segid[1]].m_entropy
        //          << ". PTN["  << extracted_bpe_table[i].m_segid[0] << "]: " << itv_ptn_entropy[extracted_bpe_table[i].m_segid[0]].m_entropy
        //          << ". PTN["  << extracted_bpe_table[i].m_segid[1] << "]: " << itv_ptn_entropy[extracted_bpe_table[i].m_segid[1]].m_entropy
        //         << endl;
        // }
        // cout << "used_bits: " << vec_to_string(used_bits) << endl;
    }

    cout << "\nGETCFG5 Candidate Stats:\n";
    for (uint32_t i=0; i < used_bits.size(); ++i) {
        uint32_t index = compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],used_bits[i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
             << ". ETP["  << used_bits[i] << "]: " << itv_entropy[used_bits[i]].m_entropy
             << ". PTN["  << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
             << ". PTN["  << used_bits[i] << "]: " << itv_ptn_entropy[used_bits[i]].m_entropy
             << endl;
    }
    cout << endl;
    for (uint32_t i=0; i < c_seq_a.size(); ++i) {
        for (uint32_t j=0; j < sorted_nohash_idx_sequence_bpe_based.size(); ++j) {
            if (sorted_nohash_idx_sequence_bpe_based[j] == c_seq_a[i])
                c_seq_b.push_back(used_bits[j]);
        }
    }
    return;
}

void EntropyV3::get_cfg_6(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    c_seq_a = x_seq_a;    // one sequence must be the nohash index
    vector<uint32_t> used_bits;
    for (int32_t i=c_seq_a.size()-1; i >= 0; --i) {
        uint32_t closest_mid_ptn = itv_ptn_entropy[c_seq_a[i]].m_entropy;
        uint32_t closest_mid_idx = 0;
        for (uint32_t j=IDX_SIZE; j < itv_ptn_entropy.size(); ++j) {
            if (is_existed(itv_ptn_entropy[j].m_id, used_bits)) 
                continue;

            uint32_t ptn_distance = (itv_ptn_entropy[c_seq_a[i]].m_entropy/2 > itv_ptn_entropy[j].m_entropy)? 
                (itv_ptn_entropy[c_seq_a[i]].m_entropy/2 - itv_ptn_entropy[j].m_entropy) : 
                (itv_ptn_entropy[j].m_entropy - itv_ptn_entropy[c_seq_a[i]].m_entropy/2);
            if (ptn_distance < closest_mid_ptn) {
                closest_mid_ptn = ptn_distance;
                closest_mid_idx = itv_ptn_entropy[j].m_id;
            }
        }
        c_seq_b.push_back(closest_mid_idx);
        used_bits.push_back(closest_mid_idx);
    }

    std::reverse(c_seq_b.begin(), c_seq_b.end());

    cout << "\nGETCFG6 Candidate Stats:\n";
    for (uint32_t i=0; i < used_bits.size(); ++i) {
        uint32_t index = compute_pair_table_idx(c_seq_a[i],c_seq_b[i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << c_seq_a[i] << "]: " << itv_entropy[c_seq_a[i]].m_entropy
             << ". ETP["  << c_seq_b[i] << "]: " << itv_entropy[c_seq_b[i]].m_entropy
             << ". PTN["  << c_seq_a[i] << "]: " << itv_ptn_entropy[c_seq_a[i]].m_entropy
             << ". PTN["  << c_seq_b[i] << "]: " << itv_ptn_entropy[c_seq_b[i]].m_entropy
             << endl;
    }
    return;
}

/*
 * seq_a = nohash index sequence.
 * pair each bit in A with a bit in B in a way that the bitpair entropy (BPE) 
 * increases by a fixed rate from one pair to the next
 * the same as get_cfg_5, but start from the lowest BPE bit
 */
void EntropyV3::get_cfg_8(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    vector<BitEntropy> sorted_bitpair_etp = bitpair_entropy;
    sort(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    std::reverse(sorted_bitpair_etp.begin(), sorted_bitpair_etp.end());
    c_seq_a = x_seq_a;    // one sequence must be the nohash index

    vector<uint32_t> sorted_nohash_idx_sequence_bpe_based;
    for (uint32_t i=0; i < sorted_bitpair_etp.size(); ++i) {
        if (!is_existed(sorted_bitpair_etp[i].m_segid[0], sorted_nohash_idx_sequence_bpe_based)) {
            sorted_nohash_idx_sequence_bpe_based.push_back(sorted_bitpair_etp[i].m_segid[0]);
        }
    }
    std::reverse(sorted_nohash_idx_sequence_bpe_based.begin(), sorted_nohash_idx_sequence_bpe_based.end());
    cout << "sorted_nohash_idx_sequence_bpe_based (lowest to highest): " << vec_to_string(sorted_nohash_idx_sequence_bpe_based) << endl;

    vector<uint32_t> used_bits;
    uint32_t next_cutoff_bpe=sorted_bitpair_etp.back().m_entropy;  // start from min BPE

    for (uint32_t i=0; i < sorted_nohash_idx_sequence_bpe_based.size(); ++i) {
        bool found = false;
        uint32_t start_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],IDX_SIZE);
        uint32_t end_search_idx=compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],MAX_ADDRESS_BITS-1)+1;
        cout << "next_cutoff_bpe: " << next_cutoff_bpe << endl;
        vector<BitEntropy> extracted_bpe_table;

        cout << "\nStats list of bit: " << sorted_nohash_idx_sequence_bpe_based[i] << endl;
        for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
            cout << "CORR: " << correl_table[j].to_string()
                 << ". BPE: " << bitpair_entropy[j].to_string() 
                 << ". BPP: " << bitpair_ptn_ctr[j].to_string()
                 << ". ETP[" << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
                 << ". ETP[" << correl_table[j].m_segid[1] << "]: " << itv_entropy[correl_table[j].m_segid[1]].m_entropy
                 << ". PTN[" << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
                 << ". PTN[" << correl_table[j].m_segid[1] << "]: " << itv_ptn_entropy[correl_table[j].m_segid[1]].m_entropy
                 << endl;
        }
        cout << endl;

        for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
            if ((bitpair_entropy[j].m_entropy <= next_cutoff_bpe) && !is_existed(bitpair_entropy[j].m_segid[1], used_bits)
                && (itv_entropy[bitpair_entropy[j].m_segid[1]].m_entropy > 0)) {
                extracted_bpe_table.push_back(bitpair_entropy[j]);
            }
        }
        sort(extracted_bpe_table.begin(), extracted_bpe_table.end());

        if (extracted_bpe_table.size() > 0) {
            used_bits.push_back(extracted_bpe_table[extracted_bpe_table.size()-1].m_segid[1]);
            next_cutoff_bpe = extracted_bpe_table[extracted_bpe_table.size()-1].m_entropy * CUTOFF_PRORATE;
            found = true;
        }
        if (!found) {
            for (uint32_t j=start_search_idx; j < end_search_idx; ++j) {
                if ((bitpair_entropy[j].m_entropy > next_cutoff_bpe) && !is_existed(bitpair_entropy[j].m_segid[1], used_bits)
                    && (itv_entropy[bitpair_entropy[j].m_segid[1]].m_entropy > 0)) {
                    extracted_bpe_table.push_back(bitpair_entropy[j]);
                }
            }
            sort(extracted_bpe_table.begin(), extracted_bpe_table.end());

            if (extracted_bpe_table.size() > 0) {
                used_bits.push_back(extracted_bpe_table[0].m_segid[1]);
                next_cutoff_bpe = extracted_bpe_table[0].m_entropy * CUTOFF_PRORATE;
                found = true;
            }

            if (!found) {
                assert(false);
            }
        }

        cout << "extracted_bpe_table: " << endl;
        for (uint32_t i=0; i < extracted_bpe_table.size(); ++i) {
            uint32_t index = compute_pair_table_idx(extracted_bpe_table[i].m_segid[0], extracted_bpe_table[i].m_segid[1]);
            cout << "BPE: " << bitpair_entropy[index].to_string() 
                 << ". BPP: " << bitpair_ptn_ctr[index].to_string()
                 << ". ETP["  << extracted_bpe_table[i].m_segid[0] << "]: " << itv_entropy[extracted_bpe_table[i].m_segid[0]].m_entropy
                 << ". ETP["  << extracted_bpe_table[i].m_segid[1] << "]: " << itv_entropy[extracted_bpe_table[i].m_segid[1]].m_entropy
                 << ". PTN["  << extracted_bpe_table[i].m_segid[0] << "]: " << itv_ptn_entropy[extracted_bpe_table[i].m_segid[0]].m_entropy
                 << ". PTN["  << extracted_bpe_table[i].m_segid[1] << "]: " << itv_ptn_entropy[extracted_bpe_table[i].m_segid[1]].m_entropy
                 << endl;
        }
        cout << "used_bits: " << vec_to_string(used_bits) << endl;
    }

    cout << "\nGETCFG8 Candidate Stats:\n";
    for (uint32_t i=0; i < used_bits.size(); ++i) {
        uint32_t index = compute_pair_table_idx(sorted_nohash_idx_sequence_bpe_based[i],used_bits[i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
             << ". ETP["  << used_bits[i] << "]: " << itv_entropy[used_bits[i]].m_entropy
             << ". PTN["  << sorted_nohash_idx_sequence_bpe_based[i] << "]: " << itv_ptn_entropy[sorted_nohash_idx_sequence_bpe_based[i]].m_entropy
             << ". PTN["  << used_bits[i] << "]: " << itv_ptn_entropy[used_bits[i]].m_entropy
             << endl;
    }
    cout << endl;
    for (uint32_t i=0; i < c_seq_a.size(); ++i) {
        for (uint32_t j=0; j < sorted_nohash_idx_sequence_bpe_based.size(); ++j) {
            if (sorted_nohash_idx_sequence_bpe_based[j] == c_seq_a[i])
                c_seq_b.push_back(used_bits[j]);
        }
    }
    return;
}



vector<uint32_t> EntropyV3::read_dna_config(string app_name) {
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
vector<uint32_t> EntropyV3::parse_config(string input_line) {
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
void EntropyV3::get_cfg_genetic(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
    vector<uint32_t>& c_seq_a, vector<uint32_t>& c_seq_b) {
    c_seq_a = m_genetic_cfg_a;    // one sequence must be the nohash index
    c_seq_b = m_genetic_cfg_b;

    cout << "\nGENETIC Candidate Stats:\n";
    for (uint32_t i=0; i < c_seq_a.size(); ++i) {
        uint32_t index = compute_pair_table_idx(c_seq_a[i],c_seq_b[i]);
        cout << "CORR: "  << correl_table[index].to_string()
             << ". BPE: " << bitpair_entropy[index].to_string() 
             << ". BPP: " << bitpair_ptn_ctr[index].to_string()
             << ". ETP["  << c_seq_a[i] << "]: " << itv_entropy[c_seq_a[i]].m_entropy
             << ". ETP["  << c_seq_b[i] << "]: " << itv_entropy[c_seq_b[i]].m_entropy
             << ". PTN["  << c_seq_a[i] << "]: " << itv_ptn_entropy[c_seq_a[i]].m_entropy
             << ". PTN["  << c_seq_b[i] << "]: " << itv_ptn_entropy[c_seq_b[i]].m_entropy
             << endl;
    }
    return;
}