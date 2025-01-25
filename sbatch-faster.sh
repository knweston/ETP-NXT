# python3 autoslurm-1core.py -a cvp1-extra -o cvp-nohashpf -e nohashpf -w 2 -r cvp-nohashpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-skewedpf -e skewedpf -w 2 -r cvp-skewedpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-xorhashpf -e xorhashpf -w 2 -r cvp-xorhashpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-entropypf -e entropypf -w 2 -r cvp-entropypf -t cvp1-traces

# python3 autoslurm-1core.py -a nxetp -o etp2b -e etp2b -w 2 -r etp2b -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2b -e etp2b -w 2 -r etp2b -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2b -e etp2b -w 2 -r etp2b -t more-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2b -e etp2b -w 2 -r gap-etp2b -t gaps-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-etp2b -e etp2b -w 2 -r cvp-etp2b -t cvp1-traces

python3 autoslurm-1core.py -a nxetp -o etp2r9a -e etp2r9a -w 2 -r etp2r9a -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o etp2r9a -e etp2r9a -w 2 -r etp2r9a -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o etp2r9a -e etp2r9a -w 2 -r etp2r9a -t more-traces

# python3 autoslurm-1core.py -a nxetp -o xorhashpfb -e xorhashpfb -w 2 -r xorhashpfb -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o xorhashpfb -e xorhashpfb -w 2 -r xorhashpfb -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o xorhashpfb -e xorhashpfb -w 2 -r xorhashpfb -t more-traces

python3 autoslurm-1core.py -a nxetp -o etp2r9apfn -e etp2r9apfn -w 2 -r etp2r9apfn -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o etp2r9apfn -e etp2r9apfn -w 2 -r etp2r9apfn -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o etp2r9apfn -e etp2r9apfn -w 2 -r etp2r9apfn -t more-traces

python3 autoslurm-1core.py -a nxetp -o etp2r9apfb -e etp2r9apfb -w 2 -r etp2r9apfb -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o etp2r9apfb -e etp2r9apfb -w 2 -r etp2r9apfb -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o etp2r9apfb -e etp2r9apfb -w 2 -r etp2r9apfb -t more-traces