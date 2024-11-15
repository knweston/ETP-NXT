#ifndef GLOBAL_H
#define GLOBAL_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <chrono>
#include <pthread.h>
#include <queue>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <bitset>
#include <random>

// Cache Configurations
#define NUM_CORES 1
#define LLC_SET NUM_CORES*2048
#define LLC_WAY 16
#define LOG2_BLOCK_SIZE 6
#define LOG2_PAGE_SIZE  12          // page size = frame size
#define LOG2_NUM_FRAMES 20          // assume 4GB memory, 4KB frame size => 2^20 frames 
#define PAGEID_MASK 0xFFFFFFFFF     // 36-bit pageID

// VA-PA Translation Simulation Settings
#define REMAP_FRACTION 4            // amount of VA-PA mappings change = 1/REMAP_FRACTION
#define TRANSLATION_ENABLE 0
#define USE_RANDOM_CLASS 0
#define XOR_NEW_FRAMEID 1

// Multi-threading
#define MAX_THREADS 1

// Cache Access Types from ChampSim
#define LOAD      0
#define RFO       1
#define PREFETCH  2
#define WRITEBACK 3
#define NUM_TYPES 4

using namespace std;

//========================================================================================================================//
// HELPER CLASSES
//========================================================================================================================//
class Access {
public:
    Access() {}
    uint64_t address = 0;
    uint64_t ip      = 0;
    uint32_t type    = 0;
};

class Record {
public:
    uint64_t address   = 0;
    uint64_t frequency = 1;
    uint32_t set_id    = 0;

    Record() {}
    bool static compare_frequency(const Record &r1, const Record &r2);

    friend bool operator== (const Record &r1, const Record &r2) { return r1.address == r2.address; }
    friend bool operator!= (const Record &r1, const Record &r2) { return r1.address != r2.address; }
    friend bool operator<  (const Record &r1, const Record &r2) { return r1.address <  r2.address; }
    friend bool operator<= (const Record &r1, const Record &r2) { return r1.address <= r2.address; }
    friend bool operator>  (const Record &r1, const Record &r2) { return r1.address >  r2.address; }
    friend bool operator>= (const Record &r1, const Record &r2) { return r1.address >= r2.address; }
};

struct ThreadData {
    vector<uint64_t> *history;
    uint64_t* rdist_arr;
    uint64_t* victim_set;
    uint64_t start_pt;
    uint32_t* way_list;
    
    ThreadData(vector<uint64_t> *hist, uint64_t ptr, uint64_t* set, uint64_t rd[], uint32_t list[])
        :history(hist), start_pt(ptr), victim_set(set), rdist_arr(rd), way_list(list) {}
};



struct Translation {
    Translation(uint64_t pid, uint64_t fid)
        :pageid(pid), frameid(fid) {}
    uint64_t pageid;    // computed from original address in LLC trace; we call pageid but it is supposed to be frameid
    uint64_t frameid;   // randomly generated frameid 

    friend bool operator== (const Translation &r1, const Translation &r2) { return r1.pageid == r2.pageid; }
    friend bool operator!= (const Translation &r1, const Translation &r2) { return r1.pageid != r2.pageid; }
    friend bool operator<  (const Translation &r1, const Translation &r2) { return r1.pageid <  r2.pageid; }
    friend bool operator<= (const Translation &r1, const Translation &r2) { return r1.pageid <= r2.pageid; }
    friend bool operator>  (const Translation &r1, const Translation &r2) { return r1.pageid >  r2.pageid; }
    friend bool operator>= (const Translation &r1, const Translation &r2) { return r1.pageid >= r2.pageid; }

    Translation(const Translation& x_trans) {
        pageid  = x_trans.pageid;
        frameid = x_trans.frameid; 
    }
};

class RANDOM {
public:
    std::random_device rd;
    std::mt19937_64 engine{rd()};
    std::uniform_int_distribution<uint64_t> dist{0, 0xFFFFFFFFF}; // used to generate random physical page numbers

    RANDOM (uint64_t seed) {
        engine.seed(seed);
    }

    uint64_t draw_rand() {
        return dist(engine);
    }
};

//========================================================================================================================//
// HELPER FUNCTIONS
//========================================================================================================================//
void init_global_history();
int check_hit(uint64_t address, uint32_t set_index);
void print_final_stats();
int lg2(int n);
void* compute_distance(void* arg);
void create_checkpoint(ofstream& cp_file);
void load_checkpoint(ifstream& cp_file);
void export_eviction_result();

int32_t createSocket();
int32_t readPortConfig(string filename);
string readIPConfig(string filename);

void normalize_array(uint64_t* input_arr, double* output_arr, uint64_t array_size);
void standardize_array(uint64_t* input_arr, double* output_arr, uint64_t array_size);
void standardize_array(double* input_arr, uint64_t array_size);
string flatten(double* array, uint64_t end_pt, uint64_t start_pt=0);
void reset_statistics(bool is_warmup=false);


