# python3 autoslurm-1core.py -a nxetp -o etp2a -e etp2a -w 2 -r etp2a -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp2a -e etp2a -w 2 -r etp2a -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp2a -e etp2a -w 2 -r etp2a -t more-traces

# python3 autoslurm-1core.py -a gapmore -o gap-etp2a -e etp2a -w 2 -r gap-etp2a -t gaps-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-etp2a -e etp2a -w 2 -r cvp-etp2a -t cvp1-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp2a -e etp2a -w 2 -r xsbm-etp2a -t xsbm-traces

# python3 autoslurm-1core.py -a nxetp -o drrip-entropy -e drrip-entropy -w 2 -r drrip-entropy -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp -o srrip-entropy -e srrip-entropy -w 2 -r srrip-entropy -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp -o ship-entropy -e ship-entropy -w 2 -r ship-entropy -t dpc3-traces

python3 autoslurm-1core.py -a cvp1-extra -o cvp-nohash -e nohash -w 2 -r cvpextra-nohash -t cvp1-traces
python3 autoslurm-1core.py -a cvp1-extra -o cvp-xorhash -e xorhash -w 2 -r cvpextra-xorhash -t cvp1-traces
python3 autoslurm-1core.py -a cvp1-extra -o cvp-etp2a -e etp2a -w 2 -r cvpextra-etp2a -t cvp1-traces
python3 autoslurm-1core.py -a cvp1-extra -o cvp-entropyv2 -e entropyv2 -w 2 -r cvpextra-entropyv2 -t cvp1-traces
python3 autoslurm-1core.py -a cvp1-extra -o cvp-entropy -e entropy -w 2 -r cvpextra-entropy -t cvp1-traces