# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
python3 autoslurm-1core.py -a gapmore -o gap-entropyv2 -e entropyv2 -w 2 -r gap-entropyv2 -t gaps-traces
python3 autoslurm-1core.py -a gapmore -o gap-etp2a -e etp2a -w 2 -r gap-etp2a -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2r1 -e etp2r1 -w 2 -r gap-etp2r1 -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o gap-etp2r1a -e etp2r1a -w 2 -r gap-etp2r1a -t gaps-traces

# python3 autoslurm-1core.py -a xsbench -o xsbm-entropyv2 -e entropyv2 -w 2 -r xsbm-entropyv2 -t xsbm-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp2a -e etp2a -w 2 -r xsbm-etp2a -t xsbm-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp2r1 -e etp2r1 -w 2 -r xsbm-etp2r1 -t xsbm-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp2r1a -e etp2r1a -w 2 -r xsbm-etp2r1a -t xsbm-traces