template<typename T>
int64_t search(vector<T> &array, int64_t left_pt, int64_t right_pt, T &element) {
    if (right_pt >= left_pt) {
        uint64_t mid_pt = left_pt + (right_pt - left_pt)/2;
        if (array[mid_pt] == element)
            return mid_pt;
        else if (array[mid_pt] > element)
            return search(array, left_pt, mid_pt-1, element);
        else
            return search(array, mid_pt+1, right_pt, element);
    }
    return -1;
}

template<typename T> 
double compute_mean(T* array, uint64_t array_size) {
    if (array_size) {
        double total_accesses = 0.0;
        for (uint32_t i=0; i < array_size; ++i)
            total_accesses += array[i];
        return total_accesses / array_size;
    }
    else
        return 0;
}

template<typename T>
double compute_std_dev(T* array, uint64_t array_size) {
    double mean     = compute_mean(array, array_size);
    double variance = 0.0;
    for (uint32_t i=0; i < array_size; ++i)
        variance += pow(array[i] - mean, 2);
    variance /= array_size;
    return sqrt(variance);
}

template<typename T>
bool is_existed(uint32_t x_pt, vector<T> array) {
    for (uint32_t i=0; i < array.size(); ++i) {
        if (x_pt == array[i])
            return true;
    }
    return false;
}

/*
 * min-inclusive, max-exclusive
 * fixed_first_bits=true means generating an interleaving sequence: 0, randnum_0, 1, randnum_1, 2, randnum_2, ... 
 */ 

template<typename T>
vector<T> rand_gen(uint32_t size, T max_val, T min_val=0, bool fixed_first_bits=false) {
    vector<T> array;
    uint32_t first_bits = 0;
    while(array.size() < size) {
        T new_key = (rand() % (max_val-min_val)) + min_val;
        if (!is_existed(new_key, array)) {
            if (fixed_first_bits) {
                array.push_back(first_bits);
                first_bits++;
            }
            array.push_back(new_key);
        }
    }
    return array;
}

template<typename T>
string arr_to_string(T* array, uint32_t array_size, string split_sym) {
    string output = "";
    for (uint32_t i=0; i < array_size; ++i) {
        output += to_string(array[i]) + split_sym;
    }
    output.pop_back();
    return output;
}

template<typename T>
string vec_to_string(vector<T> &vec, string split_sym=",") {
    string output = "";
    for (uint32_t i=0; i < vec.size(); ++i) {
        output += to_string(vec[i]) + split_sym;
    }
    output.pop_back();
    return output;
}

template<typename T>
T sum(T* array, uint64_t array_size) {
    T sum = 0;
    for (uint32_t i=0; i < array_size; ++i) {
        sum += array[i];
    }
    return sum;
}


/*
 * VA-PA translation strategy:
 * 1. we assume the global history addresses are virtual addresses.
 * 2. we do some extra things:
 * 2.1. during the global history initialization, we profile a list of distinct pageIDs of the application
 * 2.2. every time we run main_loop(), we first do the following:
 *      - randomly pick a set of pageIDs.
 *      - generate one random frameID per pageID, keep the mapping in a table.
 *      - modify each address in the global history by its new frameID, if any.
 *      - start the main_loop as normal.
 *
 * NOTE: global history addresses does not include 6-bit block size
 */
void init_global_page_table();
void insertion_sort(std::vector<uint64_t>& vec);
void change_va_pa_mapping();
uint64_t translate_addr(uint64_t addr);
void reset_va_pa_mapping();

//========================================================================================================================//
// GLOBAL VARIABLES
//========================================================================================================================//
extern vector<Access> GLOBAL_HISTORY;
extern vector<Translation> GLOBAL_PAGETABLE;

extern uint64_t CACHE[LLC_SET][LLC_WAY];
extern uint64_t ACCESSES[NUM_TYPES];
extern uint64_t HITS[NUM_TYPES];
extern uint64_t MISSES[NUM_TYPES];
extern uint64_t EVICTIONS[LLC_SET];
extern uint64_t INTERVAL_EVICTIONS[LLC_SET];
extern uint64_t INTERVAL_MISSCOUNT[LLC_SET];

extern uint32_t NUM_THREADS;
extern uint32_t PRINT_INTERVAL;
extern uint32_t CHECKPOINT;
extern uint64_t CURRENT_PTR;
extern uint64_t WARMUP_INST;
extern uint64_t NUM_DROPS;
extern uint64_t NUM_LOOPS;
extern uint64_t MAX_INST;
extern uint64_t WARM_INST;
extern string   TRACE_FILE;
extern string   INDEXING_SCHEME;
extern string   REPL_POLICY;
extern string   CONFIG_FILE;
extern RANDOM   RAND_ENGINE;
extern uint32_t RAND_SEED;

#endif // GLOBAL_H