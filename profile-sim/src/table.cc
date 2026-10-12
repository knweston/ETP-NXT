#include "table.h"
#include <fstream>
#include <sstream>
#include <algorithm>

bool Record::compare_frequency(const Record &r1, const Record &r2) {
    if (r1.frequency == r2.frequency)
        return r1.address < r2.address;
    else
        return r1.frequency > r2.frequency;
}

long long Table::find_addr(uint64_t addr) {
    if (!records.empty()) {
        sort(records.begin(), records.end());
        Record new_record(addr);
        return search(records, 0, records.size()-1, new_record);
    }
    else {
        return -1;
    }
}

void Table::add_record(uint64_t addr) {
    Record new_record(addr);
    records.push_back(new_record);
}

long long Table::search(vector<Record> &array, long long left_pt, long long right_pt, Record &element) {
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

void Table::export_table(string filename) {
    ofstream file(filename);
    for (uint64_t i=0; i < records.size(); ++i) {
        file << std::hex << std::setw(64) << std::setfill('0') << records[i].get_address() << " ";
        file << std::dec << std::setw(0) << records[i].get_frequency() << endl;
    }
    file.close();
    cout << "Profiling data exported to file: " << filename << endl;
}

bool Table::import_table(string filename) {
    ifstream file(filename);
    if (file) {
        while(!file.eof()) {
            string line;
            uint64_t address;
            uint64_t frequency;
            getline(file, line);
            stringstream ss(line);
            ss >> address;
            ss >> frequency;
            Record new_record(address, frequency);
            records.push_back(new_record);
        }
        return true;
    }
    return false;
}

void Table::assign_set_id(uint32_t num_sets) {
    vector<Record> freq_sorted = sort_by_freq(records);
    for (uint64_t i=0; i < freq_sorted.size(); ++i) {
        long long index = find_addr(freq_sorted[i].get_address());
        records[index].set_setid(i % num_sets);
    }
}

vector<Record> Table::sort_by_freq(vector<Record> array) {
    sort(array.begin(), array.end(), Record::compare_frequency);
    return array;
}