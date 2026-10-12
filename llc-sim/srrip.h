#ifndef SRRIP_H
#define SRRIP_H

#include "replacement.h"
#define maxRRPV 3

class SRRIP : public Replacement {
public:
    SRRIP() {}
    ~SRRIP() {}
    void initialize(vector<Access>& history, Index& indexing);
    uint32_t get_victim(uint64_t address, uint32_t set_index, uint64_t *victim_set);
    void update(uint64_t address, uint32_t set_index, uint32_t way_index, bool is_hit);
    void save_state(ofstream &file);
    void load_state(ifstream &file);
    void reset();


public:
    uint32_t m_rrpv[LLC_SET][LLC_WAY];
};



#endif  // SRRIP_H