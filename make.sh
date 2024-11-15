# python3 autobuild.py -i nohash -p true -f nohash
# python3 autobuild.py -i primeideal -p true -f primeideal
# python3 autobuild.py -i xorhash -p true -f xorhash
# python3 autobuild.py -i skewed -p true -f skewed
# python3 autobuild.py -i entropy -p true -f entropy

# python3 autobuild.py -i nohash -p false -f nohash
# python3 autobuild.py -i primeideal -p false -f primeideal
# python3 autobuild.py -i xorhash -p false -f xorhash
# python3 autobuild.py -i skewed -p false -f skewed
# python3 autobuild.py -i entropy -p false -f entropy

python3 autobuild.py -i nohash -p false -f nohash-dblru -r dblru