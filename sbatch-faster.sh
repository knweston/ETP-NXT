# python3 autoslurm-1core.py -a gap -o gap-etp3k   -e etp3k -r gap-etp3k -w 2 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3kpf_db_2i   -e etp3kpf_db_2i -r gap-etp3kpf_db_2i -w 2 -t gaps-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-etp3kpf_db_2i   -e etp3kpf_db_2i -r cvpetp-etp3kpf_db_2i -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-etp3k   -e etp3k -r cvpetp-etp3k -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3k   -e etp3k -r cste-etp3k -w 2 -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3kpf_db_2i   -e etp3kpf_db_2i -r cste-etp3kpf_db_2i -w 2 -t cste-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp3k   -e etp3k -r xsbm-etp3k -w 2 -t xsbm-traces
# python3 autoslurm-1core.py -a xsbench -o xsbm-etp3kpf_db_2i   -e etp3kpf_db_2i -r xsbm-etp3kpf_db_2i -w 2 -t xsbm-traces

# python3 autoslurm-1core.py -a dpc3-simpoint -o genetic -e genetic -r genetic -w 2 -t dpc3-traces -f fidx_genetic_nopf2.ini
# python3 autoslurm-1core.py -a dpc3-specmore -o genetic -e genetic -r genetic -w 2 -t more-traces -f fidx_genetic_nopf2.ini
# python3 autoslurm-1core.py -a dpc3-simpoint -o geneticpf_db_2i -e geneticpf_db_2i -r geneticpf_db_2i -w 2 -t dpc3-traces -f fidx_genetic_pf2.ini
# python3 autoslurm-1core.py -a dpc3-specmore -o geneticpf_db_2i -e geneticpf_db_2i -r geneticpf_db_2i -w 2 -t more-traces -f fidx_genetic_pf2.ini

# python3 autoslurm-4core.py -a hpca26c4v2 -o c4nohashpf_db_2i -e c4nohashpf_db_2i -r c4nohashv2pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4xorhashpf_db_2i -e c4xorhashpf_db_2i -r c4xorhashv2pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4entropypf_db_2i -e c4entropypf_db_2i -r c4entropyv2pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4fxetppf_db_2i -e c4fxetppf_db_2i -r c4fxetpv2pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4etp3rpf_db_2i -e c4etp3rpf_db_2i -r c4etp3rv2pf_db_2i -w 2