#ifndef XORHASH_INDEX_H
#define XORHASH_INDEX_H

#include "index.h"

class XorHashIndex : public Index {
public:
   XorHashIndex() {};
   ~XorHashIndex() {};
   void initialize(vector<Access>& history) {
      index_size = lg2(LLC_SET);
   }
   uint32_t get_set(uint64_t address) { 
      uint32_t unhashed_index = address & ((1 << index_size) - 1);
      uint32_t bitmask        = (address >> index_size) & ((1 << index_size) - 1);
      return unhashed_index ^ bitmask;
   }
   bool update(uint64_t address) { return false; }

protected:
   uint32_t index_size;
};



class CustomXorIndex : public Index {
public:
   CustomXorIndex() {};
   ~CustomXorIndex() {};
   void initialize(vector<Access>& history) {
      index_size = lg2(LLC_SET);
      vector<uint32_t> dna_sequence = read_dna_config();
      for (uint32_t i=0; i < dna_sequence.size(); ++i) {
         if (i % 2 == 0)
            selected_bits_a.push_back(dna_sequence[i]);
         else
            selected_bits_b.push_back(dna_sequence[i]);
      }
      
      cout << "XORHASH CONFIG FILE: " << CONFIG_FILE << endl;
      cout << "dna_sequence: " << vec_to_string(dna_sequence) << endl;
      cout << "SeqA: " << vec_to_string(selected_bits_a) << endl;
      cout << "SeqB: " << vec_to_string(selected_bits_b) << endl;
   }

   uint32_t get_set(uint64_t address) {
      uint32_t hashed_index = 0;
      for (uint32_t i=0; i < index_size; ++i) {
         uint32_t selected_bit_a = (address >> selected_bits_a[i]) & 1;
         uint32_t selected_bit_b = (address >> selected_bits_b[i]) & 1;
         hashed_index = (hashed_index << 1) | (selected_bit_a ^ selected_bit_b);
      }
      return hashed_index;
   }
   
   bool update(uint64_t address) { return false; }

   vector<uint32_t> read_dna_config() {
      ifstream conf_file(CONFIG_FILE);
      string app_name = "";

      size_t end_position = TRACE_FILE.find("-LLC-TRACE");
      for (uint32_t i=end_position-1; i > 0; i--) {
         if (TRACE_FILE[i] == '/')
            break;
         else {
            app_name = TRACE_FILE[i] + app_name;
         }
      }

      // further parsing for GAP and XSBench apps
      if (app_name.find("-") != string::npos) {
         string reparsing_name = app_name;
         app_name = "";
         end_position = reparsing_name.find("-");

         for (uint32_t i=end_position-1; i > 0; i--) {
            app_name = reparsing_name[i] + app_name;
         }
      }

      string line;
      if (conf_file.is_open()) {
         while(getline(conf_file, line)) {
            if (line.find(app_name+" = ") != string::npos) {
               return parse_config(line);
            }
         }
      }
   }

   vector<uint32_t> parse_config(string input_line) {
      // input format: [app_name] = [dna_sequence in space-separated-numbers format]
      vector<uint32_t> read_sequence;
      uint32_t element=0;
      uint64_t split_index=0;
      for (; split_index < input_line.size(); ++split_index) {
         if (input_line[split_index] == '=') {
               split_index++;
               break;
         }
      }

      string str_sequence = input_line.substr(split_index, input_line.size());
      stringstream ss(str_sequence);
      while (ss >> element)
         read_sequence.push_back(element);
      
      return read_sequence;
   }

protected:
   uint32_t index_size;
   vector<uint32_t> selected_bits_a;
   vector<uint32_t> selected_bits_b;
};

#endif // XORHASH_INDEX_H