#include "index/etpv5.h"

void EntropyV5::move_cache_data(MEMORY *lower_level) {
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

bool EntropyV5::remap_tie_break(BLOCK* src, BLOCK* dest, MEMORY *lower_level) {
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

bool EntropyV5::should_update(bool is_hit, uint32_t type) {
    if (is_hit)
        return false;
    if (V5_SKIP_UPDATE_PF && type == PREFETCH)
        return false;
    if (V5_SKIP_UPDATE_WB && type == WRITEBACK)
        return false;
    return true;
}

uint32_t EntropyV5::replacement_getvictim(uint32_t index) {
    if (V5_REPLACEMENT_POLICY == 0)
        return get_lru_victim(index);
    else if (V5_REPLACEMENT_POLICY == 1)
        return get_rrip_victim(index);
    else if (V5_REPLACEMENT_POLICY == 2)
        return get_rlr_victim(index);
    else
        assert(false);
}

void EntropyV5::replacement_update(uint32_t index, uint32_t way, uint32_t remapping_index, uint32_t remapping_way) {
    if (V5_REPLACEMENT_POLICY == 0) {
        lru_update(index, way, block_array[remapping_index][remapping_way].lru);
    }
    else if (V5_REPLACEMENT_POLICY == 1) {
        rrip_update(index, way, rrpv[remapping_index][remapping_way]);
    }
    else if (V5_REPLACEMENT_POLICY == 2) {
        rlr_update(index, way, remapping_index, remapping_way);
    }
    else {
        assert(false);
    }
}

uint32_t EntropyV5::compute_pair_table_idx(uint32_t bit_a, uint32_t bit_b) {
    if (bit_a > bit_b) {
        return (bit_b*(MAX_ADDRESS_BITS-IDX_SIZE) + (bit_a-IDX_SIZE));
    }
    else {
        return (bit_a*(MAX_ADDRESS_BITS-IDX_SIZE) + (bit_b-IDX_SIZE));
    }
}

void EntropyV5::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type, uint64_t eviction_address) {
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

    if (should_update(is_hit, type) && eviction_address != NO_EVICTION_ADDRESS &&
        (!V5_SKIP_TRAINING_DURING_REMAP || !m_should_remap)) {
        eviction_pair_update(address, eviction_address);
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
        cout << "   entropy_stdev:  " << std::fixed << compute_std_dev(PI_H_ACCESS, NUM_SET) << endl;
        cout << "   eviction_count: " << std::fixed << compute_sum(PI_EVICTION, NUM_SET) << "(" 
             << compute_sum(PI_EVICTION, NUM_SET)*100/INTERVAL_SIZE 
             << "%)" << endl;
        cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
        cout << "   num_reset_itv:         " << num_reset_itv << endl;
        cout << "   num_reset_bitpair_etp: " << num_reset_bitpair_etp << endl;
        cout << "   num_total_updates:     " << num_updates << endl;
        cout << "   num_eviction_pairs:    " << num_eviction_pairs << endl;
        cout << "========================================================================" << endl;

        // Do not replace a mapping that is still being migrated.
        if (m_should_remap) {
            cout << "Defer candidate selection until remapping completes." << endl;
        }
        else {
            m_last_seq_a = m_curr_seq_a;
            m_last_seq_b = m_curr_seq_b;
            compute_new_config(m_curr_seq_a, m_curr_seq_b);
            m_should_remap = check_remap();
            map_ptr = 0;
        }

        cout << "m_last_seq_a: " << vec_to_string(m_last_seq_a) << endl;
        cout << "m_last_seq_b: " << vec_to_string(m_last_seq_b) << endl;
        cout << "m_curr_seq_a: " << vec_to_string(m_curr_seq_a) << endl;
        cout << "m_curr_seq_b: " << vec_to_string(m_curr_seq_b) << endl;
        cout << "should_remap: " << (m_should_remap ? "Yes" : "No") << endl << endl;

        memcpy(PAST_PI_EVICTION, PI_EVICTION, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_H_ACCESS, 0, NUM_SET * sizeof(*PI_H_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

        num_interval++;
        access_ctr = 0;
        num_updates= 0;
        reset_eviction_pair_counters();
    }
    warmup_check();
}


void EntropyV5::eviction_pair_update(uint64_t address, uint64_t eviction_address) {
    // Scale all related counters and their sample weight together.
    const uint32_t counter_limit = V5_ETP_CTR_MAX;
    if (num_updates >= counter_limit) {
        for (auto &counter : itv_entropy)
            counter.m_entropy /= 2;
        for (auto &counter : bitpair_entropy)
            counter.m_entropy /= 2;
        num_updates /= 2;
        num_reset_itv++;
        num_reset_bitpair_etp++;
    }

    const uint64_t difference = address ^ eviction_address;
    for (uint32_t i = 0; i < itv_entropy.size(); ++i) {
        if ((difference >> i) & 1ULL)
            itv_entropy[i].m_entropy++;
    }
    for (uint32_t k = 0; k < bitpair_entropy.size(); ++k) {
        const uint32_t i = bitpair_entropy[k].m_segid[0];
        const uint32_t j = bitpair_entropy[k].m_segid[1];
        const bool separates = ((difference >> i) ^ (difference >> j)) & 1ULL;
        if (separates)
            bitpair_entropy[k].m_entropy++;
    }
    num_updates++;
    num_eviction_pairs++;
}

void EntropyV5::reset_eviction_pair_counters() {
    // Each window describes collisions generated by its current mapping.
    for (auto &counter : itv_entropy)
        counter.m_entropy = 0;
    for (auto &counter : bitpair_entropy)
        counter.m_entropy = 0;
    num_updates = 0;
    num_eviction_pairs = 0;
}

void EntropyV5::compute_new_config(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b) {
    if (num_eviction_pairs == 0) {
        cout << "No eviction-pair samples; keep current candidate." << endl;
        return;
    }
    vector<uint32_t> candidate_a, candidate_b;
    get_cfg_5(x_seq_a, x_seq_b, candidate_a, candidate_b);
    uint64_t separation_score = 0;
    for (uint32_t i = 0; i < candidate_a.size(); ++i) {
        const uint32_t index = compute_pair_table_idx(candidate_a[i], candidate_b[i]);
        separation_score += bitpair_entropy[index].m_entropy;
    }
    cout << "Candidate separation score: " << separation_score << endl;
    cout << "Eviction-pair samples:      " << num_eviction_pairs << endl;
    // Relative improvement over the current mapping can divide by zero.
    if (separation_score > 0) {
        x_seq_a = candidate_a;
        x_seq_b = candidate_b;
        cout << "Use candidate selected from eviction-pair separation counters." << endl;
    }
    else {
        cout << "No separation benefit; keep current candidate." << endl;
    }
}

void EntropyV5::get_cfg_5(vector<uint32_t>& x_seq_a, vector<uint32_t>& x_seq_b, 
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
        vector<BitEntropy> extracted_bpe_table;
        int32_t ETP_CUTOFF_PT = V5_ETP_CUTOFF_STARTRATE*num_updates/100;


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
                next_cutoff_bpe = extracted_bpe_table[extracted_bpe_table.size()-1].m_entropy * V5_CUTOFF_PRORATE;
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
                    next_cutoff_bpe = extracted_bpe_table[0].m_entropy * V5_CUTOFF_PRORATE;
                    found = true;
                }

                if (!found) {
                    if (ETP_CUTOFF_PT > 0)
                        ETP_CUTOFF_PT -= (((double)num_updates)*0.005);
                    else
                        ETP_CUTOFF_PT = -1;
                    cout << "No eligible pair. Reduce cutoff to: " << ETP_CUTOFF_PT << endl;
                }
            }
        }

    }

    for (uint32_t i=0; i < c_seq_a.size(); ++i) {
        for (uint32_t j=0; j < sorted_nohash_idx_sequence_bpe_based.size(); ++j) {
            if (sorted_nohash_idx_sequence_bpe_based[j] == c_seq_a[i])
                c_seq_b.push_back(used_bits[j]);
        }
    }
    return;
}

