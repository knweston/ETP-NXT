#include "global.h"

//========================================================================================================================//
// HELPER CLASSES
//========================================================================================================================//
bool Record::compare_frequency(const Record &r1, const Record &r2) {
    if (r1.frequency == r2.frequency)
        return r1.address < r2.address;
    else
        return r1.frequency > r2.frequency;
}

//========================================================================================================================//
// HELPER FUNCTIONS
//========================================================================================================================//
void init_global_history() {
    ifstream trace_file(TRACE_FILE);

    if (trace_file) {
        while(!trace_file.eof()) {
            string line;
            uint64_t address=0;
            uint64_t ip=0;
            string type="";
            getline(trace_file, line);
            stringstream ss(line);
            ss >> address;
            ss >> ip;
            ss >> type;

            // skip zero address
            if (!address)
                continue;

            // add new access entry to the global access history
            Access new_access;
            new_access.address = address;
            new_access.ip      = ip;
            
            if (type == "LOAD")
                new_access.type = 0;
            else if (type == "RFO")
                new_access.type = 1;
            else if (type == "PREFETCH")
                new_access.type = 2;
            else if (type == "WRITEBACK")
                new_access.type = 3;
            else {
                cout << "Unrecognized Access Type: " << type << endl;
                exit(1);
            }
            GLOBAL_HISTORY.push_back(new_access);
        }
        trace_file.close();
    }
}

int check_hit(uint64_t address, uint32_t set_index) {
    if (set_index >= LLC_SET)
        return -1;
    for (uint32_t way=0; way < LLC_WAY; ++way) {
        if (CACHE[set_index][way] == address)
            return way;
    }
    return -1;
}

void print_final_stats() {
    uint64_t total_accesses  = ACCESSES[LOAD] + ACCESSES[RFO] + ACCESSES[PREFETCH] + ACCESSES[WRITEBACK];
    uint64_t total_hits      = HITS[LOAD] + HITS[RFO] + HITS[PREFETCH] + HITS[WRITEBACK];
    uint64_t total_misses    = MISSES[LOAD] + MISSES[RFO] + MISSES[PREFETCH] + MISSES[WRITEBACK];
    uint64_t total_evictions = 0;
    for (uint32_t set=0; set < LLC_SET; ++set)
        total_evictions += EVICTIONS[set];

    cout << "========================================================================" << endl;
    cout << "REMAP_FRACTION:     " << REMAP_FRACTION << endl;
    cout << "TRANSLATION_ENABLE: " << TRANSLATION_ENABLE << endl;
    cout << "USE_RANDOM_CLASS:   " << USE_RANDOM_CLASS << endl;
    cout << "XOR_NEW_FRAMEID:    " << XOR_NEW_FRAMEID << endl;
    cout << "========================================================================" << endl;
    cout << "TRACE   = " << TRACE_FILE << endl;
    cout << "LLC_SET = " << LLC_SET << endl;
    cout << "LLC_WAY = " << LLC_WAY << endl;
    cout << "THREADS = " << NUM_THREADS << endl;
    cout << "========================================================================" << endl;
    cout << "LLC FINAL STATISTICS" << endl;
    cout << "========================================================================" << endl;
    cout << left << setw(15) << ""           << setw(15) << "ACCESSES"          << setw(15) << "HITS"          << setw(15) << "MISSES"          << endl;
    cout << left << setw(15) << "LOAD"       << setw(15) << ACCESSES[LOAD]      << setw(15) << HITS[LOAD]      << setw(15) << MISSES[LOAD]      << endl;
    cout << left << setw(15) << "RFO"        << setw(15) << ACCESSES[RFO]       << setw(15) << HITS[RFO]       << setw(15) << MISSES[RFO]       << endl; 
    cout << left << setw(15) << "PREFETCH"   << setw(15) << ACCESSES[PREFETCH]  << setw(15) << HITS[PREFETCH]  << setw(15) << MISSES[PREFETCH]  << endl; 
    cout << left << setw(15) << "WRITEBACK"  << setw(15) << ACCESSES[WRITEBACK] << setw(15) << HITS[WRITEBACK] << setw(15) << MISSES[WRITEBACK] << endl;
    cout << "========================================================================" << endl;
    cout << left << setw(15) << "TOTAL"      << setw(15) << total_accesses      << setw(15) << total_hits      << setw(15) << total_misses << endl;
    cout << "========================================================================" << endl;
    cout << left << setw(15) << "EVICTIONS"  << total_evictions << endl;
    cout << left << setw(15) << "DROPS:"     << NUM_DROPS << endl;
    cout << "========================================================================" << endl;
}

// log base 2 function from efectiu (from ChampSim)
int lg2(int n) {
    int i, m = n, c = -1;
    for (i=0; m; i++) {
        m /= 2;
        c++;
    }
    return c;
}

void* compute_distance(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    for (uint32_t i=0; i < LLC_WAY/NUM_THREADS; ++i) {
        uint32_t way_id = data->way_list[i];
        uint64_t index  = data->start_pt;
        for (; index < data->history->size(); ++index) {
            if (data->victim_set[way_id] == (*data->history)[index]) // address found
                break;
        }
        data->rdist_arr[way_id] = index - data->start_pt;
    }
    return 0;
}

