#include "belady.h"
#include <fstream>
#include <sstream>

void OptRepl::initialize(uint32_t cpu, string trace_file) {
    // open trace file
    ifstream file(trace_file);

    if (file) {
        while(!file.eof()) {
            string line;
            uint64_t address;
            string type;
            getline(file, line);
            stringstream ss(line);
            ss >> address;
            ss >> type;

            // get accessing set
            uint32_t set_index = address & ((1 << lg2(LLC_SET)) - 1);

            // add new access entry to the access history
            set_history[cpu][set_index].push_back(address);
        }
        cout << "Belady Initialization Completed" << endl;
    }

    else {
        cout << "Belady Trace File Not Found: " << trace_file << endl;
        assert(0);
    }
}

void OptRepl::update(uint32_t cpu, uint32_t set, uint32_t way, uint64_t full_addr, uint64_t ip, uint64_t victim_addr, uint32_t type, uint8_t hit) {
    return;
}

uint32_t OptRepl::get_victim(uint32_t cpu, uint64_t instr_id, uint32_t set, const BLOCK *current_set, uint64_t ip, uint64_t full_addr, uint32_t type) {
    // fill invalid line first
    for (uint32_t way=0; way < LLC_WAY; ++way) {
        if (current_set[way].valid == false)
            return way;
    }

    vector<int> candidates;
    for (uint32_t way=0; way < LLC_WAY; ++way) {
        candidates.push_back(way);
    }

    /* 
        Compare reuse distances of all ways in set
        - this implementation assumes no bypassing
        - the last remaining candidate is the final victim
        - more than one candidates => dead blocks (never re-accessed) => randomly select one among them
    */
    for (uint64_t entry=exec_tracker[set]; entry < set_history[cpu][set].size(); ++entry) {
        if (candidates.size() == 1) {
            // the last remaining candidate has the longest reuse distance
            break;
        }

        for (uint32_t way=0; way < candidates.size(); ++way) {
            if (current_set[candidates[way]].address == set_history[cpu][set][entry]) {
                candidates.erase(candidates.begin()+way);
                break;
            }
        }
    }

    exec_tracker[set]++;    // update execution progress tracker
    return candidates[rand() % candidates.size()];
}
