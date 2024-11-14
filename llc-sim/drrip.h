#ifndef DRRIP_H
#define DRRIP_H

#include "replacement.h"

//========================================================================================================================//
// DRRIP POLICY
//========================================================================================================================//
#define maxRRPV 3
#define NUM_POLICY 2
#define SDM_SIZE 32
#define TOTAL_SDM_SETS NUM_CORES*NUM_POLICY*SDM_SIZE
#define BIP_MAX 32
#define PSEL_WIDTH 10
#define PSEL_MAX ((1<<PSEL_WIDTH)-1)
#define PSEL_THRS PSEL_MAX/2



class DRRIP : public Replacement {
public:
    DRRIP() {}
    ~DRRIP() {}
    void initialize(vector<Access>& history, Index& indexing);
    uint32_t get_victim(uint64_t address, uint32_t set_index, uint64_t *victim_set);
    void update(uint64_t address, uint32_t set_index, uint32_t way_index, bool is_hit);
    void save_state(ofstream &file);
    void load_state(ifstream &file);
    void reset();

    int is_it_leader(uint32_t cpu, uint32_t set);

public:
    uint32_t m_rrpv[LLC_SET][LLC_WAY];
    uint32_t bip_counter;
    uint32_t PSEL[NUM_CORES];
    uint32_t rand_sets[TOTAL_SDM_SETS];
};

#endif // DRRIP_H