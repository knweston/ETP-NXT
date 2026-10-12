#include "srrip.h"

void SRRIP::initialize(vector<Access>& history, Index& indexing) {
    cout << "Initialize SRRIP state" << endl;
    for(int i=0; i<LLC_SET; i++) {
        for(int j=0; j<LLC_WAY; j++)
            m_rrpv[i][j] = maxRRPV;
    }
}

uint32_t SRRIP::get_victim(uint64_t address, uint32_t set_index, uint64_t *victim_set) {
    // find invalid lines first
    for (int i=0; i<LLC_WAY; ++i) {
        if (victim_set[i] == 0)
            return i;
    }
    // look for the maxRRPV line
    while (1) {
        for (int i=0; i<LLC_WAY; i++)
            if (m_rrpv[set_index][i] == maxRRPV) {
                return i;
            }

        for (int i=0; i<LLC_WAY; i++)
            m_rrpv[set_index][i]++;
    }
}

void SRRIP::update(uint64_t address, uint32_t set_index, uint32_t way_index, bool is_hit) {
    if (is_hit)
        m_rrpv[set_index][way_index] = 0;
    else
        m_rrpv[set_index][way_index] = maxRRPV-1;
}

void SRRIP::save_state(ofstream &file) {
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way; way < LLC_WAY; ++way)
            file << m_rrpv[set][way] << endl;
    }
}

void SRRIP::load_state(ifstream &file) {
    string line;
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way; way < LLC_WAY; ++way) {
            getline(file, line);
            m_rrpv[set][way] = stoi(line);
        }
    }
}

void SRRIP::reset() {
    for(int i=0; i<LLC_SET; i++) {
        for(int j=0; j<LLC_WAY; j++)
            m_rrpv[i][j] = maxRRPV;
    }
}