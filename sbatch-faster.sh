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

# python3 autoslurm-4core.py -a hpca26c4v20 -o c4entropy -e c4entropy -r c4entropyv20 -w 2
# python3 autoslurm-4core.py -a hpca26c4v20 -o c4nohashpf_db_2i -e c4nohashpf_db_2i -r c4nohashv20pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v20 -o c4xorhashpf_db_2i -e c4xorhashpf_db_2i -r c4xorhashv20pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v20 -o c4entropypf_db_2i -e c4entropypf_db_2i -r c4entropyv20pf_db_2i -w 2
# python3 autoslurm-4core.py -a hpca26c4v20 -o c4entropypf_db_2i -e c4entropypf_db_2i -r c4entropyv20pf_db_2i -w 2



# python3 autoslurm-4core.py -a hpca26c4v20 -o c4fxetppf_db_2i -e c4fxetppf_db_2i -r c4fxetpv20pf_db_2i -w 2

# python3 autoslurm-1core.py -a cvp1-hpca26c4 -o ref4cnohash -e ref4cnohash -r ref4cnohash -w 2 -s 250 -t cvp1-traces
# python3 autoslurm-1core.py -a cvp1-hpca26c4 -o ref4cnohashpf_db_2i -e ref4cnohashpf_db_2i -r ref4cnohashpf_db_2i -w 2 -s 250 -t cvp1-traces

# python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3r -e etp3r -r ml-etp3r -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3rpf_db_2i -e etp3rpf_db_2i -r ml-etp3rpf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-primereal -e primereal -r ml-primereal -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-primerealpf_db_2i -e primerealpf_db_2i -r ml-primerealpf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-entropy1c -e entropy1c -r ml-entropy1c -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-entropy1cpf_db_2i -e entropy1cpf_db_2i -r ml-entropy1cpf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-nohash -e nohash -r ml-nohash -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-nohashpf_db_2i -e nohashpf_db_2i -r ml-nohashpf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-xorhash -e xorhash -r ml-xorhash -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-xorhashpf_db_2i -e xorhashpf_db_2i -r ml-xorhashpf_db_2i -w 2 -t dpc4-traces

# python3 autoslurm-1core.py -a parsec -o parsec-etp3r -e etp3r -r parsec-etp3r -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-etp3rpf_db_2i -e etp3rpf_db_2i -r parsec-etp3rpf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-prime -e prime -r parsec-prime -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-primepf_db_2i -e primepf_db_2i -r parsec-primepf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-entropy -e entropy -r parsec-entropy -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-entropypf_db_2i -e entropypf_db_2i -r parsec-entropypf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-nohash -e nohash -r parsec-nohash -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-nohashpf_db_2i -e nohashpf_db_2i -r parsec-nohashpf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-xorhash -e xorhash -r parsec-xorhash -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-xorhashpf_db_2i -e xorhashpf_db_2i -r parsec-xorhashpf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-primereal -e primereal -r parsec-primereal -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-primerealpf_db_2i -e primerealpf_db_2i -r parsec-primerealpf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-entropy1c -e entropy1c -r parsec-entropy1c -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-entropy1cpf_db_2i -e entropy1cpf_db_2i -r parsec-entropy1cpf_db_2i -w 2

