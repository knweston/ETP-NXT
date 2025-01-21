#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

//========================================================================//
// ALGORITHMS
//========================================================================//
/*
   Binary search
   - input: vector array, search start point in the array, search ending point in the array, search value
   - output: index of the search value (if found) or -1 (if not found)
*/
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

/*
   Linear search of a value in a vector
*/
template<typename T>
bool is_existed(uint32_t x_pt, vector<T> array) {
    for (uint32_t i=0; i < array.size(); ++i) {
        if (x_pt == array[i])
            return true;
    }
    return false;
}

template<typename T>
int32_t linear_search(uint32_t x_pt, vector<T> array) {
    for (uint32_t i=0; i < array.size(); ++i) {
        if (x_pt == array[i])
            return i;
    }
    return -1;
}

/*
   create n-round perfect shuffled vector
*/
inline
vector<uint32_t> perfect_shuffle(uint32_t size, uint32_t num_rounds, uint32_t start_num=0) {
   vector<uint32_t> bit_arr, shuffled_arr;
   for (uint32_t i=0; i < size; ++i)
      bit_arr.push_back(i+start_num);

   for (uint32_t i=0; i < num_rounds; ++i) {
      shuffled_arr.resize(bit_arr.size(), 0);
      for (uint32_t bit=0; bit < bit_arr.size(); ++bit) {
         if (bit <= ((bit_arr.size()+1)/2-1))
            shuffled_arr[2*bit] = bit_arr[bit];
         else {
            if (bit_arr.size() % 2 == 0)
               shuffled_arr[2*bit+1-bit_arr.size()] = bit_arr[bit];
            else
               shuffled_arr[2*bit-bit_arr.size()] = bit_arr[bit];
         }
      }
      bit_arr = shuffled_arr;
   }
   return bit_arr;
}

//========================================================================//
// ARRAY/VECTOR ARITHMETIC OPERATIONS
//========================================================================//
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
T compute_sum(T* array, uint64_t array_size) {
    T sum = 0;
    for (uint32_t i=0; i < array_size; ++i) {
        sum += array[i];
    }
    return sum;
}

template<typename T>
void normalize_array(T* input_arr, double* output_arr, uint64_t array_size) {
   double mean   = compute_mean(input_arr, array_size);
   if ((uint32_t)mean == 0)
      return;

   for (uint32_t i=0; i < array_size; ++i) {
      output_arr[i] = (input_arr[i]/mean);
   }
}

template<typename T>
void standardize_array(T* input_arr, uint64_t array_size) {
    double mean   = compute_mean(input_arr, array_size);
    double sig_sd = compute_std_dev(input_arr, array_size);
    for (uint32_t i=0; i < array_size; ++i) {
        if (int(sig_sd*10000))
            input_arr[i] = (input_arr[i] - mean) / sig_sd;
        else
            input_arr[i] = 0;
    }
}

template<typename T>
void standardize_array(T* input_arr, double* output_arr, uint64_t array_size) {
    double mean   = compute_mean(input_arr, array_size);
    double sig_sd = compute_std_dev(input_arr, array_size);
    for (uint32_t i=0; i < array_size; ++i) {
        if (int(sig_sd*10000))
            output_arr[i] = (input_arr[i] - mean) / sig_sd;
        else
            output_arr[i] = 0;
    }
}

template<typename T>
uint32_t find_min_index(T* input_arr, uint32_t array_size, vector<uint32_t>& avoid_idx) {
   uint32_t start_idx = 0;
   bool start_idx_found = false;
   for (start_idx; start_idx < array_size; ++start_idx) {
      if (!is_existed(start_idx, avoid_idx)) {
         start_idx_found = true;
         break;
      }
   }
   if (!start_idx_found) {
      assert(false);
   }

   uint32_t min_idx = start_idx;
   T min_val        = input_arr[start_idx];
   for (uint32_t i=1; i < array_size; ++i) {
      if ((min_val > input_arr[i]) && !is_existed(i, avoid_idx)) {
         min_val = input_arr[i];
         min_idx = i;
      }
   }
   return min_idx; 
}

template<typename T>
uint32_t find_max_index(T* input_arr, uint32_t array_size, vector<uint32_t>& avoid_idx) {
   uint32_t start_idx = 0;
   bool start_idx_found = false;
   for (start_idx; start_idx < array_size; ++start_idx) {
      if (!is_existed(start_idx, avoid_idx)) {
         start_idx_found = true;
         break;
      }
   }
   if (!start_idx_found) {
      assert(false);
   }

   uint32_t max_idx = start_idx;
   T max_val        = input_arr[start_idx];
   for (uint32_t i=0; i < array_size; ++i) {
      if ((max_val < input_arr[i]) && !is_existed(i, avoid_idx)) {
         max_val = input_arr[i];
         max_idx = i;
      }
   }
   return max_idx; 
}

//========================================================================//
// TO STRING
//========================================================================//
/*
   convert array to string, values split by split_sym (split symbol)
*/
template<typename T>
string arr_to_string(T* array, uint32_t array_size, string split_sym=",") {
    string output = "";
    for (uint32_t i=0; i < array_size; ++i) {
        output += to_string(array[i]) + split_sym;
    }
    if (output != "")
        output.pop_back();
    return output;
}

/*
   convert vector to string, values split by split_sym (split symbol)
*/
template<typename T>
string vec_to_string(vector<T> &vec, string split_sym=",") {
    string output = "";
    for (uint32_t i=0; i < vec.size(); ++i) {
        output += to_string(vec[i]) + split_sym;
    }
    if (output != "")
        output.pop_back();
    return output;
}

#endif   // UTILS_H