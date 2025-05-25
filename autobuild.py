import sys
import os
from enum import Enum
import subprocess
import argparse

# Indexing types supported
class INDEX(Enum):
   NOHASH      = 0
   PRIMEIDEAL  = 1
   CEASER      = 2
   XORHASH     = 3
   GENETIC     = 4
   ENTROPY     = 5
   SKEWED      = 6
   PRIMEREAL   = 7
   ENTROPYV2   = 8
   ENTROPYV3   = 9

def parse_index_type(str_type):
   if str_type == "nohash":
      return INDEX.NOHASH
   elif str_type == "primeideal":
      return INDEX.PRIMEIDEAL
   elif str_type == "ceaser":
      return INDEX.CEASER
   elif str_type == "xorhash":
      return INDEX.XORHASH
   elif str_type == "genetic":
      return INDEX.GENETIC
   elif str_type == "entropy":
      return INDEX.ENTROPY
   elif str_type == "skewed":
      return INDEX.SKEWED
   elif str_type == "primereal":
      return INDEX.PRIMEREAL
   elif str_type == "entropyv2":
      return INDEX.ENTROPYV2
   elif str_type == "entropyv3":
      return INDEX.ENTROPYV3
   else:
      print("Index not found: ", str_type)

# From left to right, the latency (in cycles) of:
# nohash, primeideal, ceaser, xorhash, genetic, entropy, skewed, primereal
LATENCY = [0,0,2,0,0,0,0,5,0,0]



def change_idx_in_file(idx_type):
   filepath    = "inc/cache_global.h"
   typeterm    = "#define LLC_INDEX_TYPE"
   latencyterm = "#define LLC_HASH_LATENCY"
   
   with open(filepath, 'r', encoding='utf-8') as file: 
      data = file.readlines()
   
   if idx_type == INDEX.PRIMEREAL:
      data[51] = typeterm + " " + str(INDEX.PRIMEIDEAL.value) + "\n"
   else:
      data[51] = typeterm + " " + str(idx_type.value) + "\n"

   data[52] = latencyterm + " " + str(LATENCY[idx_type.value]) + "\n"

   with open(filepath, 'w', encoding='utf-8') as file: 
      file.writelines(data) 

   file.close()



def build(idx_type, l1d_prefetcher, l1i_prefetcher, l2_prefetcher, filename, repl_policy, num_core):
   idx_str = str(idx_type).split(".")[-1].lower()

   if (filename == ""):
      exec_name = idx_str
   else:
      exec_name = filename

   cmd = "./build_champsim.sh bimodal " + l1i_prefetcher + " " + l1d_prefetcher + " " + l2_prefetcher + " no "
   if idx_type == INDEX.SKEWED:
      cmd += "skewed " + num_core + " "
   else:
      cmd += repl_policy + " " + num_core + " "

   if int(num_core) > 1:
      cmd += "c" + num_core
   cmd += exec_name

   if l1d_prefetcher != "no" or l1i_prefetcher != "no" or l2_prefetcher != "no":
      cmd += "pf"
      if l1d_prefetcher != "no":
         cmd += "_d" + l1d_prefetcher[0]
      if l1i_prefetcher != "no":
         cmd += "_i" + l1i_prefetcher[0]
      if l2_prefetcher != "no":
         cmd += "_2" + l2_prefetcher[0]

   change_idx_in_file(idx_type)
   os.system(cmd)



def execute(exec_file, traces_dir, result_dir=""):
   cmd = "./run_champsim.sh bin/" + exec_file + " 2 1000 none " + traces_dir
   traces_list = (subprocess.check_output(("ls " + traces_dir), shell=True, text=True)).split("\n")
   cmd += traces_list[1]

   if result_dir != "":
      cmd += " " + result_dir
   else:
      cmd += " " + exec_file

   print(cmd)
   os.system(cmd)


def main():
   parser = argparse.ArgumentParser()
   parser.add_argument("-i", "--idx_type", required=True, type=str)
   parser.add_argument("-r", "--replacement_type", required=False, default="lru", type=str)
   parser.add_argument("-o", "--output_filename", required=False, default="", type=str)
   parser.add_argument("-p", "--l1d_prefetcher", required=False, default="no", type=str)
   parser.add_argument("-f", "--l1i_prefetcher", required=False, default="no", type=str)
   parser.add_argument("-d", "--l2_prefetcher", required=False, default="no", type=str)
   parser.add_argument("-c", "--num_core", required=False, default="1", type=str)
   args = parser.parse_args()
   
   build(parse_index_type(args.idx_type), args.l1d_prefetcher, args.l1i_prefetcher, args.l2_prefetcher, args.output_filename, args.replacement_type, args.num_core)

if __name__ == "__main__":
   main()
