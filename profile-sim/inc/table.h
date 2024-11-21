#ifndef TABLE_H
#define TABLE_H

#include <champsim.h>

class Record {
public:
    Record(uint64_t _addr, uint64_t _freq=0)
    :address(_addr), frequency(_freq) {}

    void increment_freq() { ++frequency; }
    uint64_t get_address() { return address; }
    uint64_t get_frequency() { return frequency; }
    void set_setid(uint64_t _id) { set_id = _id; }
    uint64_t get_setid() { return set_id; }
    // bool static compare_frequency(const Record &r1, const Record &r2) { return r1.frequency > r2.frequency; }
    bool static compare_frequency(const Record &r1, const Record &r2);
    
    friend bool operator== (const Record &r1, const Record &r2) { return r1.address == r2.address; }
    friend bool operator!= (const Record &r1, const Record &r2) { return r1.address != r2.address; }
    friend bool operator<  (const Record &r1, const Record &r2) { return r1.address <  r2.address; }
    friend bool operator<= (const Record &r1, const Record &r2) { return r1.address <= r2.address; }
    friend bool operator>  (const Record &r1, const Record &r2) { return r1.address >  r2.address; }
    friend bool operator>= (const Record &r1, const Record &r2) { return r1.address >= r2.address; }

private:
    uint64_t address;
    uint64_t frequency;
    uint32_t set_id;
};


class Table {
public:
    Table() {};
    long long find_addr(uint64_t addr);
    void add_record(uint64_t addr);
    void increment_freq(uint64_t index) { records[index].increment_freq(); }
    uint64_t num_records() { return records.size(); }
    long long search(vector<Record> &array, long long left_pt, long long right_pt, Record &element);
    void export_table(string filename);
    bool import_table(string filename);
    void assign_set_id(uint32_t num_sets);
    vector<Record> sort_by_freq(vector<Record> array);

    Record& get_record(uint64_t index) { return records[index]; }  

private:
    vector<Record> records;
};

#endif // TABLE_H