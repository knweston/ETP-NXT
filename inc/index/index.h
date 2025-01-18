#ifndef INDEX_H
#define INDEX_H

#include "cache_global.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <algorithm>

extern string CONFIG_FILE;
extern vector<string> APP_LIST, APP_PATH;

//========================================================================//
// HELPER FUNCTIONS
//========================================================================//
string flatten(double* array, uint64_t end_pt, uint64_t start_pt);
void override_block(BLOCK *src, BLOCK *des);
bool force_writeback(BLOCK* block, MEMORY *lower_level, uint32_t fill_lvl, uint32_t cpu_id);
vector<uint64_t> get_significant_indices(uint64_t* array, uint32_t array_size, double mean, uint32_t num_select);

//========================================================================//
// BASE CLASS INDEX
//========================================================================//
class Index {
public:
    Index(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length, uint32_t x_warmup=0)
        :NUM_SET(x_numset), NUM_WAY(x_numway), 
         INTERVAL_SIZE(x_interval_length), access_ctr(0), num_interval(0)
    {
        EVICTION = new uint64_t [NUM_SET];
        N_ACCESS = new uint64_t [NUM_SET];
        MISS     = new uint64_t [NUM_SET];
        PI_EVICTION = new uint64_t [NUM_SET];
        PI_N_ACCESS = new uint64_t [NUM_SET];
        PI_MISS     = new uint64_t [NUM_SET];
        memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
        memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
        memset(MISS, 0, NUM_SET * sizeof(*MISS));
        memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
        memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
        memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));

        warmup_access(x_warmup);
    }

    ~Index() {
        delete[] EVICTION;
        delete[] N_ACCESS;
        delete[] MISS;
        delete[] PI_EVICTION;
        delete[] PI_N_ACCESS;
        delete[] PI_MISS;
    }

    virtual uint32_t get_set(uint64_t address) = 0;
    virtual void     update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type);
    virtual void     print_stats() = 0;
    virtual void     eviction_update(BLOCK &victim_blk) {}

    virtual void increment_eviction(uint32_t set) { 
        EVICTION[set]++;
        PI_EVICTION[set]++;
    }

    

    void interval_size(uint32_t x_interval_length) {
        INTERVAL_SIZE = x_interval_length;
    }
    void warmup_access(uint32_t x_warmup) {
        num_warmup_access = x_warmup;
        if (num_warmup_access == 0)
            idx_warmup_complete = true;
        else
            idx_warmup_complete = false;
        cout << "cache: " << NUM_SET << " x " << NUM_WAY
             << ". num warmup access: " << num_warmup_access << endl;
    }

public:
    const uint32_t NUM_SET;
    const uint32_t NUM_WAY;

    // connection config
    static int32_t m_port;
    static int32_t m_sock;
    static int32_t m_bsize;
    static string  m_ipaddr;

    // runtime stats
    uint64_t* N_ACCESS;     // nohash access (overall)
    uint64_t* PI_N_ACCESS;  // nohash access (per-interval)
    uint64_t* EVICTION;     // eviction      (overall)
    uint64_t* PI_EVICTION;  // eviction      (per-interval)
    uint64_t* MISS;         // miss          (overall)
    uint64_t* PI_MISS;      // miss          (per-interval)
    uint32_t INTERVAL_SIZE;
    uint64_t access_ctr;
    uint32_t num_interval;
    uint32_t num_warmup_access;
    bool idx_warmup_complete;
};


//========================================================================//
// NOHASH INDEX
//========================================================================//
class NoHashIndex : public Index {
public:
    NoHashIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length)
    :Index(x_numset, x_numway, x_interval_length) {}

    ~NoHashIndex() {}
    
    uint32_t get_set(uint64_t address) {
        return (uint32_t) (address & ((1 << lg2(NUM_SET)) - 1));
    }

    void print_stats() {
        cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(N_ACCESS, NUM_SET) / compute_mean(N_ACCESS, NUM_SET) << endl;
        cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
        cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
        cout << "PER-SET EVICTION STATS: " << arr_to_string(EVICTION, NUM_SET, ", ") << endl;
        cout << "PER-SET ACCESS STATS:   " << arr_to_string(N_ACCESS, NUM_SET, ", ") << endl;
    }
};


