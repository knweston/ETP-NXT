# python3 autoslurm-1core.py -a cvp1-extra -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-skewedpf -e skewedpf -w 2 -r cvp-skewedpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-extra -o cvp-r2-etp2r12pf -e r2-etp2r12pf -w 2 -r cvp-r2-etp2r12pf -t cvp1-traces

# python3 autoslurm-1core.py -a nxetp -o etp2b -e etp2b -w 2 -r etp2b -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2b -e etp2b -w 2 -r etp2b -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2b -e etp2b -w 2 -r etp2b -t more-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2b -e etp2b -w 2 -r gap-etp2b -t gaps-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-etp2b -e etp2b -w 2 -r cvp-etp2b -t cvp1-traces

python3 autoslurm-1core.py -a nxetp -o skewed -e skewed -w 2 -r skewed -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o skewed -e skewed -w 2 -r skewed -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o skewed -e skewed -w 2 -r skewed -t more-traces

python3 autoslurm-1core.py -a nxetp -o skewedpfn -e skewedpfn -w 2 -r skewedpfn -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o skewedpfn -e skewedpfn -w 2 -r skewedpfn -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o skewedpfn -e skewedpfn -w 2 -r skewedpfn -t more-traces

python3 autoslurm-1core.py -a nxetp -o skewedpfb -e skewedpfb -w 2 -r skewedpfb -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o skewedpfb -e skewedpfb -w 2 -r skewedpfb -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o skewedpfb -e skewedpfb -w 2 -r skewedpfb -t more-traces


# ======================================================================================================================== #
# python3 autoslurm-1core.py -a nxetp -o etp2r17 -e etp2r17 -w 2 -r etp2r17 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2r17 -e etp2r17 -w 2 -r etp2r17 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2r17 -e etp2r17 -w 2 -r etp2r17 -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp2r17pfn -e etp2r17pfn -w 2 -r etp2r17pfn -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2r17pfn -e etp2r17pfn -w 2 -r etp2r17pfn -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2r17pfn -e etp2r17pfn -w 2 -r etp2r17pfn -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp2r17pfb -e etp2r17pfb -w 2 -r etp2r17pfb -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2r17pfb -e etp2r17pfb -w 2 -r etp2r17pfb -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2r17pfb -e etp2r17pfb -w 2 -r etp2r17pfb -t more-traces

# python3 autoslurm-1core.py -a gapmore -o gap-etp2r17 -e etp2r17 -w 2 -r gap-etp2r17 -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2r17pfn -e etp2r17pfn -w 2 -r gap-etp2r17pfn -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2r17pfb -e etp2r17pfb -w 2 -r gap-etp2r17pfb -t gaps-traces
# ======================================================================================================================== #
# python3 autoslurm-1core.py -a gapmore -o gap-skewed -e skewed -w 2 -r gap-skewed -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-primeideal -e primeideal -w 2 -r gap-primeideal -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-entropy -e entropy -w 2 -r gap-entropy -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-xorhash -e xorhash -w 2 -r gap-xorhash -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-nohash -e nohash -w 2 -r gap-nohash -t gaps-traces

# python3 autoslurm-1core.py -a gapmore -o gap-skewedpfn -e skewedpfn -w 2 -r gap-skewedpfn -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-primeidealpfn -e primeidealpfn -w 2 -r gap-primeidealpfn -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-entropypfn -e entropypfn -w 2 -r gap-entropypfn -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-xorhashpfn -e xorhashpfn -w 2 -r gap-xorhashpfn -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-nohashpfn -e nohashpfn -w 2 -r gap-nohashpfn -t gaps-traces

# python3 autoslurm-1core.py -a gapmore -o gap-skewedpfb -e skewedpfb -w 2 -r gap-skewedpfb -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-primeidealpfb -e primeidealpfb -w 2 -r gap-primeidealpfb -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-entropypfb -e entropypfb -w 2 -r gap-entropypfb -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-xorhashpfb -e xorhashpfb -w 2 -r gap-xorhashpfb -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-nohashpfb -e nohashpfb -w 2 -r gap-nohashpfb -t gaps-traces
# ======================================================================================================================== #