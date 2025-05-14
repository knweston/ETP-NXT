# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3tst16 -e etp3tst16 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3tst16 -e etp3tst16 -w 2 -t more-traces

# python3 autoslurm-1core.py -a cvp1 -o cvp-etp3b1 -e etp3b1 -r cvp-etp3b1 -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-etp3b1pf_dn_2s -e etp3b1pf_dn_2s -r cvp-etp3b1pf_dn_2s -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3b1 -e etp3b1 -w 2 -r gap-etp3b1 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3b1pf_dn_2s -e etp3b1pf_dn_2s -r gap-etp3b1pf_dn_2s -w 2 -t gaps-traces

# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-nohash -e nohash -r cvpetp-nohash -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-nohashpf_dn_2s -e nohashpf_dn_2s -r cvpetp-nohashpf_dn_2s -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-xorhash -e xorhash -r cvpetp-xorhash -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-xorhashpf_dn_2s -e xorhashpf_dn_2s -r cvpetp-xorhashpf_dn_2s -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-entropy -e entropy -r cvpetp-entropy -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp-entropy -o cvpetp-entropypf_dn_2s -e entropypf_dn_2s -r cvpetp-entropypf_dn_2s -w 2 -t cvp1-traces

# python3 autoslurm-1core.py -a gap -o gap-etp3b1 -e etp3b1 -w 2 -r gap-etp3b1 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3b1pf_dn_2s -e etp3b1pf_dn_2s -r gap-etp3b1pf_dn_2s -w 2 -t gaps-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3b1 -e etp3b1 -w 2  -r cste-etp3b1 -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3b1pf_dn_2s -e etp3b1pf_dn_2s -w 2  -r cste-etp3b1pf_dn_2s -t cste-traces

# python3 autoslurm-1core.py -a cvp1 -o cvp-fxetp -e fxetp -r cvp-fxetp -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o fxetp -e fxetp -r fxetp -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o fxetp -e fxetp -r fxetp -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o entropy -e entropy -r entropy -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o entropy -e entropy -r entropy -w 2 -t more-traces




# python3 autoslurm-1core.py -a dpc3-simpoint -o vaxorhash -e vaxorhash -r vaxorhash -w 2 -t dpc3-traces -l 25
# python3 autoslurm-1core.py -a dpc3-specmore -o vaxorhash -e vaxorhash -r vaxorhash -w 2 -t more-traces -l 25

# python3 autoslurm-1core.py -a dpc3-simpoint -o vaetp3b7 -e vaetp3b7 -r vaetp3b7 -w 2 -t dpc3-traces -l 25
# python3 autoslurm-1core.py -a dpc3-specmore -o vaetp3b7 -e vaetp3b7 -r vaetp3b7 -w 2 -t more-traces -l 25
# python3 autoslurm-1core.py -a dpc3-simpoint -o vaprime -e vaprime -r vaprime -w 2 -t dpc3-traces -l 25
# python3 autoslurm-1core.py -a dpc3-specmore -o vaprime -e vaprime -r vaprime -w 2 -t more-traces -l 25
# python3 autoslurm-1core.py -a dpc3-specmore -o vaprimepf_dn_2s -e vaprimepf_dn_2s -r vaprimepf_dn_2s -w 2 -t more-traces -l 25
# python3 autoslurm-1core.py -a dpc3-simpoint -o vaprimepf_dn_2s -e vaprimepf_dn_2s -r vaprimepf_dn_2s -w 2 -t dpc3-traces -l 25

# python3 autoslurm-1core.py -a dpc3-simpoint -o vanohash -e vanohash -r vanohash -w 2 -t dpc3-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vanohash -e vanohash -r vanohash -w 2 -t more-traces -l 35
# python3 autoslurm-1core.py -a dpc3-simpoint -o vaetp -e vaetp -r vaetp -w 2 -t dpc3-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vaetp -e vaetp -r vaetp -w 2 -t more-traces -l 35
# python3 autoslurm-1core.py -a dpc3-simpoint -o vafxetp -e vafxetp -r vafxetp -w 2 -t dpc3-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vafxetp -e vafxetp -r vafxetp -w 2 -t more-traces -l 35
# python3 autoslurm-1core.py -a dpc3-simpoint -o vaetp3b1 -e vaetp3b1 -r vaetp3b1 -w 2 -t dpc3-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vaetp3b1 -e vaetp3b1 -r vaetp3b1 -w 2 -t more-traces -l 35
# python3 autoslurm-1core.py -a dpc3-simpoint -o vanohashpf_dn_2s -e vanohashpf_dn_2s -r vanohashpf_dn_2s -w 2 -t dpc3-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vanohashpf_dn_2s -e vanohashpf_dn_2s -r vanohashpf_dn_2s -w 2 -t more-traces -l 35
# python3 autoslurm-1core.py -a dpc3-specmore -o vafxetppf_dn_2s -e vafxetppf_dn_2s -r vafxetppf_dn_2s -w 2 -t more-traces -l 35