//========================================================================//
// PRIME HASH INDEX
//========================================================================//
class PrimeHashIndex : public Index {
public:
    PrimeHashIndex(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length)
    :Index(x_numset, x_numway, x_interval_length) 
    {
        switch(x_numset) {
            case 256:
                m_prime_no = 251;
                break;
            
            case 512:
                m_prime_no = 509;
                break;
            
            case 1024:
                m_prime_no = 1021;
                break;

            case 2048:
                m_prime_no = 2039;
                break;

            case 4096:
                m_prime_no = 4093;
                break;

            case 8192:
                m_prime_no = 8191;
                break;

            case 16384:
                m_prime_no = 16381;
                break;

            case 32768:
                m_prime_no = 32749;
                break;

            default:
                cout << "Unrecognized NUM_SET: " << x_numset << endl;
                exit(1);
        }
        cout << "PRIME NUMBER: " << m_prime_no << endl;

        P_ACCESS    = new uint64_t [NUM_SET];
        PI_P_ACCESS = new uint64_t [NUM_SET];
        memset(P_ACCESS, 0, NUM_SET * sizeof(*P_ACCESS));
        memset(PI_P_ACCESS, 0, NUM_SET * sizeof(*PI_P_ACCESS));
    }

    ~PrimeHashIndex() {}
    uint32_t get_set(uint64_t address) { return address % m_prime_no; }

    void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) override {
        uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
        uint32_t hashed_index   = get_set(address);
        N_ACCESS[unhashed_index]++;
        P_ACCESS[hashed_index]++;
        PI_N_ACCESS[unhashed_index]++;
        PI_P_ACCESS[hashed_index]++;
        access_ctr++;

        if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
            cout << endl << endl;
            cout << "========================================================================" << endl;
            cout << "End of interval # " << num_interval << " Summary" << endl;
            cout << "========================================================================" << endl;
            cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
            cout << "   prime_stdev:    " << std::fixed << compute_std_dev(PI_P_ACCESS, NUM_SET) << endl;
            cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
            cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
            cout << "========================================================================" << endl;
            memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
            memset(PI_P_ACCESS, 0, NUM_SET * sizeof(*PI_P_ACCESS));
            memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
            access_ctr = 0;
            num_interval++;
        }

        if (!idx_warmup_complete) {
            if (compute_sum(N_ACCESS, NUM_SET) == num_warmup_access) {
                cout << "total access: " << compute_sum(N_ACCESS, NUM_SET) << endl;
                finish_warmup();
                memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
                memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
                memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
                memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
                memset(MISS, 0, NUM_SET * sizeof(*MISS));
                memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
                memset(P_ACCESS, 0, NUM_SET * sizeof(*P_ACCESS));
                memset(PI_P_ACCESS, 0, NUM_SET * sizeof(*PI_P_ACCESS));
                access_ctr = 0;
                idx_warmup_complete = true;   
            }
        }
    }

    void print_stats() {
        cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(P_ACCESS, NUM_SET) / compute_mean(P_ACCESS, NUM_SET) << endl;
        cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
        cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
        cout << "PER-SET EVICTION STATS: " << arr_to_string(EVICTION, NUM_SET, ", ") << endl;
        cout << "PER-SET ACCESS STATS:   " << arr_to_string(P_ACCESS, NUM_SET, ", ") << endl;
    }

protected:
    uint32_t m_prime_no;

    /* total runtime access counters:
        P_ACCESS + PI_P_ACCESS (this class)
        N_ACCESS + PI_N_ACCESS (base class)
        EVICTION + PI_EVICTION (base class)
    */
    uint64_t* P_ACCESS;     // prime-hashed access (overall)
    uint64_t* PI_P_ACCESS;  // prime-hashed access (per-interval)
};

//========================================================================//
// XORHASH INDEX
//========================================================================//
class XorHash : public Index {
public:
    XorHash(uint32_t x_numset, uint32_t x_numway, uint32_t x_interval_length)
    :Index(x_numset, x_numway, x_interval_length) {
        X_ACCESS    = new uint64_t [NUM_SET];
        PI_X_ACCESS = new uint64_t [NUM_SET];
        memset(X_ACCESS, 0, NUM_SET * sizeof(*X_ACCESS));
        memset(PI_X_ACCESS, 0, NUM_SET * sizeof(*PI_X_ACCESS));
    }

    ~XorHash() {
        delete[] X_ACCESS;
        delete[] PI_X_ACCESS;
        delete[] bit_sel;
    }

