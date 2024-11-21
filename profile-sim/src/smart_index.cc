#include "smart_index.h"
#include <fstream>
#include <algorithm>

SmartIndex* SmartIndex::instance; 
SmartIndex* SmartIndex::get_instance(uint32_t num_set, uint32_t num_way) {
    if (instance)
        return instance;
    instance = new SmartIndex(num_set, num_way);
    return instance;
}

uint32_t SmartIndex::get_set(uint64_t address) {
    if (EXEC_MODE == EXPLOIT) {
        long long index = profiling_table->find_addr(address);
        if (index == -1) {
            uint32_t set_index = address & ((1 << lg2(num_sets)) - 1);
            num_unseen_addr[set_index]++;
            return set_index;
        }
        else {
            uint32_t set_index = profiling_table->get_record(index).get_setid();
            return set_index;
        }
    }
    else {
        uint32_t set_index = address & ((1 << lg2(num_sets)) - 1);
        return set_index;
    }
}

void SmartIndex::create_model(vector<uint32_t> &_model_arch) {
    this->model = new BinaryNet(_model_arch);
}

void SmartIndex::profile_addr(uint64_t address) {

    // Find the address in the frequency list
    long long index = profiling_table->find_addr(address);

    // If found, increment the frequency
    if (index != -1) {
        profiling_table->increment_freq(index);
    }
    // Otherwise, save the address
    else {
        profiling_table->add_record(address);
        if (profiling_table->num_records() % 1000 == 0)
            cout << "Total profiled addresses = " << profiling_table->num_records() << endl;
    }

    // Check the set bits 
    uint32_t set_index = address & ((1 << lg2(num_sets)) - 1);

    // If this is the first access, save the address
    bool found = binary_search(accessed_addr[set_index].begin(), accessed_addr[set_index].end(), address);
    if (!found) {
        accessed_addr[set_index].push_back(address);
        sort(accessed_addr[set_index].begin(), accessed_addr[set_index].end());
    }
}

void SmartIndex::add_to_trace(uint64_t address, uint32_t type, uint64_t ip) {
    // add address to belady trace
    access_trace.push_back(Access(address, type, ip));
}

void SmartIndex::export_stats() {
    if (EXEC_MODE == PROFILE)
        profiling_table->export_table(trace_name+".txt");
}

void SmartIndex::export_trace() {
    if (EXEC_MODE == PROFILE) {
        string filename = trace_name+"-LLC-TRACE.txt";
        ofstream file(filename);
        for (uint64_t i=0; i < access_trace.size(); ++i) {
            file << std::hex << std::setw(12) << std::setfill('0') << access_trace[i].address;
            file << " " << access_trace[i].ip;
            if (access_trace[i].type == 0) 
                file << " LOAD" << endl;
            else if (access_trace[i].type == 1) 
                file << " RFO" << endl;
            else if (access_trace[i].type == 3) 
                file << " WRITEBACK" << endl;
            else
                file << " PREFETCH" << endl;
        }
        file.close();
        cout << "Access history exported to file: " << filename << endl;
    }
}

bool SmartIndex::import_stats() {
    bool import_success = profiling_table->import_table(trace_name+".txt");
    if (EXEC_MODE == EXPLOIT) {
        profiling_table->assign_set_id(num_sets);
    }
    return import_success;
}

void SmartIndex::print_exec_mode() {
    cout << "\nEXECUTION MODE = ";
    switch(EXEC_MODE) {
        case 0:
            cout << "NORMAL" << endl;
            break;
        case 1:
            cout << "PROFILE" << endl;
            break;
        case 2:
            cout << "EXPLOIT" << endl;
            break;
        default:
            cout << "UNKNOWN" << endl;
    }
    
}

void SmartIndex::print_final_stats() {
    uint64_t total_accesses    = 0;
    uint64_t total_evictions   = 0;
    uint64_t total_unseen_addr = 0;
    cout << "========================================================================" << endl;
    cout << "Total Accesses, Evictions and Distinct Addresses Per Set" << endl;
    cout << "========================================================================" << endl;
    cout << left << setw(7) << "Set" << setw(10) << "Accesses" << setw(15) << "Evictions" << setw(15) << "Unseen Addr" << endl;
    for (uint32_t i=0; i < num_sets; ++i) {
        cout << left << setw(7) << i << setw(10) << num_accesses[i] << setw(15) << num_repl[i] << setw(15) << num_unseen_addr[i] << endl;
        total_accesses    += num_accesses[i];
        total_evictions   += num_repl[i];
        total_unseen_addr += num_unseen_addr[i];
    }
    cout << "========================================================================" << endl;
    cout << left << setw(7) << "total" << setw(10) << total_accesses << setw(15) << total_evictions << setw(15) << total_unseen_addr << endl;
    cout << "========================================================================" << endl;
}