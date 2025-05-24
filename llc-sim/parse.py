import os
import sys

def main():
    infile = open(sys.argv[1], "r")
    num_gen = 0
    for line in infile:
        if line.find("Top 15 candidates:") != -1:
            num_gen += 1
    last_gen = num_gen
    infile.close()

    infile = open(sys.argv[1], "r")
    num_gen = 0
    for line in infile:
        if num_gen == last_gen:
            print(sys.argv[1].split("-LLC")[0] + " = " + line.split(" [score")[0])
            break
        if line.find("Top 15 candidates:") != -1:
            num_gen += 1
    infile.close()
    
if __name__ == '__main__':
    main()