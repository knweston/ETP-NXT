# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst19pf_dn_2s -e etp3tst19pf_dn_2s -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst19pf_dn_2s -e etp3tst19pf_dn_2s -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst15pf_dn_2s -e etp3tst15pf_dn_2s -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst15pf_dn_2s -e etp3tst15pf_dn_2s -w 2 -t more-traces

# python3 autoslurm-1core.py -a gap -o gap-etp3tst19pf_dn_2s -e etp3tst19pf_dn_2s -w 2 -r gap-etp3tst19pf_dn_2s -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3tst15pf_dn_2s -e etp3tst15pf_dn_2s -w 2 -r gap-etp3tst15pf_dn_2s -t gaps-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3tst19pf_dn_2s -e etp3tst19pf_dn_2s -w 2 -r cste-etp3tst19pf_dn_2s -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3tst15pf_dn_2s -e etp3tst15pf_dn_2s -w 2 -r cste-etp3tst15pf_dn_2s -t cste-traces

python3 autoslurm-1core.py -a dpc3-simpoint -o geneticpf_dn_2s -e geneticpf_dn_2s -w 2 -t dpc3-traces -f fixed_idx_genetic_pf.ini
python3 autoslurm-1core.py -a dpc3-specmore -o geneticpf_dn_2s -e geneticpf_dn_2s -w 2 -t more-traces -f fixed_idx_genetic_pf.ini