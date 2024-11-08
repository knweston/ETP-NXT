import sys

def readfile(filename):
    infile = open(filename, "r")
    bit_sel_arr = [[0 for i in range(0)] for j in range(15)]
    for line in infile:
        bit_array = line.rstrip().split(' = ')[-1].split(' ')
        # print(bit_array)
        for i in range(len(bit_array)-1):
            if i % 2 == 0:
                bit_sel_arr[int(bit_array[i])].append(int(bit_array[i+1]))
    
    # print()
    for item in bit_sel_arr:
        item.sort()
        # print(item)
    
    return bit_sel_arr
        
def parse_frequency(arr, freq=1):
    freq_arr_total = []
    for row in arr:
        row_freq = [0 for i in range(39)]
        for item in row:
            row_freq[item] += 1
        freq_arr_total.append(row_freq)
    
    filtered_freq_arr = []
    for row in freq_arr_total:
        freq_arr = []
        # print(row)
        for idx, item in enumerate(row):
            if item >= freq:
                freq_arr.append(idx)
        filtered_freq_arr.append(freq_arr)

    # print()
    for item in filtered_freq_arr:
        print(item)

def main():
    bit_sel_arr = readfile(sys.argv[-1])
    parse_frequency(bit_sel_arr,1)

if __name__ == "__main__":
    main()