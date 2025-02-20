#ifndef FREQUENCY_INDEX_H
#define FREQUENCY_INDEX_H

#include "index.h"

class FrequencyIndex : public Index {
public:
    vector<Record> profile_table;
    uint64_t SET_ACCESSES[LLC_SET] = {0};

    FrequencyIndex() {};
    ~FrequencyIndex() {};
    void initialize(vector<Access>& history);
    uint32_t get_set(uint64_t address);
    bool update(uint64_t address) { return false; }
};

class PatternIndex : public Index {
public:
    vector<Record> profile_table;
    uint64_t SET_ACCESSES[LLC_SET] = {0};

    PatternIndex() {};
    ~PatternIndex() {};
    // static void     map_index(vector<Record>& x_table);
    // static uint32_t compute_overlap_score(vector<Record>& x_table, uint32_t mapping_addr_idx, vector<uint32_t>& mapped_addr_idx);

    void initialize(vector<Access>& history);
    uint32_t get_set(uint64_t address);
    bool update(uint64_t address) { return false; }
    
};

#endif // FREQUENCY_INDEX_H