void create_checkpoint(ofstream& cp_file) {
    // Save simulation settings
    cp_file << CURRENT_PTR << endl;
    cp_file << PRINT_INTERVAL << endl;
    cp_file << INDEXING_SCHEME << endl;
    cp_file << REPL_POLICY << endl;

    // Save performance counters' state
    for (uint32_t type=0; type < NUM_TYPES; ++type) {
        cp_file << ACCESSES[type] << endl;
        cp_file << HITS[type] << endl;
        cp_file << MISSES[type] << endl;
    }
    for (uint32_t set=0; set < LLC_SET; ++set)
        cp_file << EVICTIONS[set] << endl;

    // Save cache content
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way)
            cp_file << CACHE[set][way] << endl;
    }
}

void load_checkpoint(ifstream& cp_file) {
    // Load simulation settings
    string line;
    getline(cp_file, line);
    CURRENT_PTR    = stol(line);
    getline(cp_file, line);
    PRINT_INTERVAL = stol(line);
    getline(cp_file, INDEXING_SCHEME);
    getline(cp_file, REPL_POLICY);

    // Load performance counters' content
    for (uint32_t type=0; type < NUM_TYPES; ++type) {
        getline(cp_file, line);
        ACCESSES[type] = stol(line);
        getline(cp_file, line);
        HITS[type] = stol(line);
        getline(cp_file, line);
        MISSES[type] = stol(line);
    }
    for (uint32_t set=0; set < LLC_SET; ++set) {
        getline(cp_file, line);
        EVICTIONS[set] = stol(line);
    }

    // Load cache content
    for (uint32_t set=0; set < LLC_SET; ++set) {
        for (uint32_t way=0; way < LLC_WAY; ++way) {
            getline(cp_file, line);
            CACHE[set][way] = stol(line);
        }
    }
}

void export_eviction_result() {
    uint64_t total_evictions = 0;
    string filename;

    for (uint32_t i=0; i < TRACE_FILE.size()-13; ++i)
        filename += TRACE_FILE[i];
    filename += "SET-EVICTIONS.txt";
    ofstream out_file(filename);
    for (uint32_t set=0; set < LLC_SET; ++set)
        out_file << set << " " << EVICTIONS[set] << endl;
    out_file.close();
}

int32_t readPortConfig(string filename) {
    ifstream conf_file(filename);
    string line;
    int32_t port_num=-1;
    if (conf_file.is_open()) {
        while(getline(conf_file, line)) {
            if (line.find("port") != string::npos) {
                uint64_t split_index=0;
                for (; split_index < line.size(); ++split_index) {
                    if (line[split_index] == '=') {
                        split_index++;
                        break;
                    }
                }
                string temp;
                for (; split_index < line.size(); ++split_index) {
                    temp += line[split_index];
                }
                port_num = stoi(temp);
                break;
            }
        }
        conf_file.close();
    }
    return port_num;
}

string readIPConfig(string filename) {
    ifstream conf_file(filename);
    string line;
    string ip_address;
    if (conf_file.is_open()) {
        while(getline(conf_file, line)) {
            if (line.find("ipaddress") != string::npos) {
                uint64_t split_index=0;
                for (; split_index < line.size(); ++split_index) {
                    if (line[split_index] == '=') {
                        split_index++;
                        break;
                    }
                }
                for (; split_index < line.size(); ++split_index) {
                    if (line[split_index] != ' ')
                        ip_address += line[split_index];
                }
                break;
            }
        }
        conf_file.close();
    }
    return ip_address;
}

int32_t createSocket() {
    int32_t sock = 0;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw "Socket creation error";
    return sock;
}

string flatten(double* array, uint64_t end_pt, uint64_t start_pt) {
    string data = "";
    for (uint32_t i=start_pt; i < end_pt; ++i)
        data += to_string(array[i]) + "_";

    data.pop_back();
    return data;
}

void reset_statistics(bool is_warmup) {
    memset(ACCESSES, 0, sizeof(ACCESSES));
    memset(HITS, 0, sizeof(HITS));
    memset(MISSES, 0, sizeof(MISSES));
    memset(EVICTIONS, 0, sizeof(EVICTIONS));
    memset(INTERVAL_EVICTIONS, 0, sizeof(INTERVAL_EVICTIONS));
    memset(INTERVAL_MISSCOUNT, 0, sizeof(INTERVAL_MISSCOUNT));
    NUM_DROPS = 0;
    if (!is_warmup) {
        memset(CACHE, 0, sizeof(CACHE));
        CURRENT_PTR = 0;
    }
}

void normalize_array(uint64_t* input_arr, double* output_arr, uint64_t array_size) {
    uint32_t i_min = 0;
    uint32_t i_max = 0;
    for (uint32_t i=1; i < array_size; ++i) {
        if (input_arr[i] < input_arr[i_min])
            i_min = i;
        if (input_arr[i] > input_arr[i_max])
            i_max = i;
    }

    for (uint32_t i=0; i < array_size; ++i)
        output_arr[i] = (double)(input_arr[i] - input_arr[i_min]) / (input_arr[i_max] - input_arr[i_min]);
}

