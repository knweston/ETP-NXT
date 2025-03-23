# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
python3 autoslurm-1core.py -a dpc3-simpoint -o ptn -e ptn -w 2 -r ptn -t dpc3-traces
python3 autoslurm-1core.py -a dpc3-specmore -o ptn -e ptn -w 2 -r ptn -t more-traces
python3 autoslurm-1core.py -a dpc3-simpoint -o ptn-sen4 -e ptn-sen4 -w 2 -r ptn-sen4 -t dpc3-traces
python3 autoslurm-1core.py -a dpc3-specmore -o ptn-sen4 -e ptn-sen4 -w 2 -r ptn-sen4 -t more-traces
python3 autoslurm-1core.py -a dpc3-simpoint -o ptn-sen3 -e ptn-sen3 -w 2 -r ptn-sen3 -t dpc3-traces
python3 autoslurm-1core.py -a dpc3-specmore -o ptn-sen3 -e ptn-sen3 -w 2 -r ptn-sen3 -t more-traces