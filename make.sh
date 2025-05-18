# python3 autobuild.py -i nohash -r srrip -o adj09_nohash
# python3 autobuild.py -i xorhash -r srrip -r srrip -o adj09_xorhash
# python3 autobuild.py -i entropy -r srrip -o adj09_etp
# python3 autobuild.py -i entropyv3 -r srrip -o adj09_etp3b

# python3 autobuild.py -i nohash -r srrip -p berti -d ip_stride -o adj09_nohash
# python3 autobuild.py -i xorhash -r srrip -p berti -d ip_stride -o adj09_xorhash
# python3 autobuild.py -i entropy -r srrip -p berti -d ip_stride -o adj09_etp
# python3 autobuild.py -i entropyv3 -r srrip -p berti -d ip_stride -o adj09_etp3b

# python3 autobuild.py -i nohash -r srrip -p next_line -d ip_stride -o adj09_nohash
# python3 autobuild.py -i xorhash -r srrip -p next_line -d ip_stride -o adj09_xorhash
# python3 autobuild.py -i entropy -r srrip -p next_line -d ip_stride -o adj09_etp
# python3 autobuild.py -i entropyv3 -r srrip -p next_line -d ip_stride -o adj09_etp3b

# python3 autobuild.py -i nohash -r srrip -p berti -d spp_dev -o adj09_nohash
# python3 autobuild.py -i xorhash -r srrip -p berti -d spp_dev -o adj09_xorhash
# python3 autobuild.py -i entropy -r srrip -p berti -d spp_dev -o adj09_etp
# python3 autobuild.py -i entropyv3 -r srrip -p berti -d spp_dev -o adj09_etp3b

# python3 autobuild.py -i nohash -r srrip -p next_line -d spp_dev -o adj09_nohash
# python3 autobuild.py -i xorhash -r srrip -p next_line -d spp_dev -o adj09_xorhash
# python3 autobuild.py -i entropy -r srrip -p next_line -d spp_dev -o adj09_etp
# python3 autobuild.py -i entropyv3 -r srrip -p next_line -d spp_dev -o adj09_etp3b

python3 autobuild.py -i entropy -r srrip -o adj09_fxetp
python3 autobuild.py -i entropy -r srrip -p berti -d ip_stride -o adj09_fxetp
# python3 autobuild.py -i entropy -r srrip -p next_line -d ip_stride -o adj09_fxetp
# python3 autobuild.py -i entropy -r srrip -p berti -d spp_dev -o adj09_fxetp
# python3 autobuild.py -i entropy -r srrip -p next_line -d spp_dev -o adj09_fxetp

python3 autobuild.py -i entropyv3 -r srrip -o adj09_etp3
python3 autobuild.py -i entropyv3 -r srrip -p berti -d ip_stride -o adj09_etp3
# python3 autobuild.py -i entropyv3 -r srrip -p next_line -d ip_stride -o adj09_etp3
# python3 autobuild.py -i entropyv3 -r srrip -p berti -d spp_dev -o adj09_etp3
# python3 autobuild.py -i entropyv3 -r srrip -p next_line -d spp_dev -o adj09_etp3
