# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst16 -e etp3tst16 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst16 -e etp3tst16 -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst15 -e etp3tst15 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst15 -e etp3tst15 -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst14 -e etp3tst14 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst14 -e etp3tst14 -w 2 -t more-traces

# python3 autoslurm-1core.py -a gap -o gap-etp3tst16 -e etp3tst16 -w 2 -r gap-etp3tst16 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3tst15 -e etp3tst15 -w 2 -r gap-etp3tst15 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3tst14 -e etp3tst14 -w 2 -r gap-etp3tst14 -t gaps-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3tst16 -e etp3tst16 -w 2  -r cste-etp3tst16 -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3tst15 -e etp3tst15 -w 2  -r cste-etp3tst15 -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3tst14 -e etp3tst14 -w 2  -r cste-etp3tst14 -t cste-traces


# python3 autoslurm-1core.py -a bodytrack_streamcluster -o etp3tst20 -e etp3tst20 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a bodytrack_streamcluster -o etp3tst20pf_dn_2s -e etp3tst20pf_dn_2s -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a bodytrack_streamcluster -o etp3tst21 -e etp3tst21 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a bodytrack_streamcluster -o etp3tst21pf_dn_2s -e etp3tst21pf_dn_2s -w 2 -t dpc3-traces
python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst21 -e etp3tst21 -w 2 -t dpc3-traces
python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst21 -e etp3tst21 -w 2 -t more-traces