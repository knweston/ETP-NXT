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

    void initialize(vector<Access>& history);
    uint32_t get_set(uint64_t address);
    bool update(uint64_t address) { return false; }
    
};

#endif // FREQUENCY_INDEX_H