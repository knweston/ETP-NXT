# python3 autobuild.py -i nohash -p true -o nohash
# python3 autobuild.py -i primeideal -p true -o primeideal
# python3 autobuild.py -i xorhash -p true -o xorhash
# python3 autobuild.py -i skewed -p true -o skewed
# python3 autobuild.py -i entropy -p true -o entropy
python3 autobuild.py -i entropyv2 -p next_line -o etp2

# python3 autobuild.py -i nohash -p false -o nohash
# python3 autobuild.py -i primeideal -p false -o primeideal
# python3 autobuild.py -i xorhash -p false -o xorhash
# python3 autobuild.py -i skewed -p false -o skewed
# python3 autobuild.py -i entropyv2 -p false -o entropyv2
# python3 autobuild.py -i entropy -p false -o entropy

# python3 autobuild.py -i nohash -p false -r drrip -o drrip-nohash
# python3 autobuild.py -i xorhash -p false -r drrip -o drrip-xorhash
# python3 autobuild.py -i entropy -p false -r drrip -o drrip-entropy
# python3 autobuild.py -i entropy -p false -r srrip -o srrip-entropy
# python3 autobuild.py -i entropy -p false -r ship -o ship-entropy