# python3 autoslurm-1core.py -a cvp1 -o cvp1-etp3r   -e etp3r -r cvp1-etp3r -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1 -o cvp1-etp3rpf_db_2i   -e etp3rpf_db_2i -r cvp1-etp3rpf_db_2i -w 2 -t cvp1-traces
python3 autoslurm-1core.py -a geoidx-hpca26 -o ref4cpf_db_2i -e ref4cpf_db_2i -r ref4cpf_db_2i -w 2 -s 250
python3 autoslurm-1core.py -a geoidx-hpca26 -o ref4c -e ref4c -r ref4c -w 2 -s 250

# python3 autoslurm-4core.py -a hpca26c4v2 -o c4nohash -e c4nohash -r c4nohashv2 -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4xorhash -e c4xorhash -r c4xorhashv2 -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4entropy -e c4entropy -r c4entropyv2 -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4fxetp -e c4fxetp -r c4fxetpv2 -w 2
# python3 autoslurm-4core.py -a hpca26c4v2 -o c4etp3r -e c4etp3r -r c4etp3rv2 -w 2