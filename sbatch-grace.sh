# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
# python3 autoslurm-1core.py -a xsbench -o nohash -e nohash -w 2 -r xsbm-nohash -t xsbm-traces
# python3 autoslurm-1core.py -a xsbench -o xorhash -e xorhash -w 2 -r xsbm-xorhash -t xsbm-traces
python3 autoslurm-1core.py -a xsbench -o buddy3d1 -e buddy3d1 -w 2 -r xsbm-buddy3d1 -t xsbm-traces

# python3 autoslurm-1core.py -a gapmore -o nohash -e nohash -w 2 -r gaps-nohash -t gaps-traces
# python3 autoslurm-1core.py -a gapmore -o xorhash -e xorhash -w 2 -r gaps-xorhash -t gaps-traces
python3 autoslurm-1core.py -a gapmore -o buddy3d1 -e buddy3d1 -w 2 -r gaps-buddy3d1 -t gaps-traces