# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
python3 autoslurm-1core.py -a cvp1 -o cvp-entropyv2 -e entropyv2 -w 2 -r cvp-entropyv2 -t cvp1-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-entropyv2 -e entropyv2 -w 2 -r xsbm-entropyv2 -t xsbm-traces