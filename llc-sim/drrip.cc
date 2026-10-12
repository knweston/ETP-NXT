#include "drrip.h"

void DRRIP::initialize(vector<Access>& history, Index& indexing) {
    for(int i=0; i<LLC_SET; i++) {
        for(int j=0; j<LLC_WAY; j++)
            m_rrpv[i][j] = maxRRPV;
    }

    bip_counter = 0;

    // randomly selected sampler sets
    unsigned long rand_seed = 1;
    unsigned long max_rand = 1048576;
    uint32_t my_set = LLC_SET;
    int do_again = 0;
    for (int i=0; i<TOTAL_SDM_SETS; i++) {
        do {
            do_again = 0;
            rand_seed = rand_seed * 1103515245 + 12345;
            rand_sets[i] = ((unsigned) ((rand_seed/65536) % max_rand)) % my_set;
            // printf("Assign rand_sets[%d]: %u  LLC: %u\n", i, rand_sets[i], my_set);
            for (int j=0; j<i; j++) {
                if (rand_sets[i] == rand_sets[j]) {
                    do_again = 1;
                    break;
                }
            }
        } while (do_again);
        // printf("rand_sets[%d]: %d\n", i, rand_sets[i]);
    }

    for (int i=0; i<NUM_CORES; i++)
        PSEL[i] = 0;
}

uint32_t DRRIP::get_victim(uint64_t address, uint32_t set_index, uint64_t *victim_set) {
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

void DRRIP::update(uint64_t address, uint32_t set_index, uint32_t way_index, bool is_hit) {
    uint32_t cpu = 0;   // fixed cpu_id since we are simulating single core only

	// cache hit
	if (is_hit) { 
		m_rrpv[set_index][way_index] = 0; // for cache hit, DRRIP always promotes a cache line to the MRU position
		return;
	}

	// cache miss
    int leader = is_it_leader(cpu, set_index);

    if (leader == -1) { // follower sets
        if (PSEL[cpu] > PSEL_THRS) { // follow BIP
            m_rrpv[set_index][way_index] = maxRRPV;

            bip_counter++;
            if (bip_counter == BIP_MAX)
                bip_counter = 0;
            if (bip_counter == 0)
                m_rrpv[set_index][way_index] = maxRRPV-1;
        } else // follow SRRIP
            m_rrpv[set_index][way_index] = maxRRPV-1;

    } else if (leader == 0) { // leader 0: BIP
        if (PSEL[cpu] > 0) PSEL[cpu]--;
        m_rrpv[set_index][way_index] = maxRRPV;

        bip_counter++;
        if (bip_counter == BIP_MAX) bip_counter = 0;
        if (bip_counter == 0) m_rrpv[set_index][way_index] = maxRRPV-1;

	} else if (leader == 1) { // leader 1: SRRIP 
        if (PSEL[cpu] < PSEL_MAX) PSEL[cpu]++;
        m_rrpv[set_index][way_index] = maxRRPV-1;

    } else // WE SHOULD NOT REACH HERE
        assert(0);
}

void DRRIP::save_state(ofstream &file) {
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way; way < LLC_WAY; ++way)
            file << m_rrpv[set][way] << endl;
    }
}

void DRRIP::load_state(ifstream &file) {
    string line;
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way; way < LLC_WAY; ++way) {
            getline(file, line);
            m_rrpv[set][way] = stoi(line);
        }
    }
}

void DRRIP::reset() {
    for(int i=0; i<LLC_SET; i++) {
        for(int j=0; j<LLC_WAY; j++)
            m_rrpv[i][j] = maxRRPV;
    }
}

int DRRIP::is_it_leader(uint32_t cpu, uint32_t set)
{
    uint32_t start = cpu * NUM_POLICY * SDM_SIZE,
             end = start + NUM_POLICY * SDM_SIZE;

    for (uint32_t i=start; i<end; i++)
        if (rand_sets[i] == set)
            return ((i - start) / SDM_SIZE);

    return -1;
}