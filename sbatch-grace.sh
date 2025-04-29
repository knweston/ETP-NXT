# ================================================================================================================================================ #
# entropy-next
# ================================================================================================================================================ #
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3b7 -e etp3b7 -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3b7 -e etp3b7 -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3b7pf_dn_2s -e etp3b7pf_dn_2s -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3b7pf_dn_2s -e etp3b7pf_dn_2s -w 2 -t more-traces

# python3 autoslurm-1core.py -a gap -o gap-etp3b7 -e etp3b7 -w 2 -r gap-etp3b7 -t gaps-traces
# python3 autoslurm-1core.py -a gap -o gap-etp3b7pf_dn_2s -e etp3b7pf_dn_2s -w 2 -r gap-etp3b7pf_dn_2s -t gaps-traces

# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3a1fix -e etp3a1fix -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3a1fix -e etp3a1fix -w 2 -t more-traces
# python3 autoslurm-1core.py -a dpc3-simpoint -o etp3a1fixpf_dn_2s -e etp3a1fixpf_dn_2s -w 2 -t dpc3-traces
# python3 autoslurm-1core.py -a dpc3-specmore -o etp3a1fixpf_dn_2s -e etp3a1fixpf_dn_2s -w 2 -t more-traces

# python3 autoslurm-1core.py -a cvp1 -o cvp-nohash -e nohash -w 2 -r cvp-nohash -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-fis_etp -e fis_etp -w 2 -r cvp-fis_etp -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp-fis_etppf_dn_2s -e fis_etppf_dn_2s -w 2 -r cvp-fis_etppf_dn_2s -t cvp1-traces

# python3 autoslurm-profile.py -a gap -o profile -e 2w2rlru -t gaps-traces
# python3 autoslurm-profile.py -a dpc3-simpoint -o profile-pf -e 2w2rlrupf -t dpc3-traces
# python3 autoslurm-profile.py -a dpc3-specmore -o profile-pf -e 2w2rlrupf -t more-traces
# python3 autoslurm-profile.py -a gap           -o profile-pf -e 2w2rlrupf -t gaps-traces

# python3 autoslurm-1core.py -a cloud -o cste-nohash -e nohash -w 2 -r cste-nohash -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-xorhash -e xorhash -w 2  -r cste-xorhash -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-entropy -e entropy -w 2  -r cste-entropy -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-fxetpfix -e fxetpfix -w 2  -r cste-fxetpfix -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3b7m -e etp3b7m -w 2  -r cste-etp3b7m -t cste-traces

# python3 autoslurm-1core.py -a cloud -o cste-nohashpf_dn_2s -e nohashpf_dn_2s -w 2 -r cste-nohashpf_dn_2s -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-xorhashpf_dn_2s -e xorhashpf_dn_2s -w 2  -r cste-xorhashpf_dn_2s -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-entropypf_dn_2s -e entropypf_dn_2s -w 2  -r cste-entropypf_dn_2s -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-fxetpfixpf_dn_2s -e fxetpfixpf_dn_2s -w 2  -r cste-fxetpfixpf_dn_2s -t cste-traces
# python3 autoslurm-1core.py -a cloud -o cste-etp3b7mpf_dn_2s -e etp3b7mpf_dn_2s -w 2  -r cste-etp3b7mpf_dn_2s -t cste-traces