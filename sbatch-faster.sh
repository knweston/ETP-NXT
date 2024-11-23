# python3 autoslurm-1core.py -a nxetp-bonus -o etp3 -e etp3 -w 2 -r etp3 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3 -e etp3 -w 2 -r etp3 -t more-traces
# python3 autoslurm-1core.py -a nxetp -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3-500 -e etp3-500 -w 2 -r etp3-500 -t more-traces

python3 autoslurm-1core.py -a nxetp -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o etp3b-500-dblru -e etp3b-500-dblru -w 2 -r etp3b-500-dblru -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3a-500-dblru -e etp3a-500-dblru -w 2 -r etp3a-500-dblru -t more-traces

# python3 autoslurm-1core.py -a nxetp -o etp3a-500 -e etp3a-500 -w 2 -r etp3a-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-bonus -o etp3a-500 -e etp3a-500 -w 2 -r etp3a-500 -t dpc3-traces
# python3 autoslurm-1core.py -a nxetp-specmore -o etp3a-500 -e etp3a-500 -w 2 -r etp3a-500 -t more-traces