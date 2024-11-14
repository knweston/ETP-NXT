# python3 autobuild.py -i nohash -p true -f nohash
# python3 autobuild.py -i primeideal -p true -f primeideal
# python3 autobuild.py -i xorhash -p true -f xorhash
# python3 autobuild.py -i skewed -p true -f skewed
# python3 autobuild.py -i entropy -p true -f entropy

# python3 autobuild.py -i nohash -p false -f nohash
# python3 autobuild.py -i primeideal -p false -f primeideal
# python3 autobuild.py -i xorhash -p false -f xorhash
# python3 autobuild.py -i skewed -p false -f skewed
# python3 autobuild.py -i entropy -p false -f entropy

python3 auto-slurm-1core.py -a cvp1 -o cvp-primeideal   -e primeideal   -w 2 -r cvp-primeideal -t cvp1-traces
python3 auto-slurm-1core.py -a cvp1 -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-skewed    -e skewed    -w 2 -r cvp-skewed -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-skewedpf  -e skewedpf  -w 2 -r cvp-skewedpf -t cvp1-traces