#include "index.h"

//========================================================================================================================//
// CLASS INDEX
//========================================================================================================================//
void Index::profile(vector<Access>& history, bool keep_cycle_info, bool export_mapping) {
    cout << "Cycle Information: " << (keep_cycle_info? "Yes" : "No") << endl;
    cout << "Index Mapping:     " << (export_mapping? "Yes" : "No") << endl;

    vector<Record> unsorted_table;
    vector<Record> sorted_table;

    // get distinct addresses, sort them and count access frequency
    uint32_t num_profile_accesses = history.size();
    if (MAX_INST > 0)
        num_profile_accesses = MAX_INST;

    for (uint64_t i=0; i < num_profile_accesses; ++i) {
        // print profiling progress
        if (PRINT_INTERVAL && (i % PRINT_INTERVAL == 0))
            cout << "  Profiled Accesses = " << i << endl;

        // find in sorted table
        int64_t addr_index = -1;
        addr_index = find_addr(history[i].address, sorted_table);

        if (addr_index >= 0) {  // found
            sorted_table[addr_index].frequency++;
            if (keep_cycle_info) {
                sorted_table[addr_index].accessed_cycles.push_back(history[i].cycle);
            }
            continue;
        }
        
        else {  // if not found in sorted table, check in unsorted table
            for (uint64_t entry=0; entry < unsorted_table.size(); ++entry) {
                if (history[i].address == unsorted_table[entry].address) {
                    unsorted_table[entry].frequency++;
                    if (keep_cycle_info) {
                        unsorted_table[entry].accessed_cycles.push_back(history[i].cycle);
                    }
                    addr_index = entry;
                    break;
                }
            }

            if (addr_index < 0) {
                Record new_record;
                new_record.address = history[i].address;
                if (keep_cycle_info) {
                    new_record.accessed_cycles.push_back(history[i].cycle);
                }
                unsorted_table.push_back(new_record);
            }
        }

        // merge unsorted table to main profile table 
        if (unsorted_table.size() >= 3000) {
            for (uint64_t j=0; j < unsorted_table.size(); ++j) {
                sorted_table.push_back(unsorted_table[j]);
            }

            // sort the merged profile table
            sort(sorted_table.begin(), sorted_table.end());   
            
            // empty unsorted table
            unsorted_table = {}; 
        }
    }

    // merge any leftover records
    if (unsorted_table.size()) {
        for (uint64_t j=0; j < unsorted_table.size(); ++j) {
            sorted_table.push_back(unsorted_table[j]);
        }
        // sort the merged profile table
        sort(sorted_table.begin(), sorted_table.end());
        unsorted_table = {};
    }

    // sort profiled records based on frequency
    sort(sorted_table.begin(), sorted_table.end(), Record::compare_frequency);

    // profile mapping
    if (export_mapping) {
        cout << "Starting the mapping process..." << endl;
        map_index(sorted_table);
    }

    // export to file
    string filename;
    for (uint32_t i=0; i < TRACE_FILE.size()-13; ++i)
        filename += TRACE_FILE[i];
    filename += "FBI-TRACE.txt";

    ofstream export_file(filename);
    for (uint64_t i=0; i < sorted_table.size(); ++i) {
        export_file << std::hex << std::setw(12) << std::setfill('0') << sorted_table[i].address << " " << std::dec << sorted_table[i].frequency;
        
        if (export_mapping) {
            export_file << " " << sorted_table[i].set_id;
        }
        
        export_file << endl;

        if (keep_cycle_info) {
            for (uint32_t t=0; t < sorted_table[i].accessed_cycles.size(); ++t) {
                export_file << "   " << sorted_table[i].accessed_cycles[t] << endl;
            }
        }
    }
    export_file.close();
    cout << "Profile data exported to file: " << filename << endl;
}


int64_t Index::find_addr(uint64_t address, vector<Record>& profile_table) {
    if (!profile_table.empty()) {
        Record new_record;
        new_record.address = address;
        return search(profile_table, 0, profile_table.size()-1, new_record);
    }
    else {
        return -1;
    }
}