void standardize_array(uint64_t* input_arr, double* output_arr, uint64_t array_size) {
    double mean   = compute_mean(input_arr, array_size);
    double sig_sd = compute_std_dev(input_arr, array_size);
    for (uint32_t i=0; i < array_size; ++i) {
        if (int(sig_sd*10000))
            output_arr[i] = (input_arr[i] - mean) / sig_sd;
        else
            output_arr[i] = 0;
    }
}

void standardize_array(double* input_arr, uint64_t array_size) {
    double mean   = compute_mean(input_arr, array_size);
    double sig_sd = compute_std_dev(input_arr, array_size);
    for (uint32_t i=0; i < array_size; ++i) {
        if (int(sig_sd*10000))
            input_arr[i] = (input_arr[i] - mean) / sig_sd;
        else
            input_arr[i] = 0;
    }
}



// VA-PA translation simulation API
void init_global_page_table() {
    ifstream trace_file(TRACE_FILE);
    vector<uint64_t> sorted_buffer;
    vector<uint64_t> unsorted_buffer;

    if (trace_file) {
        while(!trace_file.eof()) {
            string line;
            uint64_t address=0;
            uint64_t ip=0;
            string type="";
            getline(trace_file, line);
            stringstream ss(line);
            ss >> address;
            ss >> ip;
            ss >> type;

            // skip zero address
            if (!address)
                continue;

            uint64_t page_id = (address >> (LOG2_PAGE_SIZE-LOG2_BLOCK_SIZE)) & PAGEID_MASK;
            bool found = binary_search(sorted_buffer.begin(), sorted_buffer.end(), page_id);
            if (!found) { 
                for (uint32_t i=0; i<unsorted_buffer.size(); ++i) {
                    if (page_id == unsorted_buffer[i]) {
                        found = true;
                        break;
                    }
                }
            }

            if (!found) {
                unsorted_buffer.push_back(page_id);
            }

            if (unsorted_buffer.size() >= 3000) {
                sorted_buffer.insert(sorted_buffer.end(), unsorted_buffer.begin(), unsorted_buffer.end());
                insertion_sort(sorted_buffer);
                unsorted_buffer.resize(0);
            }    
        }
        trace_file.close();
    }

    sorted_buffer.insert(sorted_buffer.end(), unsorted_buffer.begin(), unsorted_buffer.end());
    insertion_sort(sorted_buffer);

    for (uint32_t i=0; i<sorted_buffer.size(); ++i) {
        GLOBAL_PAGETABLE.push_back(Translation(sorted_buffer[i], sorted_buffer[i]));
    }
}


void insertion_sort(std::vector<uint64_t>& vec) {
	for(auto it = vec.begin(); it != vec.end(); it++) {
   		auto const insertion_point = std::upper_bound(vec.begin(), it, *it); // Search
   		std::rotate(insertion_point, it, it+1); //insert
 	} 
}

void change_va_pa_mapping() {
    uint32_t num_pages_to_change = (uint32_t)(GLOBAL_PAGETABLE.size()/REMAP_FRACTION);
    uint32_t start_pt = rand() % (GLOBAL_PAGETABLE.size()-num_pages_to_change);
    uint32_t end_pt   = start_pt + num_pages_to_change;

    // randomly generate a bitmask
    uint32_t randmask = RAND_ENGINE.draw_rand() & PAGEID_MASK;
    
    for (uint32_t i=start_pt; i<end_pt; ++i) {
        GLOBAL_PAGETABLE[i].frameid ^= randmask;
    }
}


uint64_t translate_addr(uint64_t addr) {
    // get pageID
    uint64_t page_id = (addr >> (LOG2_PAGE_SIZE-LOG2_BLOCK_SIZE)) & PAGEID_MASK;
    Translation new_pair(page_id, page_id);
    int32_t found_idx = search(GLOBAL_PAGETABLE, 0, GLOBAL_PAGETABLE.size()-1, new_pair);

    if (found_idx >= 0) {
        // get frameID
        uint64_t frame_id = GLOBAL_PAGETABLE[found_idx].frameid;

        // translate addr
        uint64_t page_offset = addr & ((1 << (LOG2_PAGE_SIZE-LOG2_BLOCK_SIZE)) - 1);
        uint64_t translated_addr = (frame_id << (LOG2_PAGE_SIZE-LOG2_BLOCK_SIZE)) | page_offset;

        // return translated addr
        return translated_addr;
    }
    else {
        cout << "Error: page not found for address: " << addr << endl;
        assert(false);
    }
}

void reset_va_pa_mapping() {
    for (uint32_t p=0; p<GLOBAL_PAGETABLE.size(); ++p) {
        GLOBAL_PAGETABLE[p].frameid = GLOBAL_PAGETABLE[p].pageid;
    }
}