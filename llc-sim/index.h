#ifndef INDEX_H
#define INDEX_H

#include "global.h"

//========================================================================================================================//
// INDEX BASE CLASS
//========================================================================================================================//

class Index {
public:
    Index() {}
    virtual ~Index() {}
    void     profile(vector<Access>& history, bool keep_cycle_info=false, bool export_mapping=false);
    void     map_index(vector<Record>& x_table);
    uint32_t compute_overlap_score(vector<Record>& x_table, uint32_t mapping_addr_idx, vector<uint32_t>& mapped_addr_idx);

    int64_t find_addr(uint64_t address, vector<Record>& profile_table);
    virtual uint32_t get_set(uint64_t address) = 0;
    virtual void initialize(vector<Access>& history) = 0;
    virtual bool update(uint64_t address) = 0;
};

class PrimeHashIndex : public Index {
public:
   PrimeHashIndex() {};
   ~PrimeHashIndex() {};
   void initialize(vector<Access>& history) {
        switch(LLC_SET) {
            case 256:
                prime_no = 251;
                break;
            
            case 512:
                prime_no = 509;
                break;
            
            case 1024:
                prime_no = 1021;
                break;

            case 2048:
                prime_no = 2039;
                break;

            case 4096:
                prime_no = 4093;
                break;

            case 8192:
                prime_no = 8191;
                break;

            case 16384:
                prime_no = 16381;
                break;

            case 32768:
                prime_no = 32749;
                break;

            default:
                cout << "Unrecognized NUM_SET: " << LLC_SET << endl;
                exit(1);
        }
        cout << "PRIME NUMBER: " << prime_no << endl;
   }
   uint32_t get_set(uint64_t address) { 
      return address % prime_no;
   }
   bool update(uint64_t address) { return false; }

protected:
   uint32_t prime_no;
};

#endif // INDEX_H