uint32_t Index::compute_overlap_score(vector<Record>& x_table, uint32_t mapping_addr_idx, vector<uint32_t>& mapped_addr_idx) {
    uint32_t total_overlap_score = 0;
    // cout << "mapping_addr: " << x_table[mapping_addr_idx].address << endl;
    // cout << "mapping_addr_life_events: " << vec_to_string(x_table[mapping_addr_idx].accessed_cycles) << endl;

    for (uint32_t i=0; i < mapped_addr_idx.size(); ++i) {
        vector<uint64_t> mapping_addr_life_events = x_table[mapping_addr_idx].accessed_cycles;
        vector<uint64_t> mapped_addr_life_events  = x_table[mapped_addr_idx[i]].accessed_cycles;

        if ((mapped_addr_life_events[mapped_addr_life_events.size()-1] < mapping_addr_life_events[0])
         || (mapping_addr_life_events[mapping_addr_life_events.size()-1] < mapped_addr_life_events[0])) 
        {
            // cout << "   mapped_addr: " << std::hex << x_table[mapped_addr_idx[i]].address << ": " << total_overlap_score << endl;
            continue;
        }

        // merge mapping address and mapped address lifetime events. 0: mapping_addr event, 1: mapped_addr event
        vector<uint32_t> timeline;
        uint32_t mapping_curr_ptr = 0;
        uint32_t mapped_curr_ptr  = 0;

        while ((mapping_curr_ptr < mapping_addr_life_events.size()) || (mapped_curr_ptr < mapped_addr_life_events.size())) {
            if (mapping_curr_ptr == mapping_addr_life_events.size()) {
                timeline.push_back(1);
                mapped_curr_ptr++;
            }
            else if (mapped_curr_ptr == mapped_addr_life_events.size()) {
                timeline.push_back(0);
                mapping_curr_ptr++;
            }
            else if (mapping_addr_life_events[mapping_curr_ptr] < mapped_addr_life_events[mapped_curr_ptr]) {
                timeline.push_back(0);
                mapping_curr_ptr++;
            }
            else if (mapping_addr_life_events[mapping_curr_ptr] > mapped_addr_life_events[mapped_curr_ptr]) {
                timeline.push_back(1);
                mapped_curr_ptr++;
            }
            else if (mapping_addr_life_events[mapping_curr_ptr] == mapped_addr_life_events[mapped_curr_ptr]) {
                if ((timeline.size() > 0) && (timeline[timeline.size()-1] == 0)) {
                    timeline.push_back(1);
                    timeline.push_back(0);
                }
                else {
                    timeline.push_back(0);
                    timeline.push_back(1);
                }
                mapped_curr_ptr++;
                mapping_curr_ptr++;
            }
            else {
                // we should not reach here
                assert(false);
            }
        }

        // cout << "timeline: " << vec_to_string(timeline) << endl;
        for (uint32_t i=1; i < timeline.size(); ++i) {
            if (timeline[i] != timeline[i-1]) {
                total_overlap_score += 1;
            }
        }
        // cout << "   mapped_addr: " << std::hex << x_table[mapped_addr_idx[i]].address << ": " << total_overlap_score << endl;
        // cout << "   mapped_addr_life_events: " << vec_to_string(mapped_addr_life_events) << endl;
    }
    // cout << "total_overlap_score: " << total_overlap_score << endl;
    // assert(false);
    return total_overlap_score;
}

void Index::map_index(vector<Record>& x_table) {
    // 1st round: assign the first N highest frequency addresses to N sets (N = num_set)
    vector<vector<uint32_t>> mapped_addr_idx_per_set;
    mapped_addr_idx_per_set.resize(LLC_SET);

    for (uint32_t i=0; i < LLC_SET; ++i) {
        x_table[i].set_id = i;
        mapped_addr_idx_per_set[i].push_back(i);
    }

    uint32_t print_interval = 1;
    // from here on: assign index base on overlapping score
    for (uint32_t addr_idx=LLC_SET; addr_idx < x_table.size(); ++addr_idx) {
        int32_t min_score=-1;
        int32_t min_index=-1;
        for (uint32_t i=0; i < LLC_SET; ++i) {
            uint32_t score = compute_overlap_score(x_table, addr_idx, mapped_addr_idx_per_set[i]);
            if (((score < min_score) && (min_score >= 0)) || (min_score < 0)) {
                min_score = score;
                min_index = i;
            }
        }
        x_table[addr_idx].set_id = min_index;
        mapped_addr_idx_per_set[min_index].push_back(addr_idx);
        if ((addr_idx*100/x_table.size()) == print_interval) {
            printf(" ...%d%% completed\n", addr_idx*100/x_table.size());
            print_interval++;
        }
        // cout << "min_index: " << min_index << ". min_score: " << min_score << endl;
        // assert(false);
    }
    printf(" ...100%% completed\n");
}