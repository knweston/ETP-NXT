#ifndef SMART_INDEX_H
#define SMART_INDEX_H

#include "binarynet.h"
#include "table.h"
#include <cmath>

// Execution modes
#define NORMAL  0
#define PROFILE 1
#define EXPLOIT 2

struct Access {
    uint64_t address;
    uint32_t type;
    uint64_t ip;
    Access(uint64_t _addr, uint32_t _type, uint64_t _ip)
        :address(_addr), type(_type), ip(_ip) {}
};

class SmartIndex {
public:
    SmartIndex(uint32_t _sets, uint32_t _ways)
        :num_sets(_sets), num_ways(_ways) {
        profiling_table = new Table();
        EXEC_MODE = NORMAL;

        // Initialize access and replacement counters
        num_repl        = new uint64_t[num_sets];
        num_accesses    = new uint64_t[num_sets];
        num_unseen_addr = new uint64_t[num_sets];
        for (uint32_t i=0; i < num_sets; ++i) {
            num_repl[i]        = 0;
            num_accesses[i]    = 0;
            num_unseen_addr[i] = 0;
            accessed_addr.push_back(vector<uint64_t>());
        }
    }
    
    ~SmartIndex() {
        delete model;
        delete profiling_table;
        delete[] num_accesses;
        delete[] num_repl;
        delete[] num_unseen_addr;
    }
    void set_trace_file(string filename) { trace_name = filename; }
    uint32_t get_set(uint64_t address);

    void create_model(vector<uint32_t> &_model_arch);

    void profile_addr(uint64_t address);
    void add_to_trace(uint64_t address, uint32_t type, uint64_t ip);
    void export_stats();
    void export_trace();
    bool import_stats();
    void set_exec_mode(uint32_t mode) { EXEC_MODE = mode; }
    void print_exec_mode();
    void print_final_stats();

    static SmartIndex* get_instance(uint32_t num_set, uint32_t num_way);
    static SmartIndex* instance; 

private:
    uint32_t EXEC_MODE;

    uint32_t num_sets;
    uint32_t num_ways;
    string trace_name;
    BinaryNet *model;
    Table *profiling_table;

    // Access trace for Belady Algorithm
    // vector<uint64_t> access_trace;
    vector<Access> access_trace;

    // Distinct accessed addresses, one list per set
    vector<vector<uint64_t>> accessed_addr;

    // Access and replacement counters, one per set
    uint64_t *num_accesses;
    uint64_t *num_repl;

    // Counters for unexpected addresses encountered in runtime
    uint64_t *num_unseen_addr = 0;

    friend class CACHE;
};

#endif // SMART_INDEX_H