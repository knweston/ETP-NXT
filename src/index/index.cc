#include "index/index.h"

string flatten(double* array, uint64_t end_pt, uint64_t start_pt) {
    string data = "";
    for (uint32_t i=start_pt; i < end_pt; ++i)
        data += to_string(array[i]) + "_";

    data.pop_back();
    return data;
}

void override_block(BLOCK *src, BLOCK *des) {
    des->valid      = src->valid;
    des->prefetch   = src->prefetch;
    des->dirty      = src->dirty;
    des->used       = src->used;
    des->delta      = src->delta;
    des->depth      = src->depth;
    des->signature  = src->signature; 
    des->confidence = src->confidence;
    des->address    = src->address;
    des->full_addr  = src->full_addr;
    des->tag        = src->tag;
    des->data       = src->data;
    des->ip         = src->ip;
    des->cpu        = src->cpu;
    des->instr_id   = src->instr_id; 
}

bool force_writeback(BLOCK* block, MEMORY *lower_level, uint32_t fill_lvl, uint32_t cpu_id) {
    // if the lower level memory is full
    if (lower_level->get_occupancy(2, block->address) == lower_level->get_size(2, block->address)) {
        // lower level WQ is full, cannot replace this victim
        lower_level->increment_WQ_FULL(block->address);
        assert(false);
    }
    else {
        PACKET writeback_packet;
        writeback_packet.fill_level = fill_lvl << 1;
        writeback_packet.cpu = cpu_id;
        writeback_packet.address = block->address;
        writeback_packet.full_addr = block->full_addr;
        writeback_packet.data = block->data;
        writeback_packet.instr_id = block->instr_id;     // FIXME: evicted or evicting block->instr_id ?
        writeback_packet.ip = 0; // writeback does not have ip
        writeback_packet.type = WRITEBACK;
        writeback_packet.event_cycle = current_core_cycle[cpu_id];
        lower_level->add_wq(&writeback_packet);
    }
    return true;
}

vector<uint64_t> get_significant_indices(uint64_t* array, uint32_t array_size, double mean, uint32_t num_select) {
    vector<Pair> pairs;
    for (uint32_t i=0; i < array_size; ++i) {
        uint64_t difference = abs(int(array[i] - mean));
        pairs.push_back(Pair(i, difference));
    }

    sort(pairs.begin(), pairs.end(), Pair::cmp_val_b);

    vector<uint64_t> top;
    for (uint32_t i=0; i < num_select; ++i)
        top.push_back(pairs[i].val_a);

    sort(top.begin(), top.end());
    return top;
}

void Index::update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) {
    uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
    N_ACCESS[unhashed_index]++;
    PI_N_ACCESS[unhashed_index]++;
    if (!is_hit) {
        MISS[unhashed_index]++;
        PI_MISS[unhashed_index]++;
    }
    access_ctr++;
    if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
        cout << endl << endl;
        cout << "========================================================================" << endl;
        cout << "End of interval # " << num_interval << " Summary" << endl;
        cout << "========================================================================" << endl;
        cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
        cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
        cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
        cout << "========================================================================" << endl;
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
        access_ctr = 0;
        num_interval++;
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
            access_ctr = 0;
            idx_warmup_complete = true;   
        }
    }
}