    uint32_t get_set(uint64_t address) {
        uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
        uint32_t bitmask        = (address >> lg2(NUM_SET)) & ((1 << lg2(NUM_SET)) - 1);
        return unhashed_index ^ bitmask;
    }

    void update(uint64_t address, MEMORY *dram, bool is_hit, uint32_t type) override {
        uint32_t unhashed_index = address & ((1 << lg2(NUM_SET)) - 1);
        uint32_t hashed_index   = get_set(address);
        N_ACCESS[unhashed_index]++;
        X_ACCESS[hashed_index]++;
        PI_N_ACCESS[unhashed_index]++;
        PI_X_ACCESS[hashed_index]++;
        access_ctr++;

        if (INTERVAL_SIZE && (access_ctr >= INTERVAL_SIZE)) {
            cout << endl << endl;
            cout << "========================================================================" << endl;
            cout << "End of interval # " << num_interval << " Summary" << endl;
            cout << "========================================================================" << endl;
            cout << "   unhashed_stdev: " << std::fixed << compute_std_dev(PI_N_ACCESS, NUM_SET) << endl;
            cout << "   xorhash_stdev:  " << std::fixed << compute_std_dev(PI_X_ACCESS, NUM_SET) << endl;
            cout << "   eviction_stdev: " << std::fixed << compute_std_dev(PI_EVICTION, NUM_SET) << endl; 
            cout << "   eviction_rate:  " << std::fixed << (double)compute_sum(PI_EVICTION, NUM_SET)/INTERVAL_SIZE << endl;
            cout << "========================================================================" << endl;
            memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
            memset(PI_X_ACCESS, 0, NUM_SET * sizeof(*PI_X_ACCESS));
            memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
            access_ctr = 0;
            num_interval++;
        }

        if (!idx_warmup_complete) {
            if (compute_sum(N_ACCESS, NUM_SET) == num_warmup_access) {
                cout << "total access: " << compute_sum(N_ACCESS, NUM_SET) << endl;
                finish_warmup();
                memset(N_ACCESS, 0, NUM_SET * sizeof(*N_ACCESS));
                memset(PI_N_ACCESS, 0, NUM_SET * sizeof(*PI_N_ACCESS));
                memset(EVICTION, 0, NUM_SET * sizeof(*EVICTION));
                memset(PI_EVICTION, 0, NUM_SET * sizeof(*PI_EVICTION));
                memset(MISS, 0, NUM_SET * sizeof(*MISS));
                memset(PI_MISS, 0, NUM_SET * sizeof(*PI_MISS));
                memset(X_ACCESS, 0, NUM_SET * sizeof(*X_ACCESS));
                memset(PI_X_ACCESS, 0, NUM_SET * sizeof(*PI_X_ACCESS));
                access_ctr = 0;
                idx_warmup_complete = true;   
            }
        }
    }

    void print_stats() {
        cout << "Ovr. Access Stdev/Mean:   " << compute_std_dev(X_ACCESS, NUM_SET) / compute_mean(X_ACCESS, NUM_SET) << endl;
        cout << "Ovr. Eviction Stdev/Mean: " << compute_std_dev(EVICTION, NUM_SET) / compute_mean(EVICTION, NUM_SET) << endl; 
        cout << "Ovr. Evictions:           " << compute_sum(EVICTION, NUM_SET) << endl; 
        cout << "PER-SET EVICTION STATS:" << endl;
        for (uint32_t i=0; i < NUM_SET; ++i)
            cout << EVICTION[i] << endl;

        cout << "PER-SET ACCESS STATS: " << endl;
        for (uint32_t i=0; i < NUM_SET; ++i)
            cout << X_ACCESS[i] << endl;
    }

protected:
    uint64_t* X_ACCESS;     // xor-hashed access (overall)
    uint64_t* PI_X_ACCESS;  // xor-hashed access (per-interval)
    uint64_t* bit_sel;
};

//========================================================================//
// HELPER CLASSES
//========================================================================//
struct Pair {
    Pair(uint64_t a, uint64_t b)
        :val_a(a), val_b(b) {}
    uint64_t val_a = 0;
    uint64_t val_b = 0;

    bool static cmp_val_a(const Pair &p1, const Pair &p2) { return p1.val_a > p2.val_a; }
    bool static cmp_val_b(const Pair &p1, const Pair &p2) { return p1.val_b > p2.val_b; }
};

#endif // INDEX_H