# python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-etp3r_rmpmax1trn0 -e etp3r_rmpmax1trn0 -r dpc3-etp3r_rmpmax1trn0 -w 2
# python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-etp3r_rmpmax1trn0 -e etp3r_rmpmax1trn0 -r dpc3-etp3r_rmpmax1trn0 -w 2
# python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-etp3r_rmpmax1trn0pf_db_2i -e etp3r_rmpmax1trn0pf_db_2i -r dpc3-etp3r_rmpmax1trn0pf_db_2i -w 2
# python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-etp3r_rmpmax1trn0pf_db_2i -e etp3r_rmpmax1trn0pf_db_2i -r dpc3-etp3r_rmpmax1trn0pf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-etp3r_rmpmax1trn0 -e etp3r_rmpmax1trn0 -r parsec-etp3r_rmpmax1trn0 -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-etp3r_rmpmax1trn0pf_db_2i -e etp3r_rmpmax1trn0pf_db_2i -r parsec-etp3r_rmpmax1trn0pf_db_2i -w 2
# python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3r_rmpmax1trn0 -e etp3r_rmpmax1trn0 -r ml-etp3r_rmpmax1trn0 -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3r_rmpmax1trn0pf_db_2i -e etp3r_rmpmax1trn0pf_db_2i -r ml-etp3r_rmpmax1trn0pf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-etp3r_rmpmax1trn0 -e etp3r_rmpmax1trn0 -r cvp1-etp3r_rmpmax1trn0 -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-etp3r_rmpmax1trn0pf_db_2i -e etp3r_rmpmax1trn0pf_db_2i -r cvp1-etp3r_rmpmax1trn0pf_db_2i -w 2 -t cvp1-traces

# python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-skewedh1 -e skewedh1 -r dpc3-skewedh1 -w 2
# python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-skewedh1 -e skewedh1 -r dpc3-skewedh1 -w 2
# python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-skewedh1pf_db_2i -e skewedh1pf_db_2i -r dpc3-skewedh1pf_db_2i -w 2
# python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-skewedh1pf_db_2i -e skewedh1pf_db_2i -r dpc3-skewedh1pf_db_2i -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-skewedh1 -e skewedh1 -r parsec-skewedh1 -w 2
# python3 autoslurm-1core.py -a parsec -o parsec-skewedh1pf_db_2i -e skewedh1pf_db_2i -r parsec-skewedh1pf_db_2i -w 2
# python3 autoslurm-1core.py -a dpc4-ml -o ml-skewedh1 -e skewedh1 -r ml-skewedh1 -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a dpc4-ml -o ml-skewedh1pf_db_2i -e skewedh1pf_db_2i -r ml-skewedh1pf_db_2i -w 2 -t dpc4-traces
# python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-skewedh1 -e skewedh1 -r cvp1-skewedh1 -w 2 -t cvp1-traces
# python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-skewedh1pf_db_2i -e skewedh1pf_db_2i -r cvp1-skewedh1pf_db_2i -w 2 -t cvp1-traces

python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-etp3r_trn1 -e etp3r_trn1 -r dpc3-etp3r_trn1 -w 2
python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-etp3r_trn1 -e etp3r_trn1 -r dpc3-etp3r_trn1 -w 2
python3 autoslurm-1core.py -a dpc3-simpoint -o dpc3-etp3r_trn1pf_db_2i -e etp3r_trn1pf_db_2i -r dpc3-etp3r_trn1pf_db_2i -w 2
python3 autoslurm-1core.py -a dpc3-specmore -o dpc3-etp3r_trn1pf_db_2i -e etp3r_trn1pf_db_2i -r dpc3-etp3r_trn1pf_db_2i -w 2
python3 autoslurm-1core.py -a parsec -o parsec-etp3r_trn1 -e etp3r_trn1 -r parsec-etp3r_trn1 -w 2
python3 autoslurm-1core.py -a parsec -o parsec-etp3r_trn1pf_db_2i -e etp3r_trn1pf_db_2i -r parsec-etp3r_trn1pf_db_2i -w 2
python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3r_trn1 -e etp3r_trn1 -r ml-etp3r_trn1 -w 2 -t dpc4-traces
python3 autoslurm-1core.py -a dpc4-ml -o ml-etp3r_trn1pf_db_2i -e etp3r_trn1pf_db_2i -r ml-etp3r_trn1pf_db_2i -w 2 -t dpc4-traces
python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-etp3r_trn1 -e etp3r_trn1 -r cvp1-etp3r_trn1 -w 2 -t cvp1-traces
python3 autoslurm-1core.py -a geoidx-cvp1 -o cvp1-etp3r_trn1pf_db_2i -e etp3r_trn1pf_db_2i -r cvp1-etp3r_trn1pf_db_2i -w 2 -t cvp1-traces