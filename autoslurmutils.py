def parse_apps(apps):
   # parse requested apps
   requested_apps = apps.split("_")

   if apps == "gap":
      requested_apps = [
         "cc.kron-82B", "cc.twitter-85B", \
         "bc.road-174B","bc.road-79B","bc.road-95B",\
         "bc.twitter-876B","bc.twitter-1658B","bc.twitter-2320B","bc.twitter-309B","bc.web-2078B",\
         "bfs.kron-154B","bfs.kron-469B", \
         "bfs.road-112B","bfs.road-128B","bfs.road-140B","bfs.road-99B",\
         "bfs.twitter-160B","bfs.twitter-242B","bfs.twitter-408B","bfs.twitter-44B",\
         "bfs.urand-161B","bfs.urand-36B","bfs.urand-410B","bfs.urand-60B",\
         "cc.kron-10B","cc.kron-17B","cc.kron-78B","cc.kron-99B",\
         "cc.twitter-10B","cc.twitter-15B","cc.twitter-16B","cc.twitter-17B","cc.twitter-25B",\
         "sssp.twitter-1116B","sssp.twitter-900B",\
         "tc.twitter-2097B","tc.twitter-34B",\
         "tc.urand-825B","tc.web-5088B","tc.web-2259B","tc.web-3876B",
         "bc.urand-3004B","bc.web-1627B", \
         "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
         "cc.road-36B","cc.urand-353B","cc.web-24B"
      ]

   elif apps == "cloud":
      requested_apps = [
         "cassandra_phase0_core0",
         "cassandra_phase1_core0",
         "cassandra_phase2_core0",
         "cassandra_phase3_core0",
         "cassandra_phase4_core0",
         "cassandra_phase5_core0",
         "classification_phase0_core0",
         "classification_phase1_core0",
         "classification_phase2_core0",
         "classification_phase3_core0",
         "classification_phase4_core0",
         "classification_phase5_core0",
         "cloud9_phase0_core0",
         "cloud9_phase1_core0",
         "cloud9_phase2_core0",
         "cloud9_phase3_core0",
         "cloud9_phase4_core0",
         "cloud9_phase5_core0",
         "nutch_phase0_core0",
         "nutch_phase1_core0",
         "nutch_phase1_core1",
         "nutch_phase2_core0",
         "nutch_phase3_core0",
         "nutch_phase3_core3",
         "nutch_phase4_core0",
         "nutch_phase5_core0",
         "streaming_phase0_core0",
         "streaming_phase1_core0",
         "streaming_phase2_core0",
         "streaming_phase3_core0",
         "streaming_phase3_core1",
         "streaming_phase4_core0",
         "streaming_phase5_core0",
         "streaming_phase5_core2"
      ]

   elif apps == "dpc3-simpoint":
      # MPKI >= 1 under config 256/2-2,512/4-8,2048/16-32, rank2, DRAM=24
      requested_apps = [
         "403","410","433","436","437","450","459","462","470","471","473","481","482",\
         "602","605","607","620","621","623","627","628","649","654",\
         "bodytrack","canneal","facesim","ferret","fluidanimate","freqmine","streamcluster","vips",\
         "cassandra","http","tomcat","llama"
      ]
   
   elif apps == "dpc3-specmore":
      # MPKI > 1 under config 256/2-2,512/4-8,2048/16-32, rank2, DRAM=24
      requested_apps = [
         "401.bzip2-226B","401.bzip2-277B",\
         "403.gcc-16B",\
         "434.zeusmp-10B",\
         "456.hmmer-327B",\
         "483.xalancbmk-716B",\
         "602.gcc_s-2226B",\
         "603.bwaves_s-1740B","603.bwaves_s-2609B","603.bwaves_s-891B",\
         "654.roms_s-1007B","654.roms_s-1070B","654.roms_s-1613B","654.roms_s-293B","654.roms_s-294B"
      ]
   
   elif apps == "dpc3-spectemp":
      requested_apps = [
         "602.gcc_s-1850B","603.bwaves_s-2931B","654.roms_s-523B","654.roms_s-1390B","483.xalancbmk-127B","403.gcc-48B"
      ]

   elif apps == "cvp-entropy":
      requested_apps = [
         "compute_int_921","compute_int_925","compute_int_4","compute_int_817","compute_int_467","compute_int_529","compute_int_105","compute_int_41","compute_int_177","compute_int_28",\
         "compute_fp_84","compute_fp_44","compute_fp_94","compute_fp_12","compute_fp_129","compute_fp_136","compute_fp_105","compute_fp_124","compute_fp_111","compute_fp_96",\
         "srv776","srv461","srv276","srv715","srv738","srv676","srv405","srv527","srv495","srv48"
      ]
   
   elif apps == "xsbench":
      # "fast" xsbench under 8w1r setup
      requested_apps = ["xs.XL100hash-194B",\
                     "xs.XL100hash-28B",\
                     "xs.XL100nuclide-185B",\
                     "xs.XL100nuclide-296B",\
                     "xs.XL100nuclide-577B",\
                     "xs.XL100nuclide-7B",\
                     "xs.XL100nuclide-829B",\
                     "xs.XL100unionized-28B",\
                     "xs.XL100unionized-56B",\
                     "xs.XL100unionized-66B",\
                     "xs.XL100unionized-75B",\
                     "xs.XL64hash-126B",\
                     "xs.XL64hash-49B",\
                     "xs.XL64nuclide-355B",\
                     "xs.XL64nuclide-7B",\
                     "xs.XL64unionized-28B",\
                     "xs.XL64unionized-56B",\
                     "xs.XL64unionized-66B",\
                     "xs.XL64unionized-75B",\
                     "xs.XXL100hash-18B",\
                     "xs.XXL100hash-94B",\
                     "xs.XXL100nuclide-30B",\
                     "xs.XXL100unionized-122B",\
                     "xs.XXL100unionized-155B",\
                     "xs.XXL100unionized-188B",\
                     "xs.XXL100unionized-24B",\
                     "xs.XXL64hash-24B",\
                     "xs.XXL64nuclide-24B",\
                     "xs.XXL64unionized-122B",\
                     "xs.XXL64unionized-155B",\
                     "xs.XXL64unionized-188B",\
                     "xs.XXL64unionized-24B"]

   elif apps == "geoidx-hpca26":
      requested_apps = ["403.gcc-16B","410","433","434.zeusmp-10B","437","450","456.hmmer-327B","459","462","471","473","482",\
                        "602.gcc_s-2226B","603.bwaves_s-891B","607","620","621","627","628","649","654.roms_s-1007B"]

   elif apps == "cvp1":
      requested_apps = [
         "compute_fp_100",\
         "compute_fp_101",\
         "compute_fp_103",\
         "compute_fp_104",\
         "compute_fp_105",\
         "compute_fp_108",\
         "compute_fp_110",\
         "compute_fp_111",\
         "compute_fp_112",\
         "compute_fp_121",\
         "compute_fp_122",\
         "compute_fp_124",\
         "compute_fp_127",\
         "compute_fp_128",\
         "compute_fp_129",\
         "compute_fp_12",\
         "compute_fp_130",\
         "compute_fp_132",\
         "compute_fp_133",\
         "compute_fp_134",\
         "compute_fp_136",\
         "compute_fp_137",\
         "compute_fp_139",\
         "compute_fp_13",\
         "compute_fp_14",\
         "compute_fp_15",\
         "compute_fp_18",\
         "compute_fp_1",\
         "compute_fp_23",\
         "compute_fp_25",\
         "compute_fp_26",\
         "compute_fp_28",\
         "compute_fp_2",\
         "compute_fp_30",\
         "compute_fp_34",\
         "compute_fp_37",\
         "compute_fp_41",\
         "compute_fp_44",\
         "compute_fp_45",\
         "compute_fp_46",\
         "compute_fp_47",\
         "compute_fp_48",\
         "compute_fp_49",\
         "compute_fp_4",\
         "compute_fp_51",\
         "compute_fp_52",\
         "compute_fp_54",\
         "compute_fp_55",\
         "compute_fp_56",\
         "compute_fp_57",\
         "compute_fp_58",\
         "compute_fp_59",\
         "compute_fp_5",\
         "compute_fp_60",\
         "compute_fp_61",\
         "compute_fp_63",\
         "compute_fp_64",\
         "compute_fp_65",\
         "compute_fp_67",\
         "compute_fp_6",\
         "compute_fp_70",\
         "compute_fp_71",\
         "compute_fp_72",\
         "compute_fp_73",\
         "compute_fp_75",\
         "compute_fp_76",\
         "compute_fp_78",\
         "compute_fp_79",\
         "compute_fp_7",\
         "compute_fp_82",\
         "compute_fp_84",\
         "compute_fp_86",\
         "compute_fp_87",\
         "compute_fp_8",\
         "compute_fp_90",\
         "compute_fp_94",\
         "compute_fp_95",\
         "compute_fp_96",\
         "compute_fp_97",\
         "compute_fp_99",\
         "compute_fp_9",\
         "compute_int_141",\
         "compute_int_149",\
         "compute_int_150",\
         "compute_int_158",\
         "compute_int_161",\
         "compute_int_174",\
         "compute_int_181",\
         "compute_int_204",\
         "compute_int_20",\
         "compute_int_248",\
         "compute_int_259",\
         "compute_int_297",\
         "compute_int_353",\
         "compute_int_354",\
         "compute_int_415",\
         "compute_int_41",\
         "compute_int_424",\
         "compute_int_434",\
         "compute_int_479",\
         "compute_int_536",\
         "compute_int_563",\
         "compute_int_568",\
         "compute_int_63",\
         "compute_int_645",\
         "compute_int_751",\
         "compute_int_788",\
         "compute_int_841",\
         "compute_int_844",\
         "compute_int_848",\
         "compute_int_856",\
         "compute_int_859",\
         "compute_int_865",\
         "compute_int_877",\
         "compute_int_907",\
         "compute_int_921",\
         "compute_int_942",\
         "compute_int_962",\
         "srv111",\
         "srv128",\
         "srv12",\
         "srv131",\
         "srv132",\
         "srv133",\
         "srv137",\
         "srv151",\
         "srv155",\
         "srv160",\
         "srv171",\
         "srv174",\
         "srv194",\
         "srv207",\
         "srv215",\
         "srv21",\
         "srv222",\
         "srv225",\
         "srv228",\
         "srv242",\
         "srv255",\
         "srv259",\
         "srv262",\
         "srv276",\
         "srv279",\
         "srv287",\
         "srv293",\
         "srv314",\
         "srv326",\
         "srv32",\
         "srv331",\
         "srv333",\
         "srv358",\
         "srv364",\
         "srv366",\
         "srv371",\
         "srv393",\
         "srv394",\
         "srv3",\
         "srv401",\
         "srv403",\
         "srv405",\
         "srv408",\
         "srv41",\
         "srv425",\
         "srv426",\
         "srv440",\
         "srv442",\
         "srv44",\
         "srv453",\
         "srv456",\
         "srv459",\
         "srv45",\
         "srv461",\
         "srv470",\
         "srv475",\
         "srv483",\
         "srv489",\
         "srv48",\
         "srv490",\
         "srv495",\
         "srv504",\
         "srv506",\
         "srv509",\
         "srv526",\
         "srv527",\
         "srv537",\
         "srv540",\
         "srv551",\
         "srv552",\
         "srv562",\
         "srv564",\
         "srv56",\
         "srv582",\
         "srv58",\
         "srv598",\
         "srv602",\
         "srv606",\
         "srv609",\
         "srv617",\
         "srv61",\
         "srv623",\
         "srv637",\
         "srv641",\
         "srv642",\
         "srv64",\
         "srv650",\
         "srv662",\
         "srv664",\
         "srv669",\
         "srv671",\
         "srv676",\
         "srv69",\
         "srv702",\
         "srv706",\
         "srv715",\
         "srv717",\
         "srv727",\
         "srv72",\
         "srv738",\
         "srv742",\
         "srv743",\
         "srv745",\
         "srv757",\
         "srv760",\
         "srv764",\
         "srv767",\
         "srv768",\
         "srv771",\
         "srv776",\
         "srv78",\
         "srv85",\
         "srv93",\
         "compute_int_105",\
         "compute_int_177",\
         "compute_int_28",\
         "compute_int_4",\
         "compute_int_467",\
         "compute_int_529",\
         "compute_int_817",\
         "compute_int_925"
      ]
   
   # Note: ferret is ok at 1B, but consistently fails at 10B run (at instruction 1750000001) 
   # due to "src/ooo_cpu.cc:1653: void O3_CPU::execute_store(uint32_t, uint32_t, uint32_t): Assertion `0' failed."
   # this bug is seen in all indexing types, including nohash

   elif apps == "hpca26c4v1":
      requested_apps = [
         ['403.gcc-16B', '410', '434.zeusmp-10B', '450'],
         ['434.zeusmp-10B', '462', '482', '649'],
         ['450', '456.hmmer-327B', '459', '602.gcc_s-2226B'],
         ['410', '433', '456.hmmer-327B', '482'],
         ['410', '620', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '459', '462', '482'],
         ['434.zeusmp-10B', '473', '603.bwaves_s-891B', '620'],
         ['459', '602.gcc_s-2226B', '621', '628'],
         ['410', '473', '627', '628'],
         ['437', '482', '602.gcc_s-2226B', '607'],
         ['433', '459', '628', '649'],
         ['459', '462', '471', '482'],
         ['433', '434.zeusmp-10B', '456.hmmer-327B', '462'],
         ['403.gcc-16B', '602.gcc_s-2226B', '628', '649'],
         ['410', '433', '437', '654.roms_s-1007B'],
         ['410', '462', '649', '654.roms_s-1007B'],
         ['471', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['437', '450', '462', '621'],
         ['433', '434.zeusmp-10B', '602.gcc_s-2226B', '649'],
         ['410', '471', '482', '607'],
         ['434.zeusmp-10B', '456.hmmer-327B', '628', '649'],
         ['437', '450', '462', '603.bwaves_s-891B'],
         ['410', '437', '602.gcc_s-2226B', '649'],
         ['456.hmmer-327B', '473', '620', '621'],
         ['471', '603.bwaves_s-891B', '620', '627'],
         ['403.gcc-16B', '437', '471', '627'],
         ['437', '462', '603.bwaves_s-891B', '620'],
         ['434.zeusmp-10B', '456.hmmer-327B', '602.gcc_s-2226B', '649'],
         ['403.gcc-16B', '410', '459', '602.gcc_s-2226B'],
         ['456.hmmer-327B', '607', '627', '649'],
         ['437', '450', '459', '607'],
         ['450', '456.hmmer-327B', '471', '621'],
         ['482', '603.bwaves_s-891B', '627', '649'],
         ['403.gcc-16B', '410', '620', '649'],
         ['434.zeusmp-10B', '603.bwaves_s-891B', '607', '627'],
         ['403.gcc-16B', '410', '437', '602.gcc_s-2226B'],
         ['403.gcc-16B', '450', '471', '603.bwaves_s-891B'],
         ['437', '471', '628', '649'],
         ['433', '437', '471', '473'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '620'],
         ['471', '602.gcc_s-2226B', '603.bwaves_s-891B', '621'],
         ['459', '603.bwaves_s-891B', '627', '628'],
         ['456.hmmer-327B', '473', '620', '627'],
         ['456.hmmer-327B', '621', '627', '628'],
         ['456.hmmer-327B', '471', '603.bwaves_s-891B', '620'],
         ['410', '434.zeusmp-10B', '462', '471'],
         ['403.gcc-16B', '437', '473', '621'],
         ['410', '434.zeusmp-10B', '459', '607'],
         ['456.hmmer-327B', '602.gcc_s-2226B', '627', '649'],
         ['410', '434.zeusmp-10B', '482', '627'],
         ['433', '471', '603.bwaves_s-891B', '621'],
         ['462', '482', '621', '628'],
         ['437', '450', '473', '628'],
         ['433', '473', '620', '649'],
         ['462', '471', '627', '649'],
         ['403.gcc-16B', '437', '450', '621'],
         ['433', '450', '459', '649'],
         ['433', '437', '459', '607'],
         ['433', '456.hmmer-327B', '603.bwaves_s-891B', '649'],
         ['450', '462', '602.gcc_s-2226B', '621'],
         ['450', '482', '602.gcc_s-2226B', '654.roms_s-1007B'],
         ['473', '627', '628', '649'],
         ['434.zeusmp-10B', '459', '602.gcc_s-2226B', '621'],
         ['456.hmmer-327B', '462', '603.bwaves_s-891B', '620'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '482'],
         ['459', '621', '628', '649'],
         ['410', '459', '471', '627'],
         ['410', '607', '620', '628'],
         ['437', '450', '602.gcc_s-2226B', '607'],
         ['437', '603.bwaves_s-891B', '607', '620'],
         ['403.gcc-16B', '471', '473', '628'],
         ['462', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '459', '603.bwaves_s-891B', '607'],
         ['410', '602.gcc_s-2226B', '607', '620'],
         ['459', '482', '603.bwaves_s-891B', '620'],
         ['410', '437', '450', '459'],
         ['434.zeusmp-10B', '471', '482', '649'],
         ['433', '459', '620', '627'],
         ['450', '471', '473', '482'],
         ['403.gcc-16B', '462', '482', '602.gcc_s-2226B'],
         ['450', '602.gcc_s-2226B', '621', '649'],
         ['437', '459', '628', '654.roms_s-1007B'],
         ['410', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['450', '456.hmmer-327B', '462', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '459', '473'],
         ['410', '620', '621', '627'],
         ['450', '621', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '621', '628'],
         ['403.gcc-16B', '437', '473', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '628'],
         ['450', '459', '482', '628'],
         ['437', '459', '607', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '620', '627'],
         ['403.gcc-16B', '433', '437', '603.bwaves_s-891B'],
         ['433', '471', '482', '627'],
         ['459', '471', '603.bwaves_s-891B', '628'],
         ['456.hmmer-327B', '471', '473', '620'],
         ['433', '471', '607', '621'],
         ['437', '462', '628', '654.roms_s-1007B'],
         ['410', '459', '649', '654.roms_s-1007B']
      ]

   elif apps == "hpca26c4v2":
      requested_apps = [
         ['403.gcc-16B', '459', '482', '649'],
         ['410', '459', '471', '607'],
         ['462', '602.gcc_s-2226B', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '456.hmmer-327B', '607', '628'],
         ['410', '437', '602.gcc_s-2226B', '620'],
         ['437', '459', '607', '620'],
         ['450', '462', '620', '649'],
         ['403.gcc-16B', '433', '434.zeusmp-10B', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '607', '620'],
         ['434.zeusmp-10B', '437', '473', '649'],
         ['433', '603.bwaves_s-891B', '628', '649'],
         ['410', '450', '602.gcc_s-2226B', '628'],
         ['437', '462', '471', '482'],
         ['403.gcc-16B', '433', '627', '654.roms_s-1007B'],
         ['437', '450', '473', '627'],
         ['433', '482', '620', '621'],
         ['434.zeusmp-10B', '459', '607', '620'],
         ['403.gcc-16B', '471', '482', '627'],
         ['403.gcc-16B', '462', '482', '649'],
         ['433', '471', '621', '627'],
         ['410', '437', '482', '620'],
         ['450', '456.hmmer-327B', '459', '654.roms_s-1007B'],
         ['456.hmmer-327B', '607', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '473', '602.gcc_s-2226B'],
         ['437', '459', '462', '482'],
         ['403.gcc-16B', '434.zeusmp-10B', '456.hmmer-327B', '462'],
         ['437', '450', '456.hmmer-327B', '620'],
         ['482', '620', '621', '627'],
         ['410', '456.hmmer-327B', '471', '620'],
         ['410', '433', '434.zeusmp-10B', '462'],
         ['462', '473', '602.gcc_s-2226B', '603.bwaves_s-891B'],
         ['456.hmmer-327B', '482', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '433', '628'],
         ['473', '620', '621', '627'],
         ['433', '471', '620', '627'],
         ['462', '471', '607', '649'],
         ['410', '433', '482', '654.roms_s-1007B'],
         ['482', '603.bwaves_s-891B', '620', '649'],
         ['450', '602.gcc_s-2226B', '620', '654.roms_s-1007B'],
         ['450', '459', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '410', '433', '607'],
         ['410', '450', '459', '471'],
         ['462', '607', '620', '654.roms_s-1007B'],
         ['410', '607', '649', '654.roms_s-1007B'],
         ['433', '471', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['450', '471', '607', '628'],
         ['459', '482', '603.bwaves_s-891B', '628'],
         ['410', '462', '482', '654.roms_s-1007B'],
         ['403.gcc-16B', '437', '473', '603.bwaves_s-891B'],
         ['433', '620', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '620', '628'],
         ['410', '456.hmmer-327B', '603.bwaves_s-891B', '627'],
         ['473', '603.bwaves_s-891B', '627', '628'],
         ['462', '471', '621', '649'],
         ['434.zeusmp-10B', '437', '462', '627'],
         ['437', '603.bwaves_s-891B', '620', '627'],
         ['437', '482', '620', '654.roms_s-1007B'],
         ['410', '473', '603.bwaves_s-891B', '620'],
         ['403.gcc-16B', '471', '473', '482'],
         ['403.gcc-16B', '471', '627', '649'],
         ['403.gcc-16B', '437', '603.bwaves_s-891B', '620'],
         ['450', '462', '607', '620'],
         ['403.gcc-16B', '434.zeusmp-10B', '437', '450'],
         ['456.hmmer-327B', '462', '482', '602.gcc_s-2226B'],
         ['450', '620', '621', '627'],
         ['403.gcc-16B', '450', '607', '627'],
         ['459', '473', '602.gcc_s-2226B', '607'],
         ['410', '434.zeusmp-10B', '607', '628'],
         ['459', '607', '620', '621'],
         ['437', '482', '628', '649'],
         ['434.zeusmp-10B', '471', '627', '654.roms_s-1007B'],
         ['410', '433', '434.zeusmp-10B', '621'],
         ['403.gcc-16B', '473', '620', '627'],
         ['410', '620', '628', '649'],
         ['433', '482', '649', '654.roms_s-1007B'],
         ['450', '456.hmmer-327B', '473', '620'],
         ['450', '603.bwaves_s-891B', '620', '628'],
         ['434.zeusmp-10B', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '462', '627'],
         ['434.zeusmp-10B', '473', '602.gcc_s-2226B', '628'],
         ['471', '602.gcc_s-2226B', '620', '628'],
         ['433', '456.hmmer-327B', '462', '602.gcc_s-2226B'],
         ['471', '473', '602.gcc_s-2226B', '620'],
         ['433', '434.zeusmp-10B', '471', '627'],
         ['410', '434.zeusmp-10B', '459', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '471', '627', '628'],
         ['456.hmmer-327B', '459', '621', '628'],
         ['437', '621', '627', '628'],
         ['450', '459', '473', '627'],
         ['456.hmmer-327B', '607', '621', '628'],
         ['403.gcc-16B', '410', '456.hmmer-327B', '602.gcc_s-2226B'],
         ['459', '602.gcc_s-2226B', '649', '654.roms_s-1007B'],
         ['410', '433', '621', '654.roms_s-1007B'],
         ['433', '437', '456.hmmer-327B', '649'],
         ['433', '450', '459', '471'],
         ['462', '602.gcc_s-2226B', '649', '654.roms_s-1007B'],
         ['473', '607', '620', '649'],
         ['403.gcc-16B', '471', '602.gcc_s-2226B', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '459', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['471', '620', '621', '627']
      ]

   return requested_apps
