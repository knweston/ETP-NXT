# python3 autoslurm-1core.py -a cvp1 -o cvp-nohash -e nohash -w 2 -r cvp-nohash -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-xorhash -e xorhash -w 2 -r cvp-xorhash -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-buddy3b -e buddy3b -w 2 -r cvp-buddy3b -t cvp1-traces

python3 autoslurm-1core.py -a nxetp -o buddy3d1 -e buddy3d1 -w 2 -r buddy3d1 -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-bonus -o buddy3d1 -e buddy3d1 -w 2 -r buddy3d1 -t dpc3-traces
python3 autoslurm-1core.py -a nxetp-specmore -o buddy3d1 -e buddy3d1 -w 2 -r buddy3d1 -t more-traces
python3 autoslurm-1core.py -a cvp1 -o cvp-buddy3d1 -e buddy3d1 -w 2 -r cvp-buddy3d1 -t cvp1-traces