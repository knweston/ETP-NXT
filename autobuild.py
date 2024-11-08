import sys
import os
from enum import Enum
import subprocess

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

# From left to right, the latency (in cycles) of:
# nohash, primeideal, ceaser, xorhash, genetic, entropy, skewed, primereal
LATENCY = [0,0,2,0,0,0,0,5]



def change_idx_in_file(idx_type):
   filepath    = "inc/cache_global.h"
   typeterm    = "#define LLC_INDEX_TYPE"
   latencyterm = "#define LLC_HASH_LATENCY"
   
   with open(filepath, 'r', encoding='utf-8') as file: 
      data = file.readlines()
   data[51] = typeterm + " " + str(idx_type.value) + "\n"
   data[52] = latencyterm + " " + str(LATENCY[idx_type.value]) + "\n"

   with open(filepath, 'w', encoding='utf-8') as file: 
      file.writelines(data) 

   file.close()



def build(idx_type, is_pf):
   idx_str = str(idx_type).split(".")[-1].lower()
   if is_pf:
      if idx_type == INDEX.SKEWED:
         cmd = "./build_champsim.sh bimodal no next_line spp_dev no skewed 1 " + idx_str + "pf"
      else:
         cmd = "./build_champsim.sh bimodal no next_line spp_dev no lru 1 " + idx_str + "pf"
   else:
      if idx_type == INDEX.SKEWED:
         cmd = "./build_champsim.sh bimodal no no no no skewed 1 " + idx_str
      else:
         cmd = "./build_champsim.sh bimodal no no no no lru 1 " + idx_str
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
   build(INDEX.NOHASH, is_pf=False)
   # build(INDEX.PRIMEIDEAL, is_pf=False)
   # build(INDEX.CEASER, is_pf=False)
   # build(INDEX.XORHASH, is_pf=False)
   # build(INDEX.ENTROPY, is_pf=False)
   # build(INDEX.SKEWED, is_pf=False)
   # build(INDEX.PRIMEREAL, is_pf=False)

   build(INDEX.NOHASH, is_pf=True)
   # build(INDEX.PRIMEIDEAL, is_pf=True)
   # build(INDEX.CEASER, is_pf=True)
   # build(INDEX.XORHASH, is_pf=True)
   # build(INDEX.ENTROPY, is_pf=True)
   # build(INDEX.SKEWED, is_pf=True)
   # build(INDEX.PRIMEREAL, is_pf=True)

   execute("nohash", "/Volumes/microSD/dpc3-traces/")

if __name__ == "__main__":
   main()
