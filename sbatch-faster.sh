# python3 autoslurm-1core.py -a cvp1-extra -o cvp-nohashpf -e nohashpf -w 2 -r cvp-nohashpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-skewedpf -e skewedpf -w 2 -r cvp-skewedpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-xorhashpf -e xorhashpf -w 2 -r cvp-xorhashpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-entropypf -e entropypf -w 2 -r cvp-entropypf -t cvp1-traces

python3 autoslurm-1core.py -a nxetp -o entropy2pf -e entropy2pf -w 2 -r entropy2pf -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o entropy2pf -e entropy2pf -w 2 -r entropy2pf -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o entropy2pf -e entropy2pf -w 2 -r entropy2pf -t more-traces

# python3 autoslurm-1core.py -a gapmore -o gap-entropy2pf -e entropy2pf -w 2 -r gap-entropy2pf -t gaps-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-entropy2pf -e entropy2pf -w 2 -r cvp-entropy2pf -t cvp1-traces