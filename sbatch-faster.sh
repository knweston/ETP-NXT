# python3 autoslurm-1core.py -a nxetp-bonus -o etp3 -e etp3 -w 2 -r etp3 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3 -e etp3 -w 2 -r etp3 -t more-traces
# python3 autoslurm-1core.py -a nxetp -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t more-traces

# python3 autoslurm-1core.py -a nxetp -o entropyv2 -e entropyv2 -w 2 -r entropyv2 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o entropyv2 -e entropyv2 -w 2 -r entropyv2 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o entropyv2 -e entropyv2 -w 2 -r entropyv2 -t more-traces

# python3 autoslurm-1core.py -a gapmore -o gap-entropyv2 -e entropyv2 -w 2 -r gap-entropyv2 -t gaps-traces
python3 autoslurm-1core.py -a cvp1 -o cvp-entropyv2 -e entropyv2 -w 2 -r cvp-entropyv2 -t cvp1-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-entropyv2 -e entropyv2 -w 2 -r xsbm-entropyv2 -t xsbm-traces

# python3 autoslurm-1core.py -a nxetp -o drrip-entropy -e drrip-entropy -w 2 -r drrip-entropy -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp -o srrip-entropy -e srrip-entropy -w 2 -r srrip-entropy -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp -o ship-entropy -e ship-entropy -w 2 -r ship-entropy -t dpc3-traces