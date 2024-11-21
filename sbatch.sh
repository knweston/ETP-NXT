# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
# python3 auto-slurm-1core.py -a nxetp-specmore -o entropy      -e entropy      -w 2 -r entropy      -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o entropypf    -e entropypf    -w 2 -r entropypf    -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o xorhash      -e xorhash      -w 2 -r xorhash      -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o xorhashpf    -e xorhashpf    -w 2 -r xorhashpf    -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o nohash       -e nohash       -w 2 -r nohash       -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o nohashpf     -e nohashpf     -w 2 -r nohashpf     -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o primeideal   -e primeideal   -w 2 -r primeideal   -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o primeidealpf -e primeidealpf -w 2 -r primeidealpf -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o skewed       -e skewed       -w 2 -r skewed       -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o skewedpf     -e skewedpf     -w 2 -r skewedpf     -t more-traces

# python3 auto-slurm-1core.py -a xsbench -o entropy      -e entropy      -w 2 -r xs-entropy      -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o entropypf    -e entropypf    -w 2 -r xs-entropypf    -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o xorhash      -e xorhash      -w 2 -r xs-xorhash      -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o xorhashpf    -e xorhashpf    -w 2 -r xs-xorhashpf    -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o nohash       -e nohash       -w 2 -r xs-nohash       -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o nohashpf     -e nohashpf     -w 2 -r xs-nohashpf     -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o primeideal   -e primeideal   -w 2 -r xs-primeideal   -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o primeidealpf -e primeidealpf -w 2 -r xs-primeidealpf -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o skewed       -e skewed       -w 2 -r xs-skewed       -t xsbm-traces
# python3 auto-slurm-1core.py -a xsbench -o skewedpf     -e skewedpf     -w 2 -r xs-skewedpf     -t xsbm-traces

# python3 auto-slurm-1core.py -a nxetp-bonus -o nohash       -e nohash       -w 2 -r nohash       -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o primeideal   -e primeideal   -w 2 -r primeideal   -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o xorhash      -e xorhash      -w 2 -r xorhash      -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o skewed       -e skewed       -w 2 -r skewed       -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o entropy      -e entropy      -w 2 -r entropy      -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o nohashpf     -e nohashpf     -w 2 -r nohashpf     -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o primeidealpf -e primeidealpf -w 2 -r primeidealpf -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o xorhashpf    -e xorhashpf    -w 2 -r xorhashpf    -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o skewedpf     -e skewedpf     -w 2 -r skewedpf     -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o entropypf    -e entropypf    -w 2 -r entropypf    -t dpc3-traces


# python3 auto-slurm-1core.py -a cvp1 -o cvp-nohash       -e nohash       -w 2 -r cvp-nohash       -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-nohashpf     -e nohashpf     -w 2 -r cvp-nohashpf     -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-entropy      -e entropy      -w 2 -r cvp-entropy      -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-entropypf    -e entropypf    -w 2 -r cvp-entropypf    -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-xorhash      -e xorhash      -w 2 -r cvp-xorhash      -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-xorhashpf    -e xorhashpf    -w 2 -r cvp-xorhashpf    -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-primeideal   -e primeideal   -w 2 -r cvp-primeideal   -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-primeidealpf -e primeidealpf -w 2 -r cvp-primeidealpf -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-skewed       -e skewed       -w 2 -r cvp-skewed       -t cvp1-traces
# python3 auto-slurm-1core.py -a cvp1 -o cvp-skewedpf     -e skewedpf     -w 2 -r cvp-skewedpf     -t cvp1-traces

python3 auto-slurm-1core.py -a gapmore -o gap-nohash       -e nohash       -w 2 -r gap-nohash       -t gaps-traces
python3 auto-slurm-1core.py -a gapmore -o gap-nohashpf     -e nohashpf     -w 2 -r gap-nohashpf     -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-entropy      -e entropy      -w 2 -r gap-entropy      -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-entropypf    -e entropypf    -w 2 -r gap-entropypf    -t gaps-traces
python3 auto-slurm-1core.py -a gapmore -o gap-xorhash      -e xorhash      -w 2 -r gap-xorhash      -t gaps-traces
python3 auto-slurm-1core.py -a gapmore -o gap-xorhashpf    -e xorhashpf    -w 2 -r gap-xorhashpf    -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-primeideal   -e primeideal   -w 2 -r gap-primeideal   -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-primeidealpf -e primeidealpf -w 2 -r gap-primeidealpf -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-skewed       -e skewed       -w 2 -r gap-skewed       -t gaps-traces
# python3 auto-slurm-1core.py -a gapmore -o gap-skewedpf     -e skewedpf     -w 2 -r gap-skewedpf     -t gaps-traces

# python3 auto-slurm-1core.py -a nxetp-specmore -o nohash -e nohash-dblru -w 2 -r nohash-dblru -t more-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o nohash -e nohash-dblru -w 2 -r nohash-dblru -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o xorhash -e xorhash-dblru -w 2 -r xorhash-dblru -t more-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o primeideal -e primeideal-dblru -w 2 -r primeideal-dblru -t more-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o xorhash -e xorhash-dblru -w 2 -r xorhash-dblru -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o primeideal -e primeideal-dblru -w 2 -r primeideal-dblru -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp-specmore -o etp3-dblru -e etp3-dblru -w 2 -r etp3-dblru -t more-traces
# python3 auto-slurm-1core.py -a nxetp-bonus -o etp3-dblru -e etp3-dblru -w 2 -r etp3-dblru -t dpc3-traces

# python3 auto-slurm-1core.py -a 470_605_649_bc.web-1627B_cc.web-24B_bodytrack_freqmine_streamcluster -o etp2t5 -e etp2t5 -w 2 -r etp2t5 -t dpc3-traces
# python3 auto-slurm-1core.py -a 470_605_649_bc.web-1627B_cc.web-24B_bodytrack_freqmine_streamcluster -o etp2t5pf -e etp2t5pf -w 2 -r etp2t5pf -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp -o etp2t5 -e etp2t5 -w 2 -r etp2t5 -t dpc3-traces
# python3 auto-slurm-1core.py -a nxetp -o etp2t5pf -e etp2t5pf -w 2 -r etp2t5pf -t dpc3-traces