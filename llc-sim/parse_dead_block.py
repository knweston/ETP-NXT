import sys

def main():
   print("Processing trace: ", sys.argv[1])
   trace = open(sys.argv[1], "r")
   dead_block_list = []
   for line in trace:
      address = int(line.split(" ")[0],16)
      num_access = int(line.split(" ")[1].strip())
      if num_access == 1:
         dead_block_list.append(address)
   trace.close()
   
   output_filename = sys.argv[1].split("FBI-TRACE.txt")[0] + "DEAD-BLKS.txt"
   outfile = open(output_filename, "w")
   for addr in dead_block_list:
      outfile.write(hex(addr)[2:]+"\n")
   outfile.close()

if __name__ == "__main__":
   main()