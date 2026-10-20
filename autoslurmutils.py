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
   elif apps == "parsec":
      requested_apps = ["bodytrack","canneal","facesim","ferret","fluidanimate","freqmine","streamcluster","vips"]
   elif apps == "dpc4-ml":
      requested_apps = [
         "bark.cpp-bark-small.coarse_token_gen.1",\
         "bark.cpp-bark-small.coarse_token_gen.4",\
         "bark.cpp-bark-small.coarse_token_gen.5",\
         "clip_trace_1",\
         "clip_trace_3",\
         "llama2.c-stories110M.1",\
         "llama2.c-stories110M.2",\
         "llama2.c-stories110M.3",\
         "llama2.c-stories15M.1",\
         "llama2.c-stories15M.2",\
         "llama2.c-stories15M.3",\
         "llama2.c-stories42M.1",\
         "llama2.c-stories42M.2",\
         "llama2.c-stories42M.3",\
         "rwkv_trace_3",\
         "stable-diffusion.cpp-sd-v1-4.ckpt.1",\
         "stable-diffusion.cpp-sd-v1-4.ckpt.2",\
         "stable-diffusion.cpp-v2-1_768-nonema-pruned.1",\
         "stable-diffusion.cpp-v2-1_768-nonema-pruned.3",\
         "vit.cpp-base-ggml-model-f16.gguf.armadillo.1",\
         "vit.cpp-large-ggml-model-f16.gguf.armadillo.1",\
         "whisper_trace_1",\
         "whisper_trace_2",\
         "whisper_trace_3"
      ]
   elif apps == "dpc4-gms":
      requested_apps = [
         "gms.k_clique_count_set_based.soc-pokec-relationships.slice-4",\
         "gms.k_clique_list_danisch_edge_parallel.soc-pokec-relationships.slice-367",\
         "gms.k_clique_list_danisch_edge_parallel.soc-pokec-relationships.slice-80",\
         "gms.k_clique_list_danisch_node_parallel.soc-pokec-relationships.slice-236",\
         "gms.k_clique_list_danisch_node_parallel.soc-pokec-relationships.slice-248",\
         "gms.k_clique_list_danisch_serial.soc-pokec-relationships.slice-4",\
         "gms.k_clique_star_list.soc-pokec-relationships.slice-1291",\
         "gms.maximal_clique_enum_bron_kerbosch.soc-pokec-relationships.slice-3852",\
         "gms.triangle_count.soc-pokec-relationships.slice-117",\
         "gms.triangle_count.soc-pokec-relationships.slice-137",\
         "gms.triangle_count.soc-pokec-relationships.slice-360"
      ]
   elif apps == "dpc3-simpoint":
      # MPKI >= 1 under config 256/2-2,512/4-8,2048/16-32, rank2, DRAM=24
      requested_apps = [
         "410","433","436","437","450","459","462","470","471","473","481","482",\
         "602","605","607","620","621","623","627","628","649","654"
      ]
   elif apps == "dpc3-specmore":
      # MPKI > 1 under config 256/2-2,512/4-8,2048/16-32, rank2, DRAM=24
      requested_apps = [
         # "401.bzip2-226B","401.bzip2-277B",\
         # "403.gcc-16B",\
         # "434.zeusmp-10B",\
         # "456.hmmer-327B",\
         # "483.xalancbmk-716B",\
         # "602.gcc_s-2226B",\
         # "603.bwaves_s-1740B","603.bwaves_s-2609B","603.bwaves_s-891B",\
         # "654.roms_s-1007B","654.roms_s-1070B","654.roms_s-1613B","654.roms_s-293B","654.roms_s-294B"
         "403.gcc-16B","434.zeusmp-10B","456.hmmer-327B","602.gcc_s-2226B","603.bwaves_s-891B","654.roms_s-1007B"
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

   elif apps == "geoidx-spec":
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
   
   elif apps == "cvp1-hpca26c4":
      requested_apps = ["compute_fp_75","compute_fp_105","compute_fp_101","compute_fp_124","compute_fp_59","compute_fp_4","compute_fp_57","compute_fp_96","compute_fp_37","compute_fp_111",
                        "compute_int_751","compute_int_28",
                        "srv366","srv137","srv160","srv461","srv602","srv132","srv509","srv69","srv440","srv776","srv262","srv717","srv279","srv394","srv650","srv767","srv64","srv676","srv242"]
      # requested_apps = ["srv470","srv78","srv58","srv637","compute_fp_100","compute_fp_12","compute_fp_61"]

   # Note: ferret is ok at 1B, but consistently fails at 10B run (at instruction 1750000001) 
   # due to "src/ooo_cpu.cc:1653: void O3_CPU::execute_store(uint32_t, uint32_t, uint32_t): Assertion `0' failed."
   # this bug is seen in all indexing types, including nohash

   elif apps == "geoidx-cvp1":
      requested_apps = [
         "compute_fp_1",\
         "compute_fp_100",\
         "compute_fp_101",\
         "compute_fp_104",\
         "compute_fp_105",\
         "compute_fp_108",\
         "compute_fp_110",\
         "compute_fp_111",\
         "compute_fp_112",\
         "compute_fp_12",\
         "compute_fp_121",\
         "compute_fp_124",\
         "compute_fp_127",\
         "compute_fp_128",\
         "compute_fp_129",\
         "compute_fp_130",\
         "compute_fp_133",\
         "compute_fp_134",\
         "compute_fp_136",\
         "compute_fp_139",\
         "compute_fp_14",\
         "compute_fp_15",\
         "compute_fp_25",\
         "compute_fp_30",\
         "compute_fp_37",\
         "compute_fp_4",\
         "compute_fp_44",\
         "compute_fp_48",\
         "compute_fp_49",\
         "compute_fp_5",\
         "compute_fp_51",\
         "compute_fp_54",\
         "compute_fp_56",\
         "compute_fp_57",\
         "compute_fp_59",\
         "compute_fp_6",\
         "compute_fp_61",\
         "compute_fp_63",\
         "compute_fp_65",\
         "compute_fp_67",\
         "compute_fp_7",\
         "compute_fp_75",\
         "compute_fp_78",\
         "compute_fp_8",\
         "compute_fp_84",\
         "compute_fp_87",\
         "compute_fp_94",\
         "compute_fp_95",\
         "compute_fp_96",\
         "compute_fp_97",\
         "compute_int_105",\
         "compute_int_204",\
         "compute_int_28",\
         "compute_int_354",\
         "compute_int_434",\
         "compute_int_479",\
         "compute_int_751",\
         "srv111",\
         "srv132",\
         "srv137",\
         "srv155",\
         "srv160",\
         "srv174",\
         "srv215",\
         "srv242",\
         "srv262",\
         "srv279",\
         "srv3",\
         "srv314",\
         "srv333",\
         "srv366",\
         "srv371",\
         "srv394",\
         "srv405",\
         "srv440",\
         "srv453",\
         "srv456",\
         "srv461",\
         "srv470",\
         "srv475",\
         "srv490",\
         "srv509",\
         "srv552",\
         "srv562",\
         "srv564",\
         "srv58",\
         "srv602",\
         "srv637",\
         "srv64",\
         "srv650",\
         "srv671",\
         "srv676",\
         "srv69",\
         "srv717",\
         "srv738",\
         "srv745",\
         "srv767",\
         "srv776",\
         "srv78",\
         "srv93"
      ]

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

   elif apps == "hpca26c4v3":
      requested_apps = [
         ['437', '456.hmmer-327B', '628', '649'],
         ['403.gcc-16B', '450', '459', '621'],
         ['403.gcc-16B', '459', '620', '654.roms_s-1007B'],
         ['433', '450', '607', '621'],
         ['434.zeusmp-10B', '471', '603.bwaves_s-891B', '620'],
         ['450', '482', '603.bwaves_s-891B', '620'],
         ['403.gcc-16B', '450', '459', '627'],
         ['434.zeusmp-10B', '471', '621', '649'],
         ['459', '603.bwaves_s-891B', '620', '649'],
         ['462', '607', '627', '654.roms_s-1007B'],
         ['410', '437', '607', '649'],
         ['462', '473', '620', '628'],
         ['433', '482', '621', '649'],
         ['433', '450', '620', '628'],
         ['450', '627', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '482', '603.bwaves_s-891B'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '620'],
         ['434.zeusmp-10B', '450', '473', '620'],
         ['437', '456.hmmer-327B', '462', '607'],
         ['410', '437', '482', '603.bwaves_s-891B'],
         ['462', '471', '628', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '462', '607'],
         ['462', '473', '620', '627'],
         ['403.gcc-16B', '459', '603.bwaves_s-891B', '607'],
         ['410', '471', '628', '649'],
         ['403.gcc-16B', '471', '482', '649'],
         ['433', '603.bwaves_s-891B', '620', '621'],
         ['410', '433', '627', '654.roms_s-1007B'],
         ['450', '473', '620', '621'],
         ['433', '471', '621', '628'],
         ['410', '433', '471', '621'],
         ['410', '437', '473', '607'],
         ['462', '621', '627', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '459', '482'],
         ['434.zeusmp-10B', '450', '462', '621'],
         ['450', '456.hmmer-327B', '471', '627'],
         ['433', '450', '607', '654.roms_s-1007B'],
         ['437', '459', '473', '649'],
         ['433', '450', '471', '621'],
         ['410', '433', '456.hmmer-327B', '471'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '471'],
         ['433', '450', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '450', '462', '654.roms_s-1007B'],
         ['433', '437', '450', '607'],
         ['403.gcc-16B', '450', '462', '649'],
         ['410', '459', '462', '471'],
         ['603.bwaves_s-891B', '621', '627', '628'],
         ['459', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '471', '628', '649'],
         ['450', '462', '471', '607'],
         ['433', '456.hmmer-327B', '482', '607'],
         ['403.gcc-16B', '433', '482', '649'],
         ['403.gcc-16B', '437', '462', '649'],
         ['434.zeusmp-10B', '482', '607', '620'],
         ['462', '482', '620', '654.roms_s-1007B'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '462'],
         ['456.hmmer-327B', '459', '607', '620'],
         ['434.zeusmp-10B', '462', '627', '649'],
         ['456.hmmer-327B', '607', '621', '649'],
         ['433', '482', '603.bwaves_s-891B', '627'],
         ['410', '450', '621', '628'],
         ['434.zeusmp-10B', '607', '620', '654.roms_s-1007B'],
         ['410', '456.hmmer-327B', '471', '482'],
         ['433', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '607', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '607', '620'],
         ['456.hmmer-327B', '459', '603.bwaves_s-891B', '649'],
         ['434.zeusmp-10B', '450', '471', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '437', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['410', '437', '473', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '621', '627', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '607', '621', '649'],
         ['459', '607', '620', '627'],
         ['437', '473', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '456.hmmer-327B', '607', '649'],
         ['437', '627', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '459', '621', '649'],
         ['433', '450', '620', '627'],
         ['433', '434.zeusmp-10B', '437', '462'],
         ['471', '482', '620', '627'],
         ['434.zeusmp-10B', '459', '621', '628'],
         ['434.zeusmp-10B', '459', '603.bwaves_s-891B', '649'],
         ['462', '607', '627', '649'],
         ['450', '456.hmmer-327B', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['410', '450', '471', '621'],
         ['433', '471', '482', '607'],
         ['433', '482', '621', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '456.hmmer-327B', '459', '607'],
         ['403.gcc-16B', '450', '456.hmmer-327B', '621'],
         ['462', '482', '628', '654.roms_s-1007B'],
         ['433', '437', '621', '649'],
         ['473', '607', '621', '628'],
         ['437', '462', '603.bwaves_s-891B', '627'],
         ['434.zeusmp-10B', '437', '482', '620'],
         ['437', '471', '607', '627'],
         ['434.zeusmp-10B', '450', '459', '607'],
         ['410', '434.zeusmp-10B', '607', '627'],
         ['403.gcc-16B', '433', '462', '607'],
         ['434.zeusmp-10B', '437', '471', '607'],
         ['473', '621', '627', '649']
      ]

   elif apps == "hpca26c4v4":
      requested_apps = [
         ['410', '459', '462', '621'],   # hpca26v4
         ['456.hmmer-327B', '482', '607', '621'],
         ['403.gcc-16B', '459', '462', '620'],
         ['410', '471', '621', '654.roms_s-1007B'],
         ['410', '450', '456.hmmer-327B', '473'],
         ['410', '471', '482', '628'],
         ['462', '621', '628', '649'],
         ['434.zeusmp-10B', '437', '603.bwaves_s-891B', '649'],
         ['450', '456.hmmer-327B', '628', '654.roms_s-1007B'],
         ['462', '471', '607', '621'],
         ['437', '482', '627', '649'],
         ['410', '603.bwaves_s-891B', '621', '654.roms_s-1007B'],
         ['410', '473', '621', '649'],
         ['410', '462', '607', '620'],
         ['410', '450', '603.bwaves_s-891B', '621'],
         ['434.zeusmp-10B', '437', '459', '620'],
         ['410', '603.bwaves_s-891B', '607', '627'],
         ['433', '456.hmmer-327B', '482', '628'],
         ['433', '434.zeusmp-10B', '437', '649'],
         ['433', '456.hmmer-327B', '627', '628'],
         ['462', '473', '603.bwaves_s-891B', '627'],
         ['456.hmmer-327B', '471', '482', '621'],
         ['433', '459', '603.bwaves_s-891B', '628'],
         ['482', '621', '627', '628'],
         ['437', '450', '473', '620'],
         ['459', '620', '621', '649'],
         ['462', '471', '603.bwaves_s-891B', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '437', '627'],
         ['410', '456.hmmer-327B', '482', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '456.hmmer-327B', '620', '628'],
         ['450', '471', '482', '621'],
         ['473', '603.bwaves_s-891B', '607', '649'],
         ['403.gcc-16B', '410', '437', '649'],
         ['434.zeusmp-10B', '473', '482', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '433', '654.roms_s-1007B'],
         ['450', '471', '620', '621'],
         ['434.zeusmp-10B', '437', '649', '654.roms_s-1007B'],
         ['410', '450', '462', '654.roms_s-1007B'],
         ['410', '450', '482', '603.bwaves_s-891B'],
         ['462', '471', '482', '603.bwaves_s-891B'],
         ['437', '471', '628', '654.roms_s-1007B'],
         ['410', '482', '607', '621'],
         ['456.hmmer-327B', '459', '607', '621'],
         ['456.hmmer-327B', '462', '621', '627'],
         ['437', '456.hmmer-327B', '473', '649'],
         ['434.zeusmp-10B', '462', '627', '628'],
         ['450', '471', '482', '607'],
         ['437', '456.hmmer-327B', '473', '654.roms_s-1007B'],
         ['462', '473', '627', '649'],
         ['403.gcc-16B', '620', '627', '654.roms_s-1007B'],
         ['450', '603.bwaves_s-891B', '621', '627'],
         ['403.gcc-16B', '434.zeusmp-10B', '620', '627'],
         ['403.gcc-16B', '433', '471', '607'],
         ['403.gcc-16B', '450', '462', '482'],
         ['450', '456.hmmer-327B', '462', '471'],
         ['462', '607', '621', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '628'],
         ['459', '482', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '471', '620', '628'],
         ['437', '456.hmmer-327B', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['456.hmmer-327B', '459', '473', '628'],
         ['433', '437', '620', '649'],
         ['403.gcc-16B', '410', '459', '649'],
         ['410', '482', '603.bwaves_s-891B', '628'],
         ['410', '433', '456.hmmer-327B', '459'],
         ['450', '462', '627', '628'],
         ['403.gcc-16B', '459', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['603.bwaves_s-891B', '620', '621', '654.roms_s-1007B'],
         ['433', '459', '621', '649'],
         ['433', '607', '621', '627'],
         ['437', '603.bwaves_s-891B', '607', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '462', '628'],
         ['450', '482', '603.bwaves_s-891B', '628'],
         ['433', '471', '607', '627'],
         ['462', '471', '621', '627'],
         ['456.hmmer-327B', '603.bwaves_s-891B', '627', '649'],
         ['434.zeusmp-10B', '437', '450', '621'],
         ['462', '603.bwaves_s-891B', '627', '628'],
         ['434.zeusmp-10B', '437', '462', '620'],
         ['403.gcc-16B', '410', '621', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '437', '459', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '462', '654.roms_s-1007B'],
         ['437', '473', '607', '620'],
         ['433', '437', '627', '649'],
         ['410', '434.zeusmp-10B', '450', '620'],
         ['456.hmmer-327B', '459', '462', '620'],
         ['410', '459', '607', '649'],
         ['459', '473', '621', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '473', '627'],
         ['410', '437', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '471', '627'],
         ['434.zeusmp-10B', '450', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '607', '621', '628'],
         ['482', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '603.bwaves_s-891B'],
         ['433', '450', '471', '628'],
         ['437', '450', '607', '654.roms_s-1007B'],
         ['437', '450', '471', '603.bwaves_s-891B'],
         ['459', '482', '607', '649'],
         ['473', '482', '627', '649']
      ]

   elif apps == "hpca26c4v5":
      requested_apps = [
         ['434.zeusmp-10B', '456.hmmer-327B', '621', '654.roms_s-1007B'],    # hpca26v5
         ['434.zeusmp-10B', '482', '607', '628'],
         ['410', '434.zeusmp-10B', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['433', '450', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['473', '482', '607', '621'],
         ['410', '433', '437', '456.hmmer-327B'],
         ['456.hmmer-327B', '459', '473', '621'],
         ['410', '450', '459', '607'],
         ['433', '459', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '437', '482', '649'],
         ['434.zeusmp-10B', '603.bwaves_s-891B', '628', '654.roms_s-1007B'],
         ['410', '607', '621', '628'],
         ['403.gcc-16B', '410', '462', '627'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '621'],
         ['473', '607', '621', '627'],
         ['433', '450', '482', '649'],
         ['450', '459', '462', '603.bwaves_s-891B'],
         ['403.gcc-16B', '473', '607', '654.roms_s-1007B'],
         ['410', '456.hmmer-327B', '621', '649'],
         ['434.zeusmp-10B', '462', '603.bwaves_s-891B', '649'],
         ['433', '450', '473', '621'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '607', '627'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '482'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '456.hmmer-327B'],
         ['403.gcc-16B', '434.zeusmp-10B', '462', '621'],
         ['433', '603.bwaves_s-891B', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '621', '627', '654.roms_s-1007B'],
         ['433', '462', '649', '654.roms_s-1007B'],
         ['459', '473', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['450', '473', '607', '654.roms_s-1007B'],
         ['456.hmmer-327B', '621', '627', '649'],
         ['410', '621', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '607', '628', '654.roms_s-1007B'],
         ['410', '433', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['410', '607', '621', '627'],
         ['462', '482', '627', '649'],
         ['403.gcc-16B', '450', '607', '649'],
         ['433', '459', '627', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '462', '607', '628'],
         ['434.zeusmp-10B', '482', '607', '621'],
         ['410', '433', '450', '462'],
         ['434.zeusmp-10B', '607', '627', '628'],
         ['434.zeusmp-10B', '628', '649', '654.roms_s-1007B'],
         ['433', '462', '473', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '433', '462'],
         ['433', '437', '462', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '473', '654.roms_s-1007B'],
         ['433', '482', '628', '649'],
         ['410', '433', '607', '627'],
         ['403.gcc-16B', '473', '607', '649'],
         ['410', '437', '621', '628'],
         ['437', '473', '607', '628'],
         ['450', '456.hmmer-327B', '607', '649'],
         ['434.zeusmp-10B', '473', '628', '649'],
         ['462', '627', '649', '654.roms_s-1007B'],
         ['433', '437', '607', '649'],
         ['410', '434.zeusmp-10B', '437', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '462', '607', '654.roms_s-1007B'],
         ['450', '462', '482', '621'],
         ['437', '473', '621', '649'],
         ['433', '459', '462', '654.roms_s-1007B'],
         ['437', '462', '482', '607'],
         ['403.gcc-16B', '459', '603.bwaves_s-891B', '627'],
         ['433', '482', '603.bwaves_s-891B', '621'],
         ['433', '607', '621', '649'],
         ['410', '433', '462', '482'],
         ['403.gcc-16B', '434.zeusmp-10B', '437', '628'],
         ['434.zeusmp-10B', '450', '473', '649'],
         ['403.gcc-16B', '433', '607', '627'],
         ['437', '627', '628', '649'],
         ['433', '450', '473', '628'],
         ['434.zeusmp-10B', '456.hmmer-327B', '462', '482'],
         ['437', '603.bwaves_s-891B', '607', '627'],
         ['473', '603.bwaves_s-891B', '621', '649'],
         ['410', '456.hmmer-327B', '459', '621'],
         ['410', '434.zeusmp-10B', '456.hmmer-327B', '607'],
         ['403.gcc-16B', '437', '462', '473'],
         ['403.gcc-16B', '456.hmmer-327B', '459', '627'],
         ['403.gcc-16B', '433', '628', '649'],
         ['434.zeusmp-10B', '456.hmmer-327B', '459', '473'],
         ['433', '459', '603.bwaves_s-891B', '627'],
         ['450', '459', '473', '621'],
         ['434.zeusmp-10B', '456.hmmer-327B', '482', '603.bwaves_s-891B'],
         ['433', '434.zeusmp-10B', '437', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '627', '654.roms_s-1007B'],
         ['450', '456.hmmer-327B', '482', '627'],
         ['456.hmmer-327B', '462', '473', '607'],
         ['410', '473', '627', '654.roms_s-1007B'],
         ['459', '473', '621', '628'],
         ['410', '433', '459', '462'],
         ['434.zeusmp-10B', '450', '607', '649'],
         ['434.zeusmp-10B', '450', '473', '603.bwaves_s-891B'],
         ['450', '473', '607', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '459', '627'],
         ['410', '462', '627', '649'],
         ['437', '482', '607', '649'],
         ['410', '433', '482', '603.bwaves_s-891B']
      ]

   elif apps == "hpca26c4v6":
      requested_apps = [
         ['456.hmmer-327B', '607', '621', '654.roms_s-1007B'],   # hpca26v6
         ['433', '473', '621', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '603.bwaves_s-891B', '628'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '482'],
         ['433', '462', '628', '654.roms_s-1007B'],
         ['437', '450', '456.hmmer-327B', '649'],
         ['410', '459', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '437', '621', '627'],
         ['433', '456.hmmer-327B', '621', '628'],
         ['434.zeusmp-10B', '473', '607', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '607', '628'],
         ['403.gcc-16B', '603.bwaves_s-891B', '607', '627'],
         ['410', '459', '482', '628'],
         ['450', '462', '627', '649'],
         ['433', '437', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['410', '450', '459', '473'],
         ['434.zeusmp-10B', '603.bwaves_s-891B', '607', '654.roms_s-1007B'],
         ['607', '627', '628', '649'],
         ['459', '462', '473', '654.roms_s-1007B'],
         ['433', '462', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '654.roms_s-1007B'],
         ['433', '437', '482', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '450', '456.hmmer-327B', '621'],
         ['410', '433', '437', '628'],
         ['456.hmmer-327B', '603.bwaves_s-891B', '607', '649'],
         ['434.zeusmp-10B', '437', '459', '627'],
         ['410', '462', '628', '649'],
         ['437', '456.hmmer-327B', '607', '628'],
         ['433', '437', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['437', '456.hmmer-327B', '462', '628'],
         ['433', '437', '603.bwaves_s-891B', '627'],
         ['410', '450', '482', '654.roms_s-1007B'],
         ['433', '437', '482', '649'],
         ['456.hmmer-327B', '607', '649', '654.roms_s-1007B'],
         ['433', '437', '456.hmmer-327B', '459'],
         ['410', '433', '628', '649'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '459'],
         ['410', '437', '456.hmmer-327B', '654.roms_s-1007B'],
         ['462', '621', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '603.bwaves_s-891B', '628'],
         ['410', '459', '473', '628'],
         ['437', '456.hmmer-327B', '621', '649'],
         ['450', '482', '607', '621'],
         ['434.zeusmp-10B', '482', '621', '654.roms_s-1007B'],
         ['433', '450', '603.bwaves_s-891B', '628'],
         ['450', '473', '621', '628'],
         ['603.bwaves_s-891B', '627', '628', '654.roms_s-1007B'],
         ['437', '462', '627', '654.roms_s-1007B'],
         ['473', '607', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '473', '621'],
         ['403.gcc-16B', '603.bwaves_s-891B', '628', '649'],
         ['433', '456.hmmer-327B', '621', '649'],
         ['450', '456.hmmer-327B', '473', '654.roms_s-1007B'],
         ['437', '459', '482', '654.roms_s-1007B'],
         ['456.hmmer-327B', '459', '628', '649'],
         ['434.zeusmp-10B', '437', '456.hmmer-327B', '482'],
         ['403.gcc-16B', '462', '473', '628'],
         ['403.gcc-16B', '459', '462', '473'],
         ['403.gcc-16B', '450', '627', '649'],
         ['410', '482', '621', '627'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '607'],
         ['410', '433', '437', '473'],
         ['410', '462', '607', '627'],
         ['434.zeusmp-10B', '607', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '437', '459', '621'],
         ['403.gcc-16B', '410', '433', '603.bwaves_s-891B'],
         ['410', '459', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '621', '627', '628'],
         ['403.gcc-16B', '410', '433', '627'],
         ['437', '607', '627', '649'],
         ['437', '459', '603.bwaves_s-891B', '627'],
         ['410', '433', '456.hmmer-327B', '607'],
         ['456.hmmer-327B', '459', '482', '603.bwaves_s-891B'],
         ['433', '621', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '434.zeusmp-10B', '621'],
         ['410', '456.hmmer-327B', '459', '473'],
         ['437', '459', '607', '621'],
         ['410', '434.zeusmp-10B', '603.bwaves_s-891B', '627'],
         ['437', '456.hmmer-327B', '627', '628'],
         ['603.bwaves_s-891B', '607', '621', '628'],
         ['456.hmmer-327B', '459', '473', '482'],
         ['410', '433', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '621', '649', '654.roms_s-1007B'],
         ['410', '450', '603.bwaves_s-891B', '627'],
         ['410', '450', '462', '473'],
         ['437', '450', '607', '628'],
         ['459', '462', '621', '628'],
         ['433', '456.hmmer-327B', '473', '603.bwaves_s-891B'],
         ['433', '473', '628', '649'],
         ['410', '434.zeusmp-10B', '459', '627'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '473'],
         ['403.gcc-16B', '437', '482', '603.bwaves_s-891B'],
         ['433', '450', '627', '628'],
         ['459', '473', '603.bwaves_s-891B', '607'],
         ['403.gcc-16B', '433', '607', '621'],
         ['456.hmmer-327B', '473', '607', '628'],
         ['403.gcc-16B', '603.bwaves_s-891B', '621', '649'],
         ['403.gcc-16B', '410', '437', '607'],
         ['433', '437', '607', '627']
      ]

   elif apps == "hpca26c4v7":
      requested_apps = [
         ['403.gcc-16B', '410', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '459', '462', '628'],
         ['410', '450', '473', '649'],
         ['456.hmmer-327B', '459', '482', '649'],
         ['433', '434.zeusmp-10B', '621', '649'],
         ['450', '456.hmmer-327B', '627', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '456.hmmer-327B', '654.roms_s-1007B'],
         ['410', '450', '456.hmmer-327B', '459'],
         ['410', '433', '621', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '473', '649'],
         ['462', '603.bwaves_s-891B', '628', '649'],
         ['437', '462', '473', '603.bwaves_s-891B'],
         ['403.gcc-16B', '433', '649', '654.roms_s-1007B'],
         ['433', '450', '456.hmmer-327B', '621'],
         ['434.zeusmp-10B', '437', '621', '627'],
         ['607', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '649'],
         ['437', '459', '621', '628'],
         ['437', '482', '603.bwaves_s-891B', '621'],
         ['450', '482', '649', '654.roms_s-1007B'],
         ['437', '456.hmmer-327B', '621', '654.roms_s-1007B'],
         ['433', '450', '456.hmmer-327B', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '462', '473'],
         ['450', '482', '603.bwaves_s-891B', '627'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '473'],
         ['410', '456.hmmer-327B', '462', '603.bwaves_s-891B'],
         ['410', '433', '450', '603.bwaves_s-891B'],
         ['433', '450', '482', '628'],
         ['403.gcc-16B', '433', '462', '603.bwaves_s-891B'],
         ['450', '482', '603.bwaves_s-891B', '649'],
         ['433', '459', '649', '654.roms_s-1007B'],
         ['603.bwaves_s-891B', '607', '627', '628'],
         ['456.hmmer-327B', '482', '627', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '627', '628'],
         ['410', '459', '603.bwaves_s-891B', '607'],
         ['403.gcc-16B', '433', '434.zeusmp-10B', '649'],
         ['433', '473', '607', '627'],
         ['437', '459', '628', '649'],
         ['450', '462', '603.bwaves_s-891B', '627'],
         ['482', '607', '627', '649'],
         ['434.zeusmp-10B', '459', '482', '621'],
         ['434.zeusmp-10B', '456.hmmer-327B', '459', '462'],
         ['410', '434.zeusmp-10B', '456.hmmer-327B', '628'],
         ['456.hmmer-327B', '621', '628', '649'],
         ['403.gcc-16B', '410', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '437', '607', '621'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '462'],
         ['433', '434.zeusmp-10B', '462', '482'],
         ['403.gcc-16B', '456.hmmer-327B', '628', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '462', '607', '621'],
         ['410', '627', '628', '649'],
         ['433', '434.zeusmp-10B', '462', '627'],
         ['433', '434.zeusmp-10B', '621', '654.roms_s-1007B'],
         ['459', '462', '607', '627'],
         ['434.zeusmp-10B', '450', '627', '649'],
         ['433', '450', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['410', '433', '459', '482'],
         ['603.bwaves_s-891B', '627', '649', '654.roms_s-1007B'],
         ['433', '434.zeusmp-10B', '450', '627'],
         ['403.gcc-16B', '437', '450', '473'],
         ['403.gcc-16B', '433', '459', '649'],
         ['456.hmmer-327B', '482', '649', '654.roms_s-1007B'],
         ['482', '607', '621', '649'],
         ['459', '473', '603.bwaves_s-891B', '628'],
         ['433', '456.hmmer-327B', '462', '603.bwaves_s-891B'],
         ['603.bwaves_s-891B', '607', '649', '654.roms_s-1007B'],
         ['437', '459', '482', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '473', '621', '627'],
         ['473', '482', '607', '627'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '473'],
         ['403.gcc-16B', '433', '459', '621'],
         ['434.zeusmp-10B', '473', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '607', '649', '654.roms_s-1007B'],
         ['482', '603.bwaves_s-891B', '607', '621'],
         ['403.gcc-16B', '603.bwaves_s-891B', '621', '627'],
         ['403.gcc-16B', '410', '459', '462'],
         ['433', '456.hmmer-327B', '462', '649'],
         ['459', '462', '473', '627'],
         ['403.gcc-16B', '450', '456.hmmer-327B', '473'],
         ['403.gcc-16B', '450', '456.hmmer-327B', '462'],
         ['403.gcc-16B', '456.hmmer-327B', '621', '627'],
         ['459', '462', '482', '654.roms_s-1007B'],
         ['437', '462', '621', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '473', '621'],
         ['433', '456.hmmer-327B', '473', '654.roms_s-1007B'],
         ['459', '473', '607', '649'],
         ['434.zeusmp-10B', '456.hmmer-327B', '473', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '456.hmmer-327B', '482'],
         ['403.gcc-16B', '473', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '410', '459', '603.bwaves_s-891B'],
         ['410', '434.zeusmp-10B', '607', '621'],
         ['459', '621', '627', '649'],
         ['433', '434.zeusmp-10B', '456.hmmer-327B', '654.roms_s-1007B'],
         ['450', '456.hmmer-327B', '459', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '450', '603.bwaves_s-891B', '627'],
         ['459', '628', '649', '654.roms_s-1007B'],
         ['437', '456.hmmer-327B', '459', '473'],
         ['450', '607', '621', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '456.hmmer-327B', '482', '607'],
         ['403.gcc-16B', '410', '462', '482']
      ]

   elif apps == "hpca26c4v8":
      requested_apps = [
         ['462', '607', '621', '654.roms_s-1007B'],
         ['437', '459', '621', '649'],
         ['456.hmmer-327B', '462', '482', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '450', '621', '627'],
         ['434.zeusmp-10B', '482', '628', '649'],
         ['434.zeusmp-10B', '473', '649', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '462', '621'],
         ['434.zeusmp-10B', '456.hmmer-327B', '603.bwaves_s-891B', '628'],
         ['433', '473', '603.bwaves_s-891B', '628'],
         ['410', '433', '459', '654.roms_s-1007B'],
         ['410', '437', '450', '456.hmmer-327B'],
         ['434.zeusmp-10B', '450', '627', '628'],
         ['410', '433', '473', '621'],
         ['403.gcc-16B', '433', '473', '649'],
         ['459', '462', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '459', '607', '628'],
         ['403.gcc-16B', '433', '450', '462'],
         ['456.hmmer-327B', '607', '627', '628'],
         ['434.zeusmp-10B', '450', '462', '654.roms_s-1007B'],
         ['410', '450', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '603.bwaves_s-891B', '607', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '450', '628', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '437', '482', '621'],
         ['410', '456.hmmer-327B', '459', '603.bwaves_s-891B'],
         ['410', '433', '482', '607'],
         ['456.hmmer-327B', '459', '603.bwaves_s-891B', '607'],
         ['410', '437', '482', '621'],
         ['434.zeusmp-10B', '450', '482', '628'],
         ['410', '437', '603.bwaves_s-891B', '649'],
         ['450', '459', '462', '473'],
         ['450', '603.bwaves_s-891B', '621', '649'],
         ['410', '607', '627', '649'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '654.roms_s-1007B'],
         ['437', '456.hmmer-327B', '462', '603.bwaves_s-891B'],
         ['450', '462', '473', '628'],
         ['462', '482', '627', '628'],
         ['607', '621', '627', '649'],
         ['403.gcc-16B', '437', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '410', '434.zeusmp-10B', '462'],
         ['434.zeusmp-10B', '437', '459', '482'],
         ['410', '433', '456.hmmer-327B', '649'],
         ['456.hmmer-327B', '473', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '473', '627'],
         ['433', '473', '603.bwaves_s-891B', '627'],
         ['434.zeusmp-10B', '437', '607', '628'],
         ['403.gcc-16B', '607', '628', '649'],
         ['410', '434.zeusmp-10B', '473', '603.bwaves_s-891B'],
         ['450', '459', '628', '649'],
         ['437', '473', '482', '603.bwaves_s-891B'],
         ['456.hmmer-327B', '462', '607', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '482', '603.bwaves_s-891B', '627'],
         ['450', '462', '473', '603.bwaves_s-891B'],
         ['456.hmmer-327B', '621', '627', '654.roms_s-1007B'],
         ['437', '456.hmmer-327B', '459', '603.bwaves_s-891B'],
         ['459', '607', '628', '649'],
         ['433', '456.hmmer-327B', '462', '473'],
         ['434.zeusmp-10B', '456.hmmer-327B', '621', '649'],
         ['433', '437', '607', '621'],
         ['456.hmmer-327B', '603.bwaves_s-891B', '607', '621'],
         ['433', '607', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '621', '628'],
         ['410', '456.hmmer-327B', '482', '621'],
         ['433', '450', '482', '603.bwaves_s-891B'],
         ['433', '450', '459', '621'],
         ['434.zeusmp-10B', '437', '459', '473'],
         ['459', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['433', '473', '482', '649'],
         ['410', '603.bwaves_s-891B', '621', '628'],
         ['403.gcc-16B', '437', '607', '627'],
         ['437', '456.hmmer-327B', '627', '649'],
         ['433', '450', '473', '627'],
         ['434.zeusmp-10B', '459', '482', '627'],
         ['437', '456.hmmer-327B', '482', '654.roms_s-1007B'],
         ['459', '473', '482', '607'],
         ['433', '434.zeusmp-10B', '473', '628'],
         ['459', '603.bwaves_s-891B', '621', '628'],
         ['434.zeusmp-10B', '437', '482', '627'],
         ['450', '456.hmmer-327B', '621', '654.roms_s-1007B'],
         ['450', '456.hmmer-327B', '459', '628'],
         ['433', '434.zeusmp-10B', '459', '603.bwaves_s-891B'],
         ['403.gcc-16B', '410', '456.hmmer-327B', '654.roms_s-1007B'],
         ['410', '437', '473', '628'],
         ['410', '433', '456.hmmer-327B', '473'],
         ['433', '434.zeusmp-10B', '450', '607'],
         ['434.zeusmp-10B', '450', '628', '649'],
         ['437', '607', '621', '628'],
         ['450', '603.bwaves_s-891B', '607', '621'],
         ['459', '473', '627', '628'],
         ['434.zeusmp-10B', '437', '473', '482'],
         ['410', '456.hmmer-327B', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '607', '621', '649'],
         ['403.gcc-16B', '410', '450', '627'],
         ['459', '482', '621', '628'],
         ['434.zeusmp-10B', '459', '627', '649'],
         ['434.zeusmp-10B', '459', '473', '621'],
         ['437', '450', '482', '627'],
         ['433', '621', '628', '649'],
         ['410', '437', '482', '607'],
         ['410', '433', '603.bwaves_s-891B', '654.roms_s-1007B']
      ]

   elif apps == "hpca26c4v9":
      requested_apps = [
         ['473', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['459', '473', '607', '627'],
         ['437', '473', '627', '649'],
         ['450', '627', '628', '649'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '603.bwaves_s-891B'],
         ['433', '450', '621', '649'],
         ['437', '607', '628', '649'],
         ['403.gcc-16B', '450', '459', '473'],
         ['450', '482', '607', '627'],
         ['450', '459', '603.bwaves_s-891B', '621'],
         ['410', '456.hmmer-327B', '473', '482'],
         ['410', '459', '621', '628'],
         ['433', '456.hmmer-327B', '627', '654.roms_s-1007B'],
         ['433', '437', '482', '621'],
         ['410', '459', '462', '603.bwaves_s-891B'],
         ['433', '434.zeusmp-10B', '473', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '603.bwaves_s-891B', '621'],
         ['410', '450', '462', '627'],
         ['410', '462', '482', '603.bwaves_s-891B'],
         ['473', '607', '621', '649'],
         ['456.hmmer-327B', '482', '607', '649'],
         ['410', '437', '621', '649'],
         ['434.zeusmp-10B', '450', '473', '482'],
         ['410', '433', '434.zeusmp-10B', '482'],
         ['437', '462', '627', '628'],
         ['473', '607', '628', '649'],
         ['410', '437', '628', '654.roms_s-1007B'],
         ['459', '462', '603.bwaves_s-891B', '607'],
         ['433', '607', '621', '654.roms_s-1007B'],
         ['482', '603.bwaves_s-891B', '621', '627'],
         ['450', '607', '627', '649'],
         ['403.gcc-16B', '482', '607', '654.roms_s-1007B'],
         ['437', '456.hmmer-327B', '603.bwaves_s-891B', '628'],
         ['403.gcc-16B', '434.zeusmp-10B', '459', '482'],
         ['433', '434.zeusmp-10B', '482', '607'],
         ['437', '459', '482', '621'],
         ['403.gcc-16B', '434.zeusmp-10B', '450', '627'],
         ['437', '456.hmmer-327B', '459', '462'],
         ['459', '607', '627', '649'],
         ['437', '462', '473', '654.roms_s-1007B'],
         ['410', '433', '434.zeusmp-10B', '473'],
         ['403.gcc-16B', '482', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '410', '450', '649'],
         ['433', '450', '456.hmmer-327B', '627'],
         ['433', '437', '450', '459'],
         ['403.gcc-16B', '437', '456.hmmer-327B', '607'],
         ['433', '450', '462', '473'],
         ['403.gcc-16B', '462', '482', '627'],
         ['450', '459', '462', '649'],
         ['403.gcc-16B', '433', '450', '628'],
         ['410', '450', '621', '627'],
         ['410', '456.hmmer-327B', '462', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '459', '654.roms_s-1007B'],
         ['456.hmmer-327B', '459', '482', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '437', '450', '456.hmmer-327B'],
         ['456.hmmer-327B', '462', '482', '627'],
         ['433', '459', '473', '627'],
         ['433', '437', '621', '627'],
         ['437', '450', '621', '627'],
         ['403.gcc-16B', '456.hmmer-327B', '649', '654.roms_s-1007B'],
         ['434.zeusmp-10B', '437', '462', '621'],
         ['450', '603.bwaves_s-891B', '627', '628'],
         ['437', '607', '628', '654.roms_s-1007B'],
         ['473', '603.bwaves_s-891B', '621', '627'],
         ['434.zeusmp-10B', '437', '462', '473'],
         ['473', '603.bwaves_s-891B', '627', '649'],
         ['450', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '621', '649'],
         ['450', '456.hmmer-327B', '473', '628'],
         ['437', '456.hmmer-327B', '459', '628'],
         ['433', '456.hmmer-327B', '459', '649'],
         ['410', '434.zeusmp-10B', '473', '627'],
         ['433', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['410', '433', '437', '603.bwaves_s-891B'],
         ['434.zeusmp-10B', '462', '473', '603.bwaves_s-891B'],
         ['462', '482', '607', '654.roms_s-1007B'],
         ['433', '434.zeusmp-10B', '649', '654.roms_s-1007B'],
         ['410', '473', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '473', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '627'],
         ['434.zeusmp-10B', '437', '456.hmmer-327B', '649'],
         ['437', '456.hmmer-327B', '459', '654.roms_s-1007B'],
         ['403.gcc-16B', '603.bwaves_s-891B', '621', '628'],
         ['462', '482', '603.bwaves_s-891B', '621'],
         ['410', '437', '456.hmmer-327B', '462'],
         ['433', '456.hmmer-327B', '628', '649'],
         ['433', '437', '450', '621'],
         ['433', '462', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['410', '473', '628', '654.roms_s-1007B'],
         ['410', '437', '621', '627'],
         ['434.zeusmp-10B', '459', '473', '654.roms_s-1007B'],
         ['433', '434.zeusmp-10B', '459', '649'],
         ['456.hmmer-327B', '482', '621', '627'],
         ['450', '459', '603.bwaves_s-891B', '627'],
         ['437', '456.hmmer-327B', '473', '627'],
         ['450', '459', '473', '654.roms_s-1007B'],
         ['410', '434.zeusmp-10B', '459', '482'],
         ['437', '482', '607', '627'],
         ['403.gcc-16B', '450', '462', '473']
      ]
   
   elif apps == "hpca26c4v10":
      requested_apps = [
         ['433', '462', '473', '628'],
         ['403.gcc-16B', '462', '607', '649'],
         ['473', '607', '627', '649'],
         ['456.hmmer-327B', '482', '621', '649'],
         ['403.gcc-16B', '473', '482', '621'],
         ['482', '621', '628', '649'],
         ['403.gcc-16B', '462', '607', '621'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '473'],
         ['433', '473', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '482', '654.roms_s-1007B'],
         ['403.gcc-16B', '627', '628', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '482'],
         ['473', '482', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '482', '649'],
         ['433', '456.hmmer-327B', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '462', '649'],
         ['456.hmmer-327B', '482', '628', '654.roms_s-1007B'],
         ['462', '482', '607', '649'],
         ['433', '456.hmmer-327B', '482', '627'],
         ['403.gcc-16B', '433', '627', '628'],
         ['403.gcc-16B', '607', '627', '654.roms_s-1007B'],
         ['433', '473', '627', '628'],
         ['403.gcc-16B', '473', '628', '649'],
         ['403.gcc-16B', '433', '607', '649'],
         ['433', '462', '473', '649'],
         ['462', '482', '621', '627'],
         ['403.gcc-16B', '433', '621', '628'],
         ['403.gcc-16B', '621', '627', '649'],
         ['462', '473', '482', '621'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '649'],
         ['462', '473', '607', '627'],
         ['473', '627', '628', '654.roms_s-1007B'],
         ['473', '621', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '627', '628', '649'],
         ['607', '621', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '621', '627'],
         ['433', '456.hmmer-327B', '482', '621'],
         ['621', '628', '649', '654.roms_s-1007B'],
         ['462', '607', '628', '654.roms_s-1007B'],
         ['433', '627', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '621', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '473', '654.roms_s-1007B'],
         ['456.hmmer-327B', '607', '621', '627'],
         ['456.hmmer-327B', '473', '482', '607'],
         ['462', '473', '607', '628'],
         ['482', '621', '649', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '462', '607'],
         ['456.hmmer-327B', '627', '649', '654.roms_s-1007B'],
         ['607', '628', '649', '654.roms_s-1007B'],
         ['462', '473', '621', '627'],
         ['473', '628', '649', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '621', '654.roms_s-1007B'],
         ['433', '462', '482', '628'],
         ['433', '473', '621', '627'],
         ['473', '607', '627', '628'],
         ['433', '482', '607', '649'],
         ['403.gcc-16B', '462', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '607', '621', '627'],
         ['456.hmmer-327B', '462', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '654.roms_s-1007B'],
         ['456.hmmer-327B', '628', '649', '654.roms_s-1007B'],
         ['433', '462', '627', '628'],
         ['433', '462', '627', '654.roms_s-1007B'],
         ['433', '482', '627', '649'],
         ['462', '473', '482', '654.roms_s-1007B'],
         ['403.gcc-16B', '473', '482', '627'],
         ['433', '473', '607', '621'],
         ['482', '607', '627', '654.roms_s-1007B'],
         ['482', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '621'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '482'],
         ['403.gcc-16B', '473', '621', '627'],
         ['456.hmmer-327B', '607', '628', '649'],
         ['456.hmmer-327B', '462', '627', '649'],
         ['462', '473', '607', '649'],
         ['473', '482', '607', '649'],
         ['433', '473', '482', '621'],
         ['403.gcc-16B', '482', '628', '654.roms_s-1007B'],
         ['433', '462', '473', '607'],
         ['462', '482', '628', '649'],
         ['462', '473', '482', '607'],
         ['433', '456.hmmer-327B', '607', '628'],
         ['433', '456.hmmer-327B', '473', '627'],
         ['433', '482', '621', '628'],
         ['482', '607', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '473', '621'],
         ['403.gcc-16B', '462', '627', '628'],
         ['456.hmmer-327B', '473', '607', '627'],
         ['462', '628', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '627', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '482', '654.roms_s-1007B'],
         ['482', '607', '621', '627'],
         ['403.gcc-16B', '473', '621', '628'],
         ['433', '473', '482', '654.roms_s-1007B'],
         ['433', '462', '473', '482'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '649'],
         ['403.gcc-16B', '433', '473', '621'],
         ['627', '628', '649', '654.roms_s-1007B'],
         ['462', '473', '628', '649']
      ]
   
   elif apps == "hpca26c4v11":
      requested_apps = [
         ['456.hmmer-327B', '473', '482', '621'],
         ['433', '456.hmmer-327B', '621', '627'],
         ['482', '621', '627', '654.roms_s-1007B'],
         ['433', '473', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '482', '628'],
         ['462', '473', '627', '628'],
         ['607', '627', '628', '654.roms_s-1007B'],
         ['433', '462', '473', '627'],
         ['403.gcc-16B', '433', '473', '654.roms_s-1007B'],
         ['403.gcc-16B', '473', '482', '649'],
         ['462', '473', '482', '628'],
         ['456.hmmer-327B', '482', '621', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '607', '628'],
         ['403.gcc-16B', '462', '473', '607'],
         ['403.gcc-16B', '473', '621', '649'],
         ['403.gcc-16B', '482', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '628', '649'],
         ['403.gcc-16B', '482', '627', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '607', '649'],
         ['403.gcc-16B', '462', '607', '628'],
         ['403.gcc-16B', '462', '473', '482'],
         ['403.gcc-16B', '607', '621', '628'],
         ['403.gcc-16B', '462', '473', '654.roms_s-1007B'],
         ['433', '607', '627', '654.roms_s-1007B'],
         ['473', '607', '621', '654.roms_s-1007B'],
         ['433', '473', '607', '628'],
         ['403.gcc-16B', '627', '628', '654.roms_s-1007B'],
         ['462', '473', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '462', '482'],
         ['456.hmmer-327B', '482', '607', '628'],
         ['473', '482', '621', '627'],
         ['456.hmmer-327B', '621', '649', '654.roms_s-1007B'],
         ['433', '473', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '473', '627', '654.roms_s-1007B'],
         ['433', '462', '607', '627'],
         ['462', '482', '607', '627'],
         ['403.gcc-16B', '456.hmmer-327B', '607', '621'],
         ['433', '456.hmmer-327B', '607', '627'],
         ['433', '473', '607', '649'],
         ['473', '482', '621', '649'],
         ['433', '473', '621', '628'],
         ['456.hmmer-327B', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '482', '654.roms_s-1007B'],
         ['456.hmmer-327B', '482', '627', '649'],
         ['456.hmmer-327B', '473', '607', '649'],
         ['433', '627', '628', '649'],
         ['403.gcc-16B', '607', '621', '654.roms_s-1007B'],
         ['473', '482', '649', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '473', '628'],
         ['403.gcc-16B', '473', '607', '628'],
         ['482', '627', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '627', '628'],
         ['462', '473', '621', '628'],
         ['621', '627', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '621', '649'],
         ['433', '456.hmmer-327B', '462', '627'],
         ['403.gcc-16B', '433', '462', '628'],
         ['456.hmmer-327B', '462', '473', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '607', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '621', '649'],
         ['433', '607', '627', '628'],
         ['433', '456.hmmer-327B', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '607'],
         ['403.gcc-16B', '482', '621', '627'],
         ['403.gcc-16B', '433', '473', '482'],
         ['433', '482', '627', '654.roms_s-1007B'],
         ['433', '462', '621', '649'],
         ['433', '621', '627', '649'],
         ['403.gcc-16B', '462', '627', '649'],
         ['482', '627', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '627', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '482', '621'],
         ['462', '627', '628', '654.roms_s-1007B'],
         ['433', '607', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '473', '607'],
         ['403.gcc-16B', '462', '628', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '627'],
         ['433', '627', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '621', '628'],
         ['462', '473', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '473', '627'],
         ['456.hmmer-327B', '462', '482', '621'],
         ['403.gcc-16B', '607', '628', '654.roms_s-1007B'],
         ['433', '462', '621', '627'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '621'],
         ['403.gcc-16B', '621', '628', '654.roms_s-1007B'],
         ['473', '482', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '473', '627', '628'],
         ['482', '607', '621', '628'],
         ['403.gcc-16B', '433', '621', '649'],
         ['403.gcc-16B', '482', '607', '649'],
         ['403.gcc-16B', '462', '607', '654.roms_s-1007B'],
         ['462', '482', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '621', '649'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '621', '654.roms_s-1007B'],
         ['473', '482', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '649', '654.roms_s-1007B']
      ]
   
   elif apps == "hpca26c4v12":
      requested_apps = [
         ['433', '456.hmmer-327B', '607', '621'],
         ['433', '473', '482', '607'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '607'],
         ['403.gcc-16B', '462', '627', '654.roms_s-1007B'],
         ['473', '621', '628', '649'],
         ['456.hmmer-327B', '473', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '482', '628'],
         ['462', '607', '621', '649'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '607'],
         ['433', '462', '621', '628'],
         ['456.hmmer-327B', '473', '621', '627'],
         ['403.gcc-16B', '456.hmmer-327B', '627', '649'],
         ['403.gcc-16B', '462', '621', '649'],
         ['433', '473', '482', '627'],
         ['403.gcc-16B', '433', '607', '654.roms_s-1007B'],
         ['433', '462', '628', '649'],
         ['607', '621', '628', '649'],
         ['462', '607', '628', '649'],
         ['462', '607', '621', '627'],
         ['456.hmmer-327B', '482', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '628', '654.roms_s-1007B'],
         ['462', '621', '627', '649'],
         ['473', '482', '607', '628'],
         ['403.gcc-16B', '473', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '607', '627', '628'],
         ['433', '621', '628', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '482', '649'],
         ['403.gcc-16B', '462', '473', '649'],
         ['482', '607', '621', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '473', '621'],
         ['456.hmmer-327B', '482', '621', '628'],
         ['462', '621', '649', '654.roms_s-1007B'],
         ['607', '621', '627', '628'],
         ['403.gcc-16B', '473', '482', '628'],
         ['473', '482', '627', '628'],
         ['433', '462', '607', '621'],
         ['403.gcc-16B', '433', '462', '621'],
         ['403.gcc-16B', '482', '621', '628'],
         ['433', '456.hmmer-327B', '473', '482'],
         ['433', '456.hmmer-327B', '473', '607'],
         ['473', '482', '621', '628'],
         ['482', '621', '627', '649'],
         ['433', '462', '482', '607'],
         ['403.gcc-16B', '607', '627', '649'],
         ['607', '627', '649', '654.roms_s-1007B'],
         ['433', '482', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '607', '621'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '473'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '607'],
         ['473', '621', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '482', '627'],
         ['462', '473', '621', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '627', '628'],
         ['462', '482', '627', '654.roms_s-1007B'],
         ['433', '621', '627', '628'],
         ['473', '607', '627', '654.roms_s-1007B'],
         ['456.hmmer-327B', '473', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '482', '607'],
         ['473', '621', '627', '628'],
         ['433', '462', '627', '649'],
         ['433', '462', '607', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '607', '627'],
         ['456.hmmer-327B', '473', '621', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '628', '649'],
         ['462', '482', '621', '649'],
         ['462', '473', '607', '654.roms_s-1007B'],
         ['462', '627', '628', '649'],
         ['403.gcc-16B', '473', '482', '607'],
         ['433', '473', '627', '654.roms_s-1007B'],
         ['473', '482', '628', '649'],
         ['473', '482', '621', '654.roms_s-1007B'],
         ['433', '462', '607', '628'],
         ['473', '607', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '621'],
         ['462', '621', '627', '654.roms_s-1007B'],
         ['433', '482', '621', '627'],
         ['456.hmmer-327B', '462', '607', '627'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '462'],
         ['403.gcc-16B', '473', '628', '654.roms_s-1007B'],
         ['621', '627', '649', '654.roms_s-1007B'],
         ['433', '473', '627', '649'],
         ['456.hmmer-327B', '462', '482', '628'],
         ['403.gcc-16B', '433', '482', '627'],
         ['403.gcc-16B', '433', '462', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '473', '628'],
         ['433', '628', '649', '654.roms_s-1007B'],
         ['462', '473', '607', '621'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '621'],
         ['403.gcc-16B', '462', '621', '627'],
         ['456.hmmer-327B', '473', '627', '628'],
         ['456.hmmer-327B', '462', '607', '621'],
         ['462', '473', '482', '649'],
         ['433', '621', '649', '654.roms_s-1007B'],
         ['433', '473', '621', '649'],
         ['433', '462', '482', '621'],
         ['433', '462', '482', '654.roms_s-1007B'],
         ['433', '473', '482', '628'],
         ['403.gcc-16B', '482', '607', '628'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '627'],
         ['433', '607', '627', '649']
      ]
   
   elif apps == "hpca26c4v13":
      requested_apps = [
         ['462', '603.bwaves_s-891B', '628', '654.roms_s-1007B'],
         ['433', '473', '603.bwaves_s-891B', '607'],
         ['433', '462', '482', '627'],
         ['462', '603.bwaves_s-891B', '607', '649'],
         ['456.hmmer-327B', '473', '607', '654.roms_s-1007B'],
         ['473', '482', '603.bwaves_s-891B', '649'],
         ['433', '482', '603.bwaves_s-891B', '607'],
         ['433', '607', '621', '628'],
         ['456.hmmer-327B', '473', '621', '628'],
         ['403.gcc-16B', '473', '627', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['473', '627', '649', '654.roms_s-1007B'],
         ['603.bwaves_s-891B', '628', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '603.bwaves_s-891B', '627'],
         ['403.gcc-16B', '482', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '473', '603.bwaves_s-891B'],
         ['462', '482', '603.bwaves_s-891B', '627'],
         ['603.bwaves_s-891B', '621', '628', '649'],
         ['456.hmmer-327B', '473', '603.bwaves_s-891B', '607'],
         ['456.hmmer-327B', '473', '627', '649'],
         ['603.bwaves_s-891B', '621', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '462', '603.bwaves_s-891B', '628'],
         ['603.bwaves_s-891B', '607', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '473', '603.bwaves_s-891B'],
         ['462', '473', '603.bwaves_s-891B', '628'],
         ['462', '603.bwaves_s-891B', '621', '627'],
         ['473', '603.bwaves_s-891B', '628', '649'],
         ['403.gcc-16B', '603.bwaves_s-891B', '628', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '628', '654.roms_s-1007B'],
         ['433', '603.bwaves_s-891B', '621', '649'],
         ['433', '603.bwaves_s-891B', '607', '628'],
         ['603.bwaves_s-891B', '607', '627', '649'],
         ['403.gcc-16B', '456.hmmer-327B', '482', '649'],
         ['456.hmmer-327B', '462', '603.bwaves_s-891B', '627'],
         ['462', '482', '607', '621'],
         ['403.gcc-16B', '603.bwaves_s-891B', '621', '654.roms_s-1007B'],
         ['403.gcc-16B', '603.bwaves_s-891B', '607', '649'],
         ['462', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '603.bwaves_s-891B'],
         ['482', '603.bwaves_s-891B', '628', '649'],
         ['403.gcc-16B', '473', '603.bwaves_s-891B', '621'],
         ['456.hmmer-327B', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '603.bwaves_s-891B', '621'],
         ['403.gcc-16B', '456.hmmer-327B', '462', '649'],
         ['462', '603.bwaves_s-891B', '607', '621'],
         ['462', '603.bwaves_s-891B', '621', '649'],
         ['482', '627', '628', '649'],
         ['456.hmmer-327B', '473', '607', '621'],
         ['433', '482', '603.bwaves_s-891B', '628'],
         ['462', '482', '603.bwaves_s-891B', '607'],
         ['603.bwaves_s-891B', '621', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['462', '473', '621', '654.roms_s-1007B'],
         ['473', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['433', '603.bwaves_s-891B', '607', '621'],
         ['433', '607', '628', '649'],
         ['403.gcc-16B', '462', '603.bwaves_s-891B', '621'],
         ['473', '482', '603.bwaves_s-891B', '628'],
         ['456.hmmer-327B', '473', '628', '649'],
         ['403.gcc-16B', '433', '456.hmmer-327B', '603.bwaves_s-891B'],
         ['462', '607', '649', '654.roms_s-1007B'],
         ['482', '603.bwaves_s-891B', '607', '628'],
         ['462', '473', '482', '627'],
         ['462', '603.bwaves_s-891B', '607', '628'],
         ['403.gcc-16B', '456.hmmer-327B', '607', '627'],
         ['403.gcc-16B', '456.hmmer-327B', '603.bwaves_s-891B', '607'],
         ['433', '462', '473', '621'],
         ['403.gcc-16B', '462', '482', '654.roms_s-1007B'],
         ['462', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '628', '649', '654.roms_s-1007B'],
         ['403.gcc-16B', '456.hmmer-327B', '603.bwaves_s-891B', '628'],
         ['403.gcc-16B', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['473', '603.bwaves_s-891B', '607', '628'],
         ['482', '603.bwaves_s-891B', '607', '654.roms_s-1007B'],
         ['433', '603.bwaves_s-891B', '607', '649'],
         ['456.hmmer-327B', '482', '628', '649'],
         ['473', '482', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '433', '603.bwaves_s-891B', '628'],
         ['482', '603.bwaves_s-891B', '627', '654.roms_s-1007B'],
         ['433', '473', '603.bwaves_s-891B', '621'],
         ['403.gcc-16B', '603.bwaves_s-891B', '627', '628'],
         ['607', '621', '649', '654.roms_s-1007B'],
         ['462', '482', '649', '654.roms_s-1007B'],
         ['456.hmmer-327B', '462', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['403.gcc-16B', '603.bwaves_s-891B', '649', '654.roms_s-1007B'],
         ['621', '627', '628', '649'],
         ['462', '473', '627', '654.roms_s-1007B'],
         ['433', '482', '607', '628'],
         ['456.hmmer-327B', '482', '603.bwaves_s-891B', '628'],
         ['433', '473', '603.bwaves_s-891B', '654.roms_s-1007B'],
         ['433', '456.hmmer-327B', '473', '649'],
         ['462', '473', '603.bwaves_s-891B', '649'],
         ['403.gcc-16B', '462', '603.bwaves_s-891B', '607'],
         ['473', '603.bwaves_s-891B', '621', '654.roms_s-1007B'],
         ['473', '482', '603.bwaves_s-891B', '627'],
         ['403.gcc-16B', '433', '627', '649'],
         ['403.gcc-16B', '473', '603.bwaves_s-891B', '628'],
         ['603.bwaves_s-891B', '607', '621', '649'],
         ['603.bwaves_s-891B', '621', '628', '654.roms_s-1007B'],
         ['403.gcc-16B', '482', '603.bwaves_s-891B', '628']
      ]
   
   elif apps == "hpca26c4v14":
      requested_apps = [
         ['456.hmmer-327B', '473', 'compute_int_751', 'srv776'],
         ['compute_fp_37', 'compute_fp_59', 'srv132', 'srv366'],
         ['456.hmmer-327B', '621', 'compute_fp_101', 'srv279'],
         ['628', '649', '654.roms_s-1007B', 'compute_fp_124'],
         ['compute_int_28', 'compute_int_751', 'srv137', 'srv650'],
         ['433', '654.roms_s-1007B', 'srv676', 'srv69'],
         ['compute_fp_111', 'compute_int_28', 'srv366', 'srv394'],
         ['compute_fp_75', 'compute_fp_96', 'srv132', 'srv242'],
         ['compute_fp_59', 'compute_int_751', 'srv394', 'srv64'],
         ['456.hmmer-327B', '628', 'compute_fp_59', 'srv160'],
         ['607', 'srv160', 'srv440', 'srv717'],
         ['433', '654.roms_s-1007B', 'srv242', 'srv366'],
         ['654.roms_s-1007B', 'compute_fp_59', 'compute_int_751', 'srv650'],
         ['compute_fp_101', 'compute_int_751', 'srv279', 'srv650'],
         ['433', '462', '628', 'srv676'],
         ['649', 'compute_fp_101', 'compute_fp_111', 'srv776'],
         ['621', 'compute_fp_75', 'compute_int_28', 'srv64'],
         ['456.hmmer-327B', 'compute_int_28', 'srv137', 'srv461'],
         ['628', 'compute_fp_96', 'compute_int_751', 'srv262'],
         ['456.hmmer-327B', '462', '607', 'compute_fp_57'],
         ['654.roms_s-1007B', 'srv366', 'srv69', 'srv776'],
         ['433', 'srv440', 'srv64', 'srv69'],
         ['456.hmmer-327B', 'compute_fp_124', 'srv676', 'srv776'],
         ['456.hmmer-327B', 'compute_fp_96', 'srv160', 'srv440'],
         ['473', 'compute_fp_101', 'compute_fp_124', 'compute_fp_37'],
         ['482', '603.bwaves_s-891B', '654.roms_s-1007B', 'srv160'],
         ['compute_fp_37', 'compute_fp_96', 'srv509', 'srv650'],
         ['473', '628', '654.roms_s-1007B', 'compute_fp_75'],
         ['462', 'compute_int_751', 'srv767', 'srv776'],
         ['433', '462', 'srv279', 'srv767'],
         ['compute_fp_111', 'compute_fp_37', 'srv366', 'srv650'],
         ['456.hmmer-327B', '627', 'compute_fp_124', 'srv717'],
         ['403.gcc-16B', 'compute_fp_105', 'compute_int_28', 'srv137'],
         ['compute_fp_57', 'compute_fp_96', 'srv366', 'srv509'],
         ['627', 'compute_fp_96', 'srv366', 'srv64'],
         ['462', '607', '621', 'srv160'],
         ['compute_fp_101', 'srv137', 'srv69', 'srv717'],
         ['628', 'compute_fp_111', 'compute_fp_75', 'srv717'],
         ['433', '473', '628', 'compute_fp_37'],
         ['433', '621', '654.roms_s-1007B', 'compute_fp_57'],
         ['srv242', 'srv440', 'srv69', 'srv767'],
         ['482', '621', 'compute_fp_124', 'compute_fp_57'],
         ['621', 'compute_fp_4', 'srv509', 'srv717'],
         ['462', 'compute_fp_105', 'compute_fp_4', 'srv64'],
         ['462', 'compute_fp_37', 'compute_fp_57', 'srv262'],
         ['649', 'compute_fp_96', 'srv509', 'srv717'],
         ['srv366', 'srv394', 'srv602', 'srv650'],
         ['603.bwaves_s-891B', 'compute_fp_75', 'compute_int_751', 'srv767'],
         ['482', '627', '654.roms_s-1007B', 'compute_fp_4'],
         ['433', '456.hmmer-327B', '607', 'srv132'],
         ['compute_fp_37', 'srv69', 'srv767', 'srv776'],
         ['456.hmmer-327B', '462', 'srv394', 'srv602'],
         ['compute_fp_124', 'compute_fp_37', 'srv160', 'srv602'],
         ['403.gcc-16B', 'compute_fp_75', 'srv132', 'srv650'],
         ['compute_fp_96', 'srv242', 'srv394', 'srv509'],
         ['628', 'compute_fp_105', 'srv650', 'srv717'],
         ['603.bwaves_s-891B', '628', 'compute_fp_111', 'srv767'],
         ['462', 'compute_int_28', 'srv242', 'srv279'],
         ['456.hmmer-327B', '621', '628', 'srv461'],
         ['433', '627', 'srv132', 'srv394'],
         ['482', 'compute_fp_111', 'srv132', 'srv676'],
         ['607', '628', 'compute_int_751', 'srv776'],
         ['482', 'srv160', 'srv242', 'srv717'],
         ['621', 'compute_fp_75', 'srv461', 'srv717'],
         ['482', 'compute_fp_59', 'srv132', 'srv776'],
         ['srv279', 'srv509', 'srv676', 'srv717'],
         ['456.hmmer-327B', '649', 'compute_fp_57', 'srv69'],
         ['607', '649', 'srv160', 'srv461'],
         ['607', 'compute_fp_37', 'srv650', 'srv69'],
         ['403.gcc-16B', '473', '482', 'srv776'],
         ['627', '654.roms_s-1007B', 'compute_fp_105', 'srv366'],
         ['628', 'compute_fp_111', 'compute_fp_4', 'srv602'],
         ['403.gcc-16B', '621', 'srv394', 'srv650'],
         ['649', 'srv160', 'srv602', 'srv64'],
         ['compute_fp_37', 'compute_int_28', 'srv440', 'srv509'],
         ['621', 'compute_fp_111', 'srv242', 'srv440'],
         ['628', 'srv279', 'srv440', 'srv776'],
         ['compute_fp_124', 'compute_fp_57', 'srv160', 'srv650'],
         ['403.gcc-16B', '627', 'compute_fp_124', 'srv602'],
         ['627', 'srv262', 'srv461', 'srv767'],
         ['628', 'srv132', 'srv279', 'srv676'],
         ['compute_fp_111', 'compute_fp_124', 'compute_fp_59', 'compute_int_751'],
         ['compute_fp_111', 'compute_int_28', 'srv69', 'srv717'],
         ['compute_fp_59', 'srv137', 'srv242', 'srv676'],
         ['603.bwaves_s-891B', '621', 'compute_fp_4', 'compute_fp_59'],
         ['403.gcc-16B', '607', 'srv279', 'srv64'],
         ['462', '654.roms_s-1007B', 'compute_fp_124', 'srv69'],
         ['621', '649', 'srv366', 'srv64'],
         ['srv262', 'srv366', 'srv717', 'srv776'],
         ['627', 'compute_fp_4', 'compute_fp_75', 'srv132'],
         ['403.gcc-16B', '462', '654.roms_s-1007B', 'compute_fp_124'],
         ['628', 'compute_fp_105', 'srv69', 'srv767'],
         ['462', '627', 'compute_fp_4', 'srv509'],
         ['482', '628', '649', 'srv242'],
         ['649', 'compute_fp_37', 'srv366', 'srv461'],
         ['403.gcc-16B', '621', '649', 'srv132'],
         ['621', 'srv279', 'srv394', 'srv440'],
         ['403.gcc-16B', 'srv262', 'srv650', 'srv776'],
         ['654.roms_s-1007B', 'compute_fp_4', 'srv242', 'srv602'],
         ['627', '649', 'compute_int_751', 'srv69']
      ]
   
   elif apps == "hpca26c4v15":
      requested_apps = [
         ['compute_fp_105', 'compute_fp_75', 'srv132', 'srv279'],
         ['403.gcc-16B', 'compute_fp_75', 'srv160', 'srv69'],
         ['456.hmmer-327B', 'compute_fp_96', 'srv440', 'srv509'],
         ['433', 'compute_fp_75', 'srv279', 'srv776'],
         ['482', 'compute_fp_124', 'srv160', 'srv776'],
         ['compute_fp_101', 'compute_fp_37', 'compute_fp_75', 'srv132'],
         ['482', '603.bwaves_s-891B', '654.roms_s-1007B', 'compute_fp_105'],
         ['compute_fp_101', 'compute_fp_111', 'compute_fp_124', 'srv776'],
         ['473', 'compute_fp_4', 'compute_fp_59', 'srv602'],
         ['403.gcc-16B', 'compute_fp_96', 'srv650', 'srv676'],
         ['649', 'compute_fp_96', 'srv676', 'srv717'],
         ['433', '462', 'srv440', 'srv776'],
         ['403.gcc-16B', 'srv262', 'srv279', 'srv602'],
         ['456.hmmer-327B', '627', 'srv366', 'srv676'],
         ['482', 'compute_fp_111', 'srv440', 'srv650'],
         ['607', '627', 'srv137', 'srv64'],
         ['compute_fp_37', 'compute_fp_75', 'srv137', 'srv64'],
         ['473', 'compute_int_28', 'srv509', 'srv717'],
         ['603.bwaves_s-891B', 'srv262', 'srv602', 'srv767'],
         ['433', '628', 'compute_fp_4', 'srv440'],
         ['607', '628', 'srv440', 'srv717'],
         ['456.hmmer-327B', '627', 'srv394', 'srv602'],
         ['607', 'compute_int_28', 'srv717', 'srv776'],
         ['compute_int_28', 'compute_int_751', 'srv366', 'srv509'],
         ['628', 'compute_fp_57', 'compute_fp_59', 'srv767'],
         ['462', '482', 'srv440', 'srv69'],
         ['473', 'compute_fp_4', 'srv717', 'srv767'],
         ['433', 'compute_fp_111', 'compute_int_751', 'srv366'],
         ['compute_fp_124', 'compute_fp_57', 'srv160', 'srv602'],
         ['482', '654.roms_s-1007B', 'compute_int_751', 'srv650'],
         ['603.bwaves_s-891B', 'compute_int_751', 'srv64', 'srv717'],
         ['621', 'compute_fp_37', 'compute_fp_96', 'srv279'],
         ['433', 'compute_fp_101', 'compute_int_751', 'srv676'],
         ['607', 'srv394', 'srv440', 'srv461'],
         ['654.roms_s-1007B', 'compute_fp_57', 'srv676', 'srv776'],
         ['482', 'compute_fp_37', 'srv262', 'srv366'],
         ['433', '462', '649', 'srv394'],
         ['462', '482', '628', 'srv776'],
         ['compute_fp_4', 'srv160', 'srv440', 'srv69'],
         ['403.gcc-16B', 'compute_fp_124', 'compute_fp_96', 'srv242'],
         ['462', 'compute_fp_101', 'compute_fp_105', 'srv262'],
         ['603.bwaves_s-891B', 'compute_fp_37', 'compute_int_28', 'srv69'],
         ['621', '654.roms_s-1007B', 'compute_fp_57', 'compute_fp_59'],
         ['482', '607', 'srv262', 'srv64'],
         ['403.gcc-16B', '627', 'srv767', 'srv776'],
         ['628', 'compute_int_751', 'srv366', 'srv440'],
         ['649', 'srv242', 'srv366', 'srv509'],
         ['srv262', 'srv279', 'srv509', 'srv64'],
         ['649', 'compute_fp_124', 'compute_fp_37', 'srv717'],
         ['462', 'compute_int_751', 'srv509', 'srv64'],
         ['462', 'compute_fp_101', 'compute_fp_4', 'srv132'],
         ['628', 'compute_fp_37', 'srv242', 'srv776'],
         ['403.gcc-16B', 'compute_fp_57', 'srv160', 'srv242'],
         ['482', '649', 'compute_fp_96', 'srv262'],
         ['654.roms_s-1007B', 'compute_fp_111', 'srv242', 'srv717'],
         ['403.gcc-16B', '627', 'compute_fp_59', 'srv440'],
         ['649', 'compute_fp_124', 'srv64', 'srv767'],
         ['433', '649', 'compute_fp_75', 'srv676'],
         ['403.gcc-16B', '628', 'compute_int_28', 'srv461'],
         ['649', 'compute_fp_59', 'srv279', 'srv676'],
         ['603.bwaves_s-891B', '628', 'compute_fp_124', 'compute_fp_59'],
         ['456.hmmer-327B', 'compute_fp_59', 'compute_fp_75', 'srv160'],
         ['compute_fp_4', 'compute_fp_59', 'srv137', 'srv262'],
         ['628', 'compute_fp_101', 'srv279', 'srv440'],
         ['473', '649', 'compute_fp_59', 'srv366'],
         ['compute_fp_59', 'compute_int_28', 'srv160', 'srv717'],
         ['433', '456.hmmer-327B', 'srv279', 'srv650'],
         ['627', 'compute_fp_111', 'compute_fp_124', 'compute_fp_59'],
         ['649', 'compute_fp_111', 'compute_fp_37', 'srv279'],
         ['628', 'compute_fp_37', 'srv461', 'srv676'],
         ['compute_fp_111', 'compute_fp_4', 'compute_int_751', 'srv262'],
         ['456.hmmer-327B', '628', 'srv64', 'srv69'],
         ['433', '473', 'compute_fp_124', 'srv262'],
         ['621', 'compute_fp_105', 'compute_fp_4', 'compute_fp_96'],
         ['compute_fp_124', 'compute_fp_96', 'srv366', 'srv776'],
         ['607', 'compute_fp_57', 'compute_int_28', 'srv242'],
         ['473', '607', '654.roms_s-1007B', 'compute_fp_59'],
         ['403.gcc-16B', 'srv650', 'srv717', 'srv776'],
         ['649', 'compute_fp_105', 'srv279', 'srv394'],
         ['649', 'srv242', 'srv717', 'srv776'],
         ['462', '607', '628', 'compute_fp_75'],
         ['433', '621', '627', 'srv242'],
         ['473', 'compute_fp_111', 'srv242', 'srv64'],
         ['627', 'srv242', 'srv461', 'srv767'],
         ['compute_fp_4', 'compute_int_28', 'srv160', 'srv676'],
         ['627', 'compute_fp_105', 'compute_fp_111', 'srv242'],
         ['607', 'compute_fp_101', 'srv440', 'srv717'],
         ['compute_fp_105', 'compute_fp_111', 'compute_fp_37', 'compute_int_28'],
         ['456.hmmer-327B', 'compute_fp_59', 'compute_int_751', 'srv394'],
         ['607', '628', 'compute_fp_59', 'srv440'],
         ['srv262', 'srv394', 'srv69', 'srv717'],
         ['482', 'compute_fp_105', 'srv461', 'srv717'],
         ['403.gcc-16B', '654.roms_s-1007B', 'srv160', 'srv776'],
         ['607', 'compute_fp_111', 'srv137', 'srv776'],
         ['607', 'compute_fp_101', 'compute_fp_105', 'srv137'],
         ['433', 'compute_fp_4', 'srv262', 'srv366'],
         ['473', 'srv137', 'srv440', 'srv676'],
         ['473', 'compute_fp_101', 'compute_fp_124', 'compute_fp_75'],
         ['462', '603.bwaves_s-891B', 'srv137', 'srv160'],
         ['403.gcc-16B', '473', 'compute_fp_59', 'srv717']
      ]
   
   elif apps == "hpca26c4v16":
      requested_apps = [
         ['482', '654.roms_s-1007B', 'compute_fp_37', 'compute_int_28'],
         ['compute_fp_105', 'compute_fp_59', 'compute_fp_96', 'srv137'],
         ['621', '649', 'compute_fp_37', 'compute_fp_57'],
         ['456.hmmer-327B', 'compute_int_751', 'srv132', 'srv509'],
         ['456.hmmer-327B', '473', 'srv509', 'srv64'],
         ['482', 'srv132', 'srv279', 'srv676'],
         ['compute_fp_105', 'compute_fp_75', 'compute_int_751', 'srv64'],
         ['603.bwaves_s-891B', 'compute_fp_59', 'srv242', 'srv767'],
         ['462', '627', 'srv676', 'srv69'],
         ['compute_fp_105', 'srv262', 'srv366', 'srv394'],
         ['433', 'compute_fp_105', 'compute_fp_124', 'srv440'],
         ['607', 'compute_fp_111', 'compute_fp_57', 'srv676'],
         ['627', 'compute_fp_105', 'compute_fp_37', 'srv509'],
         ['compute_fp_105', 'compute_int_751', 'srv160', 'srv242'],
         ['433', 'compute_fp_59', 'compute_fp_96', 'srv461'],
         ['473', 'compute_fp_111', 'srv279', 'srv440'],
         ['607', 'compute_fp_105', 'compute_fp_4', 'srv137'],
         ['603.bwaves_s-891B', 'compute_fp_57', 'srv132', 'srv279'],
         ['compute_fp_105', 'compute_fp_75', 'srv717', 'srv767'],
         ['603.bwaves_s-891B', '649', 'compute_fp_96', 'srv440'],
         ['482', 'srv262', 'srv366', 'srv602'],
         ['433', '627', 'srv262', 'srv767'],
         ['403.gcc-16B', '456.hmmer-327B', 'srv461', 'srv64'],
         ['627', '654.roms_s-1007B', 'srv461', 'srv767'],
         ['compute_fp_96', 'srv137', 'srv394', 'srv69'],
         ['433', 'srv132', 'srv262', 'srv366'],
         ['607', 'compute_fp_37', 'srv279', 'srv776'],
         ['607', 'compute_fp_111', 'compute_fp_37', 'compute_fp_4'],
         ['603.bwaves_s-891B', 'compute_fp_96', 'compute_int_751', 'srv776'],
         ['462', 'compute_fp_105', 'compute_fp_96', 'srv767'],
         ['621', '649', 'compute_fp_59', 'srv69'],
         ['456.hmmer-327B', 'compute_fp_101', 'compute_fp_96', 'srv676'],
         ['compute_fp_124', 'compute_fp_96', 'srv132', 'srv366'],
         ['compute_fp_111', 'srv262', 'srv440', 'srv602'],
         ['462', 'compute_fp_75', 'srv262', 'srv602'],
         ['473', 'srv137', 'srv394', 'srv650'],
         ['compute_fp_105', 'compute_fp_111', 'compute_fp_75', 'srv717'],
         ['607', '621', 'srv64', 'srv776'],
         ['649', 'compute_fp_105', 'compute_fp_75', 'srv132'],
         ['456.hmmer-327B', '654.roms_s-1007B', 'compute_fp_96', 'srv602'],
         ['649', 'compute_fp_111', 'srv366', 'srv509'],
         ['654.roms_s-1007B', 'compute_fp_75', 'srv160', 'srv650'],
         ['603.bwaves_s-891B', '627', 'compute_fp_101', 'compute_fp_57'],
         ['compute_fp_59', 'srv279', 'srv440', 'srv676'],
         ['649', 'compute_fp_105', 'srv262', 'srv461'],
         ['603.bwaves_s-891B', '607', 'compute_fp_124', 'srv602'],
         ['627', 'srv394', 'srv676', 'srv69'],
         ['654.roms_s-1007B', 'compute_fp_105', 'compute_fp_111', 'srv366'],
         ['462', '607', 'compute_fp_124', 'srv440'],
         ['compute_fp_75', 'srv160', 'srv279', 'srv509'],
         ['649', 'compute_fp_124', 'srv279', 'srv394'],
         ['456.hmmer-327B', '628', 'compute_fp_105', 'srv440'],
         ['621', 'compute_fp_101', 'srv132', 'srv717'],
         ['628', 'compute_int_28', 'srv676', 'srv69'],
         ['433', '628', 'compute_fp_96', 'srv650'],
         ['462', '607', 'compute_fp_4', 'srv279'],
         ['compute_fp_105', 'compute_fp_37', 'compute_fp_59', 'srv509'],
         ['compute_fp_111', 'srv132', 'srv509', 'srv650'],
         ['srv132', 'srv160', 'srv279', 'srv509'],
         ['403.gcc-16B', '654.roms_s-1007B', 'compute_int_28', 'srv776'],
         ['482', '628', 'compute_fp_105', 'srv160'],
         ['403.gcc-16B', 'compute_fp_96', 'compute_int_751', 'srv650'],
         ['compute_fp_124', 'srv509', 'srv69', 'srv717'],
         ['628', 'compute_fp_124', 'srv509', 'srv676'],
         ['433', '627', 'compute_fp_75', 'compute_fp_96'],
         ['473', 'compute_fp_111', 'srv767', 'srv776'],
         ['607', 'compute_fp_59', 'srv132', 'srv440'],
         ['607', 'compute_fp_4', 'srv132', 'srv767'],
         ['482', 'compute_fp_37', 'srv64', 'srv767'],
         ['456.hmmer-327B', 'compute_fp_59', 'srv440', 'srv69'],
         ['compute_fp_37', 'compute_fp_57', 'compute_fp_59', 'compute_fp_75'],
         ['403.gcc-16B', '628', 'compute_fp_111', 'srv160'],
         ['456.hmmer-327B', '627', '649', 'compute_fp_57'],
         ['473', '621', '628', 'compute_int_751'],
         ['403.gcc-16B', '654.roms_s-1007B', 'compute_fp_4', 'srv160'],
         ['654.roms_s-1007B', 'compute_fp_111', 'compute_fp_124', 'srv394'],
         ['654.roms_s-1007B', 'srv132', 'srv279', 'srv64'],
         ['compute_fp_37', 'srv366', 'srv602', 'srv650'],
         ['607', 'srv366', 'srv440', 'srv676'],
         ['compute_fp_57', 'srv279', 'srv394', 'srv767'],
         ['627', '628', 'compute_fp_96', 'srv509'],
         ['compute_fp_105', 'compute_fp_96', 'compute_int_751', 'srv602'],
         ['473', 'compute_fp_124', 'compute_fp_57', 'srv461'],
         ['473', '607', '628', 'srv767'],
         ['462', 'compute_fp_105', 'compute_fp_124', 'compute_fp_4'],
         ['462', 'compute_fp_4', 'srv440', 'srv602'],
         ['603.bwaves_s-891B', '649', 'srv602', 'srv676'],
         ['654.roms_s-1007B', 'compute_fp_101', 'compute_fp_75', 'srv132'],
         ['628', 'compute_fp_96', 'compute_int_28', 'srv602'],
         ['srv160', 'srv602', 'srv676', 'srv69'],
         ['456.hmmer-327B', 'srv440', 'srv602', 'srv717'],
         ['456.hmmer-327B', 'compute_fp_101', 'srv160', 'srv776'],
         ['456.hmmer-327B', '482', '621', 'srv69'],
         ['649', 'compute_fp_124', 'srv440', 'srv767'],
         ['compute_fp_37', 'compute_fp_96', 'srv132', 'srv767'],
         ['compute_fp_111', 'srv262', 'srv366', 'srv676'],
         ['433', 'compute_fp_59', 'compute_fp_96', 'srv279'],
         ['627', 'compute_fp_75', 'srv132', 'srv440'],
         ['462', 'compute_fp_59', 'srv242', 'srv717'],
         ['433', '462', 'srv461', 'srv776']
      ]
   
   elif apps == "hpca26c4v17":
      requested_apps = [
         ['456.hmmer-327B', '473', 'compute_fp_75', 'compute_int_28'],
         ['649', 'compute_fp_111', 'srv461', 'srv676'],
         ['compute_fp_37', 'compute_fp_96', 'srv160', 'srv394'],
         ['621', 'srv366', 'srv440', 'srv767'],
         ['403.gcc-16B', '456.hmmer-327B', 'compute_fp_111', 'compute_fp_59'],
         ['compute_fp_4', 'compute_int_28', 'srv242', 'srv394'],
         ['607', 'compute_fp_75', 'srv242', 'srv676'],
         ['compute_fp_101', 'srv160', 'srv602', 'srv69'],
         ['403.gcc-16B', '433', 'compute_int_751', 'srv137'],
         ['403.gcc-16B', '462', 'srv509', 'srv767'],
         ['433', 'compute_fp_124', 'srv394', 'srv776'],
         ['433', 'compute_fp_111', 'srv64', 'srv69'],
         ['compute_fp_111', 'srv366', 'srv394', 'srv509'],
         ['compute_fp_4', 'srv242', 'srv461', 'srv776'],
         ['628', 'compute_fp_37', 'compute_fp_96', 'srv242'],
         ['473', '649', 'compute_fp_96', 'srv69'],
         ['462', 'srv132', 'srv242', 'srv262'],
         ['srv160', 'srv279', 'srv650', 'srv676'],
         ['433', '482', 'compute_int_28', 'srv64'],
         ['649', 'srv132', 'srv137', 'srv64'],
         ['628', 'compute_fp_96', 'compute_int_28', 'srv394'],
         ['482', '603.bwaves_s-891B', '627', 'compute_fp_111'],
         ['603.bwaves_s-891B', '654.roms_s-1007B', 'compute_int_28', 'srv394'],
         ['433', 'compute_fp_4', 'srv137', 'srv160'],
         ['compute_int_28', 'srv242', 'srv366', 'srv64'],
         ['456.hmmer-327B', '473', '482', 'compute_fp_101'],
         ['403.gcc-16B', 'compute_fp_4', 'compute_fp_57', 'srv279'],
         ['403.gcc-16B', 'compute_fp_124', 'compute_fp_37', 'compute_fp_96'],
         ['462', '654.roms_s-1007B', 'srv160', 'srv509'],
         ['473', '482', 'compute_fp_111', 'srv461'],
         ['649', 'srv366', 'srv461', 'srv509'],
         ['607', 'compute_fp_59', 'compute_int_28', 'srv279'],
         ['654.roms_s-1007B', 'srv440', 'srv767', 'srv776'],
         ['compute_fp_101', 'compute_fp_4', 'srv132', 'srv64'],
         ['456.hmmer-327B', 'compute_fp_105', 'srv262', 'srv461'],
         ['621', '654.roms_s-1007B', 'compute_fp_101', 'srv394'],
         ['649', 'compute_fp_96', 'srv461', 'srv64'],
         ['462', '607', 'compute_fp_111', 'compute_int_28'],
         ['compute_fp_96', 'srv366', 'srv440', 'srv717'],
         ['654.roms_s-1007B', 'compute_fp_4', 'srv509', 'srv602'],
         ['433', '482', 'compute_fp_105', 'compute_fp_4'],
         ['621', 'compute_fp_59', 'srv242', 'srv440'],
         ['603.bwaves_s-891B', 'srv160', 'srv676', 'srv69'],
         ['403.gcc-16B', 'srv160', 'srv279', 'srv602'],
         ['473', 'compute_fp_37', 'srv461', 'srv509'],
         ['621', 'compute_fp_57', 'compute_fp_96', 'srv650'],
         ['607', 'compute_fp_105', 'compute_fp_4', 'srv242'],
         ['403.gcc-16B', 'compute_int_751', 'srv137', 'srv279'],
         ['621', '654.roms_s-1007B', 'compute_fp_101', 'compute_int_751'],
         ['607', 'compute_fp_111', 'compute_int_28', 'srv394'],
         ['627', 'compute_int_751', 'srv366', 'srv776'],
         ['473', 'srv366', 'srv509', 'srv602'],
         ['compute_fp_96', 'srv440', 'srv461', 'srv650'],
         ['403.gcc-16B', 'srv160', 'srv440', 'srv461'],
         ['607', 'compute_fp_59', 'srv132', 'srv160'],
         ['compute_fp_75', 'srv262', 'srv394', 'srv676'],
         ['649', 'compute_int_28', 'srv650', 'srv69'],
         ['603.bwaves_s-891B', 'srv440', 'srv767', 'srv776'],
         ['627', 'compute_fp_124', 'srv602', 'srv64'],
         ['compute_fp_57', 'srv137', 'srv767', 'srv776'],
         ['456.hmmer-327B', '627', 'compute_fp_57', 'srv440'],
         ['compute_fp_105', 'compute_fp_57', 'srv602', 'srv676'],
         ['compute_fp_111', 'compute_fp_59', 'srv394', 'srv64'],
         ['628', '654.roms_s-1007B', 'srv509', 'srv650'],
         ['649', 'compute_fp_37', 'compute_fp_57', 'compute_fp_75'],
         ['603.bwaves_s-891B', 'compute_fp_59', 'srv160', 'srv650'],
         ['compute_fp_57', 'compute_int_751', 'srv366', 'srv394'],
         ['482', '621', 'compute_fp_105', 'compute_fp_75'],
         ['compute_fp_101', 'srv394', 'srv64', 'srv69'],
         ['482', 'compute_fp_59', 'srv160', 'srv767'],
         ['compute_fp_96', 'srv137', 'srv160', 'srv461'],
         ['srv242', 'srv394', 'srv440', 'srv64'],
         ['compute_fp_4', 'compute_fp_96', 'srv440', 'srv650'],
         ['482', '654.roms_s-1007B', 'compute_fp_96', 'srv69'],
         ['srv160', 'srv509', 'srv64', 'srv650'],
         ['compute_fp_96', 'srv137', 'srv366', 'srv509'],
         ['628', 'compute_fp_105', 'srv279', 'srv602'],
         ['482', 'compute_fp_111', 'compute_fp_75', 'compute_fp_96'],
         ['603.bwaves_s-891B', 'compute_int_751', 'srv262', 'srv676'],
         ['627', 'compute_fp_37', 'compute_fp_75', 'srv676'],
         ['603.bwaves_s-891B', 'srv160', 'srv279', 'srv776'],
         ['621', 'compute_int_751', 'srv137', 'srv602'],
         ['compute_fp_4', 'srv242', 'srv440', 'srv461'],
         ['473', 'srv602', 'srv64', 'srv69'],
         ['649', 'compute_fp_105', 'compute_int_28', 'srv137'],
         ['433', 'srv394', 'srv676', 'srv776'],
         ['462', 'compute_fp_101', 'compute_fp_96', 'compute_int_28'],
         ['compute_fp_105', 'srv509', 'srv676', 'srv767'],
         ['compute_fp_101', 'srv137', 'srv64', 'srv69'],
         ['compute_fp_124', 'compute_fp_4', 'srv676', 'srv767'],
         ['603.bwaves_s-891B', 'compute_fp_101', 'srv132', 'srv64'],
         ['627', '628', 'compute_fp_101', 'compute_fp_124'],
         ['603.bwaves_s-891B', '621', 'srv394', 'srv717'],
         ['607', 'compute_fp_124', 'compute_fp_59', 'srv64'],
         ['compute_fp_37', 'srv137', 'srv242', 'srv650'],
         ['649', 'srv132', 'srv394', 'srv717'],
         ['compute_fp_105', 'compute_fp_4', 'srv64', 'srv676'],
         ['482', '649', 'compute_fp_4', 'srv64'],
         ['628', 'compute_fp_37', 'srv160', 'srv717'],
         ['482', '603.bwaves_s-891B', 'srv279', 'srv461']
      ]

   elif apps == "hpca26c4v18":
      requested_apps = [
         ['628', 'compute_fp_101', 'srv64', 'srv69'],
         ['473', 'compute_fp_111', 'srv242', 'srv650'],
         ['462', '621', 'compute_fp_124', 'compute_fp_75'],
         ['403.gcc-16B', 'compute_fp_101', 'compute_fp_75', 'srv650'],
         ['compute_fp_101', 'compute_fp_75', 'compute_fp_96', 'srv717'],
         ['compute_fp_37', 'compute_fp_75', 'srv461', 'srv767'],
         ['462', '621', 'compute_fp_124', 'compute_fp_37'],
         ['compute_int_28', 'srv602', 'srv64', 'srv676'],
         ['627', 'compute_fp_111', 'srv366', 'srv394'],
         ['403.gcc-16B', '627', 'compute_fp_57', 'srv461'],
         ['compute_fp_105', 'srv242', 'srv64', 'srv650'],
         ['621', '654.roms_s-1007B', 'compute_fp_105', 'compute_fp_111'],
         ['compute_fp_101', 'compute_fp_37', 'compute_fp_96', 'srv461'],
         ['627', 'compute_fp_101', 'srv160', 'srv461'],
         ['compute_fp_105', 'compute_fp_124', 'srv440', 'srv767'],
         ['433', '456.hmmer-327B', '628', 'compute_fp_37'],
         ['compute_fp_111', 'compute_fp_124', 'srv279', 'srv676'],
         ['473', '607', '654.roms_s-1007B', 'compute_fp_111'],
         ['603.bwaves_s-891B', 'compute_fp_96', 'srv366', 'srv650'],
         ['433', '621', '654.roms_s-1007B', 'srv262'],
         ['compute_fp_101', 'compute_fp_59', 'srv440', 'srv676'],
         ['compute_fp_4', 'compute_int_751', 'srv132', 'srv461'],
         ['473', '628', 'compute_fp_59', 'compute_fp_75'],
         ['403.gcc-16B', 'compute_fp_101', 'compute_fp_96', 'srv242'],
         ['482', '649', 'srv132', 'srv717'],
         ['628', 'compute_fp_105', 'srv440', 'srv776'],
         ['621', 'srv160', 'srv262', 'srv776'],
         ['627', 'compute_fp_105', 'compute_fp_124', 'compute_fp_75'],
         ['403.gcc-16B', '482', 'compute_fp_124', 'srv717'],
         ['482', 'srv262', 'srv279', 'srv602'],
         ['433', '654.roms_s-1007B', 'compute_fp_124', 'srv767'],
         ['456.hmmer-327B', '607', 'srv461', 'srv650'],
         ['compute_fp_111', 'compute_fp_59', 'compute_fp_96', 'srv602'],
         ['462', '603.bwaves_s-891B', '627', 'srv69'],
         ['607', 'compute_fp_4', 'compute_fp_96', 'srv64'],
         ['433', '607', 'compute_fp_4', 'srv767'],
         ['462', '607', '627', 'srv160'],
         ['compute_int_751', 'srv132', 'srv461', 'srv64'],
         ['473', 'compute_fp_57', 'srv461', 'srv676'],
         ['621', 'compute_fp_124', 'compute_fp_37', 'srv461'],
         ['compute_fp_105', 'srv366', 'srv394', 'srv650'],
         ['403.gcc-16B', 'compute_fp_37', 'srv242', 'srv262'],
         ['603.bwaves_s-891B', '621', 'compute_fp_37', 'srv394'],
         ['627', '628', 'srv132', 'srv509'],
         ['473', 'compute_fp_75', 'srv366', 'srv394'],
         ['649', 'compute_fp_57', 'srv279', 'srv676'],
         ['456.hmmer-327B', '628', 'srv132', 'srv279'],
         ['621', '654.roms_s-1007B', 'compute_int_751', 'srv767'],
         ['456.hmmer-327B', '603.bwaves_s-891B', 'srv279', 'srv776'],
         ['compute_fp_124', 'srv279', 'srv440', 'srv602'],
         ['473', '607', 'compute_fp_57', 'srv64'],
         ['456.hmmer-327B', 'compute_fp_111', 'srv132', 'srv137'],
         ['433', 'compute_fp_4', 'srv279', 'srv650'],
         ['403.gcc-16B', '621', '649', 'srv650'],
         ['649', 'srv242', 'srv64', 'srv676'],
         ['603.bwaves_s-891B', '628', '649', 'compute_fp_96'],
         ['621', '627', 'compute_fp_96', 'srv602'],
         ['compute_fp_57', 'compute_int_28', 'srv262', 'srv69'],
         ['654.roms_s-1007B', 'compute_fp_37', 'srv461', 'srv767'],
         ['603.bwaves_s-891B', '607', 'compute_fp_96', 'srv394'],
         ['621', 'compute_fp_59', 'compute_int_28', 'srv776'],
         ['621', 'compute_fp_101', 'srv262', 'srv676'],
         ['628', '649', 'compute_fp_101', 'srv767'],
         ['628', 'compute_fp_124', 'srv132', 'srv279'],
         ['627', 'compute_fp_124', 'compute_fp_96', 'srv602'],
         ['403.gcc-16B', '433', 'srv394', 'srv717'],
         ['456.hmmer-327B', 'compute_fp_57', 'srv509', 'srv69'],
         ['compute_fp_37', 'compute_fp_59', 'srv137', 'srv776'],
         ['654.roms_s-1007B', 'srv132', 'srv676', 'srv767'],
         ['compute_fp_101', 'srv132', 'srv137', 'srv767'],
         ['627', 'compute_fp_105', 'compute_int_28', 'srv262'],
         ['628', 'compute_fp_75', 'srv262', 'srv650'],
         ['456.hmmer-327B', '473', 'srv461', 'srv650'],
         ['628', 'compute_fp_105', 'compute_fp_124', 'srv461'],
         ['compute_int_751', 'srv137', 'srv160', 'srv676'],
         ['462', '473', 'compute_int_28', 'srv262'],
         ['473', '621', 'srv602', 'srv69'],
         ['433', '621', '649', 'srv650'],
         ['621', 'compute_fp_111', 'compute_fp_59', 'compute_int_751'],
         ['456.hmmer-327B', '621', '627', 'srv394'],
         ['462', '649', 'srv137', 'srv676'],
         ['srv132', 'srv137', 'srv461', 'srv64'],
         ['compute_fp_111', 'compute_fp_75', 'srv69', 'srv776'],
         ['462', '482', '621', 'srv717'],
         ['456.hmmer-327B', 'srv132', 'srv461', 'srv64'],
         ['403.gcc-16B', '603.bwaves_s-891B', '621', 'srv461'],
         ['compute_fp_105', 'compute_fp_4', 'compute_int_28', 'srv717'],
         ['403.gcc-16B', '607', '628', 'srv717'],
         ['628', 'srv132', 'srv64', 'srv69'],
         ['compute_fp_105', 'compute_fp_37', 'compute_fp_59', 'srv676'],
         ['compute_int_751', 'srv137', 'srv440', 'srv602'],
         ['654.roms_s-1007B', 'compute_fp_124', 'compute_fp_96', 'srv64'],
         ['603.bwaves_s-891B', 'compute_fp_96', 'srv242', 'srv279'],
         ['456.hmmer-327B', 'compute_fp_101', 'compute_fp_96', 'compute_int_28'],
         ['621', 'srv132', 'srv262', 'srv69'],
         ['473', 'compute_fp_57', 'compute_fp_96', 'srv366'],
         ['607', '654.roms_s-1007B', 'compute_fp_105', 'compute_fp_96'],
         ['456.hmmer-327B', 'compute_fp_111', 'srv262', 'srv509'],
         ['456.hmmer-327B', 'compute_fp_101', 'compute_fp_75', 'srv394'],
         ['compute_fp_59', 'srv440', 'srv602', 'srv767']
      ]

   elif apps == "hpca26c4v19":
      requested_apps = [
         ['403.gcc-16B', '482', 'compute_fp_4', 'srv132'],
         ['603.bwaves_s-891B', '621', 'compute_fp_101', 'compute_int_751'],
         ['456.hmmer-327B', '627', 'compute_int_28', 'srv137'],
         ['627', 'compute_fp_4', 'srv394', 'srv461'],
         ['462', '482', 'srv279', 'srv509'],
         ['482', 'compute_fp_57', 'compute_fp_75', 'compute_int_751'],
         ['462', 'compute_fp_59', 'srv461', 'srv602'],
         ['compute_fp_4', 'srv132', 'srv137', 'srv440'],
         ['621', 'compute_int_28', 'srv132', 'srv602'],
         ['compute_fp_105', 'compute_fp_37', 'srv394', 'srv602'],
         ['473', '649', 'compute_fp_59', 'compute_int_751'],
         ['607', '621', 'srv160', 'srv767'],
         ['compute_fp_101', 'compute_fp_75', 'srv160', 'srv69'],
         ['compute_fp_59', 'compute_int_28', 'srv242', 'srv461'],
         ['473', 'compute_fp_75', 'compute_int_28', 'srv440'],
         ['srv242', 'srv262', 'srv366', 'srv650'],
         ['456.hmmer-327B', '473', '649', 'srv69'],
         ['473', 'compute_fp_57', 'compute_fp_59', 'srv64'],
         ['403.gcc-16B', '462', '473', 'srv440'],
         ['462', '654.roms_s-1007B', 'compute_fp_4', 'srv776'],
         ['462', '649', 'compute_int_28', 'srv676'],
         ['654.roms_s-1007B', 'compute_fp_75', 'srv602', 'srv64'],
         ['649', 'compute_fp_57', 'srv160', 'srv64'],
         ['433', 'compute_fp_57', 'compute_fp_96', 'srv776'],
         ['627', 'compute_fp_37', 'compute_fp_96', 'srv137'],
         ['compute_fp_105', 'compute_fp_37', 'compute_fp_4', 'srv366'],
         ['456.hmmer-327B', '462', '654.roms_s-1007B', 'compute_fp_101'],
         ['462', '627', 'compute_fp_75', 'compute_int_28'],
         ['456.hmmer-327B', '607', '654.roms_s-1007B', 'compute_fp_111'],
         ['627', 'compute_int_28', 'srv394', 'srv767'],
         ['compute_fp_96', 'srv160', 'srv440', 'srv64'],
         ['621', 'compute_int_28', 'srv242', 'srv509'],
         ['compute_fp_4', 'srv132', 'srv394', 'srv676'],
         ['403.gcc-16B', '649', 'compute_fp_37', 'srv602'],
         ['403.gcc-16B', '462', '603.bwaves_s-891B', 'compute_fp_124'],
         ['compute_fp_101', 'srv137', 'srv602', 'srv776'],
         ['628', 'compute_fp_37', 'srv767', 'srv776'],
         ['621', 'compute_int_28', 'srv160', 'srv440'],
         ['compute_fp_105', 'compute_fp_37', 'compute_fp_75', 'srv242'],
         ['649', 'srv242', 'srv262', 'srv279'],
         ['621', 'compute_fp_105', 'compute_int_751', 'srv366'],
         ['compute_fp_59', 'srv242', 'srv394', 'srv602'],
         ['628', '654.roms_s-1007B', 'compute_fp_37', 'srv69'],
         ['627', 'compute_fp_4', 'compute_fp_57', 'srv461'],
         ['627', 'compute_fp_37', 'compute_fp_59', 'srv776'],
         ['456.hmmer-327B', '603.bwaves_s-891B', '649', 'compute_int_751'],
         ['607', '649', 'srv160', 'srv776'],
         ['627', '649', 'compute_fp_101', 'compute_fp_124'],
         ['654.roms_s-1007B', 'compute_fp_4', 'compute_fp_75', 'srv440'],
         ['603.bwaves_s-891B', 'compute_fp_105', 'srv160', 'srv366'],
         ['473', '654.roms_s-1007B', 'srv132', 'srv650'],
         ['compute_fp_124', 'srv132', 'srv242', 'srv366'],
         ['403.gcc-16B', '649', '654.roms_s-1007B', 'compute_fp_75'],
         ['403.gcc-16B', '649', 'compute_fp_75', 'srv132'],
         ['607', 'compute_fp_101', 'srv394', 'srv776'],
         ['654.roms_s-1007B', 'compute_fp_4', 'compute_fp_96', 'compute_int_751'],
         ['482', 'compute_fp_105', 'srv160', 'srv767'],
         ['607', '621', '628', 'compute_fp_57'],
         ['473', 'compute_fp_75', 'compute_fp_96', 'srv717'],
         ['433', '462', '628', 'compute_fp_57'],
         ['473', 'srv137', 'srv279', 'srv776'],
         ['403.gcc-16B', '456.hmmer-327B', 'compute_fp_111', 'srv776'],
         ['456.hmmer-327B', '654.roms_s-1007B', 'compute_fp_75', 'srv461'],
         ['compute_fp_96', 'srv366', 'srv509', 'srv69'],
         ['462', '628', 'compute_fp_57', 'srv132'],
         ['403.gcc-16B', 'compute_fp_75', 'srv160', 'srv650'],
         ['403.gcc-16B', '456.hmmer-327B', '649', 'compute_int_751'],
         ['462', 'compute_fp_59', 'compute_int_751', 'srv394'],
         ['649', 'compute_fp_4', 'compute_fp_96', 'srv69'],
         ['462', '628', 'srv461', 'srv602'],
         ['compute_fp_57', 'compute_int_751', 'srv394', 'srv69'],
         ['compute_fp_57', 'compute_fp_59', 'compute_fp_75', 'srv366'],
         ['473', 'compute_fp_57', 'compute_int_751', 'srv650'],
         ['403.gcc-16B', '607', 'compute_fp_75', 'srv132'],
         ['482', '621', 'compute_fp_37', 'srv394'],
         ['compute_fp_37', 'compute_fp_4', 'srv279', 'srv64'],
         ['compute_fp_37', 'compute_fp_75', 'srv132', 'srv676'],
         ['compute_fp_101', 'compute_fp_37', 'srv440', 'srv767'],
         ['compute_fp_105', 'compute_fp_124', 'srv509', 'srv69'],
         ['621', '628', 'srv509', 'srv767'],
         ['603.bwaves_s-891B', 'srv242', 'srv366', 'srv509'],
         ['403.gcc-16B', '433', '621', 'compute_fp_75'],
         ['srv262', 'srv440', 'srv509', 'srv602'],
         ['compute_fp_105', 'compute_fp_37', 'srv64', 'srv69'],
         ['compute_int_751', 'srv137', 'srv279', 'srv767'],
         ['compute_fp_59', 'compute_fp_75', 'srv676', 'srv776'],
         ['621', '654.roms_s-1007B', 'srv394', 'srv602'],
         ['462', 'compute_fp_37', 'compute_int_28', 'srv64'],
         ['compute_fp_59', 'srv132', 'srv279', 'srv69'],
         ['654.roms_s-1007B', 'compute_fp_124', 'srv394', 'srv717'],
         ['compute_fp_37', 'srv64', 'srv650', 'srv767'],
         ['649', 'compute_fp_59', 'srv132', 'srv366'],
         ['654.roms_s-1007B', 'compute_int_28', 'compute_int_751', 'srv262'],
         ['603.bwaves_s-891B', '654.roms_s-1007B', 'srv650', 'srv69'],
         ['compute_fp_101', 'compute_fp_105', 'srv461', 'srv717'],
         ['433', '462', '482', 'compute_fp_37'],
         ['621', 'compute_fp_75', 'srv132', 'srv137'],
         ['compute_fp_4', 'compute_fp_96', 'srv137', 'srv366'],
         ['621', 'compute_fp_4', 'compute_int_28', 'srv279'],
         ['srv160', 'srv262', 'srv64', 'srv650']
      ]
   
   elif apps == "hpca26c4v20":
      requested_apps = [
         ['compute_fp_100', 'srv132', 'srv394', 'srv470'],
         ['compute_fp_100', 'compute_fp_124', 'compute_fp_4', 'srv137'],
         ['403.gcc-16B', 'compute_fp_111', 'srv262', 'srv767'],
         ['627', 'compute_fp_96', 'srv58', 'srv637'],
         ['621', 'compute_fp_57', 'srv137', 'srv637'],
         ['403.gcc-16B', 'srv602', 'srv637', 'srv69'],
         ['403.gcc-16B', '621', '627', 'srv137'],
         ['compute_fp_100', 'srv242', 'srv279', 'srv509'],
         ['649', 'srv160', 'srv717', 'srv78'],
         ['compute_fp_124', 'compute_fp_37', 'srv137', 'srv279'],
         ['482', 'compute_fp_96', 'srv676', 'srv717'],
         ['compute_fp_124', 'compute_fp_61', 'srv394', 'srv58'],
         ['649', 'srv394', 'srv64', 'srv78'],
         ['627', 'compute_fp_12', 'compute_fp_4', 'srv64'],
         ['627', 'compute_fp_111', 'srv137', 'srv470'],
         ['compute_fp_111', 'compute_fp_124', 'srv262', 'srv776'],
         ['649', '654.roms_s-1007B', 'compute_fp_105', 'srv64'],
         ['compute_fp_111', 'compute_fp_57', 'srv470', 'srv509'],
         ['compute_int_28', 'compute_int_751', 'srv394', 'srv64'],
         ['compute_fp_59', 'compute_fp_61', 'compute_int_105srv366', 'srv160'],
         ['compute_fp_4', 'compute_fp_57', 'compute_fp_59', 'srv637'],
         ['compute_fp_57', 'srv637', 'srv650', 'srv717'],
         ['433', '649', 'compute_fp_124', 'srv64'],
         ['482', 'compute_fp_4', 'compute_int_28', 'srv767'],
         ['compute_fp_57', 'srv440', 'srv650', 'srv767'],
         ['621', 'srv602', 'srv69', 'srv78'],
         ['627', 'srv470', 'srv637', 'srv717'],
         ['433', 'srv676', 'srv776', 'srv78'],
         ['compute_fp_96', 'srv262', 'srv279', 'srv767'],
         ['compute_fp_59', 'srv262', 'srv470', 'srv602'],
         ['621', '654.roms_s-1007B', 'srv279', 'srv461'],
         ['compute_fp_4', 'compute_int_751', 'srv767', 'srv78'],
         ['compute_fp_100', 'compute_fp_111', 'compute_fp_37', 'srv461'],
         ['compute_fp_59', 'srv279', 'srv676', 'srv776'],
         ['482', 'compute_int_105srv366', 'srv137', 'srv637'],
         ['621', 'compute_fp_101', 'srv242', 'srv69'],
         ['482', 'compute_fp_111', 'compute_fp_75', 'srv650'],
         ['621', 'compute_fp_57', 'compute_fp_59', 'compute_int_751'],
         ['621', 'compute_fp_100', 'srv637', 'srv64'],
         ['compute_fp_100', 'compute_int_105srv366', 'srv650', 'srv69'],
         ['compute_fp_111', 'compute_fp_12', 'srv509', 'srv64'],
         ['compute_fp_101', 'compute_fp_111', 'srv132', 'srv650'],
         ['649', 'compute_fp_57', 'srv58', 'srv717'],
         ['433', 'compute_fp_59', 'compute_fp_61', 'srv64'],
         ['627', 'srv137', 'srv461', 'srv767'],
         ['621', 'compute_fp_111', 'srv461', 'srv58'],
         ['compute_fp_96', 'compute_int_105srv366', 'compute_int_28', 'srv132'],
         ['compute_fp_124', 'compute_fp_57', 'srv64', 'srv78'],
         ['compute_fp_59', 'srv394', 'srv470', 'srv602'],
         ['srv137', 'srv509', 'srv676', 'srv767'],
         ['compute_fp_4', 'compute_fp_57', 'compute_int_28', 'srv509'],
         ['srv160', 'srv242', 'srv394', 'srv440'],
         ['482', 'compute_fp_101', 'compute_int_28', 'srv650'],
         ['621', 'srv137', 'srv637', 'srv78'],
         ['srv160', 'srv242', 'srv637', 'srv650'],
         ['srv394', 'srv440', 'srv767', 'srv78'],
         ['482', 'compute_fp_111', 'compute_fp_75', 'srv470'],
         ['621', 'srv58', 'srv637', 'srv767'],
         ['compute_int_751', 'srv160', 'srv509', 'srv767'],
         ['compute_fp_37', 'compute_int_751', 'srv394', 'srv461'],
         ['627', 'compute_fp_105', 'compute_fp_12', 'compute_int_751'],
         ['482', 'compute_fp_100', 'srv394', 'srv440'],
         ['compute_fp_101', 'srv132', 'srv394', 'srv717'],
         ['compute_fp_4', 'compute_int_28', 'srv242', 'srv64'],
         ['621', '649', '654.roms_s-1007B', 'srv69'],
         ['621', 'compute_fp_111', 'srv160', 'srv69'],
         ['621', 'compute_fp_124', 'compute_fp_75', 'srv58'],
         ['482', 'compute_fp_100', 'compute_fp_37', 'srv717'],
         ['403.gcc-16B', 'compute_fp_37', 'srv394', 'srv78'],
         ['621', 'compute_fp_37', 'compute_fp_4', 'srv69'],
         ['srv279', 'srv394', 'srv602', 'srv78'],
         ['srv132', 'srv676', 'srv717', 'srv78'],
         ['compute_fp_111', 'compute_int_105srv366', 'srv262', 'srv637'],
         ['compute_fp_101', 'compute_fp_12', 'srv262', 'srv279'],
         ['482', 'compute_fp_57', 'srv637', 'srv776'],
         ['compute_fp_4', 'compute_int_751', 'srv132', 'srv676'],
         ['compute_fp_57', 'compute_fp_59', 'compute_fp_96', 'srv69'],
         ['621', 'compute_int_751', 'srv132', 'srv58'],
         ['654.roms_s-1007B', 'compute_fp_12', 'srv137', 'srv676'],
         ['compute_fp_96', 'srv132', 'srv137', 'srv78'],
         ['649', 'compute_fp_12', 'compute_fp_37', 'compute_fp_61'],
         ['compute_fp_96', 'compute_int_28', 'srv440', 'srv470'],
         ['compute_fp_100', 'compute_fp_12', 'compute_fp_37', 'srv262'],
         ['compute_fp_100', 'compute_fp_59', 'srv262', 'srv650'],
         ['compute_fp_105', 'srv137', 'srv279', 'srv637'],
         ['482', '621', 'compute_fp_61', 'srv394'],
         ['compute_int_751', 'srv137', 'srv637', 'srv69'],
         ['compute_fp_101', 'compute_fp_37', 'compute_fp_96', 'compute_int_28'],
         ['403.gcc-16B', 'compute_fp_105', 'compute_int_28', 'srv160'],
         ['433', 'compute_fp_59', 'srv461', 'srv69'],
         ['649', 'srv242', 'srv64', 'srv767'],
         ['compute_fp_111', 'srv69', 'srv767', 'srv776'],
         ['compute_fp_111', 'srv132', 'srv470', 'srv602'],
         ['482', 'compute_fp_101', 'compute_fp_111', 'compute_fp_124'],
         ['compute_fp_37', 'srv160', 'srv440', 'srv637'],
         ['compute_fp_37', 'srv509', 'srv676', 'srv776'],
         ['621', 'compute_int_751', 'srv470', 'srv69'],
         ['srv242', 'srv650', 'srv676', 'srv69'],
         ['compute_fp_100', 'compute_fp_75', 'compute_fp_96', 'srv64'],
         ['654.roms_s-1007B', 'compute_fp_61', 'srv461', 'srv767'],
         ['627', 'compute_fp_75', 'compute_int_28', 'srv717'],
         ['srv132', 'srv242', 'srv470', 'srv650'],
         ['621', 'compute_fp_12', 'compute_fp_96', 'srv637'],
         ['403.gcc-16B', '433', 'compute_fp_59', 'srv64'],
         ['621', 'srv394', 'srv440', 'srv676'],
         ['srv279', 'srv509', 'srv602', 'srv717'],
         ['compute_int_751', 'srv160', 'srv64', 'srv676'],
         ['srv132', 'srv440', 'srv676', 'srv776'],
         ['compute_fp_100', 'srv160', 'srv602', 'srv637'],
         ['compute_fp_111', 'srv137', 'srv242', 'srv394'],
         ['compute_fp_101', 'compute_fp_37', 'srv160', 'srv676'],
         ['649', 'compute_fp_111', 'compute_fp_57', 'srv262'],
         ['627', 'compute_fp_4', 'compute_fp_57', 'compute_int_751'],
         ['srv160', 'srv262', 'srv676', 'srv776'],
         ['482', 'compute_fp_61', 'compute_fp_75', 'srv461'],
         ['403.gcc-16B', 'compute_fp_96', 'srv509', 'srv602'],
         ['621', 'compute_fp_12', 'compute_fp_59', 'srv509'],
         ['compute_fp_12', 'compute_fp_61', 'srv69', 'srv776'],
         ['433', 'compute_fp_105', 'compute_fp_12', 'srv242'],
         ['627', 'compute_fp_12', 'compute_int_105srv366', 'srv470'],
         ['compute_fp_96', 'srv242', 'srv394', 'srv717'],
         ['compute_fp_57', 'srv262', 'srv440', 'srv602'],
         ['compute_fp_100', 'compute_fp_124', 'srv262', 'srv394'],
         ['433', 'compute_int_28', 'compute_int_751', 'srv509'],
         ['srv132', 'srv137', 'srv58', 'srv637'],
         ['627', 'compute_fp_37', 'srv137', 'srv717'],
         ['compute_fp_12', 'srv279', 'srv64', 'srv767'],
         ['compute_int_28', 'srv637', 'srv64', 'srv78'],
         ['482', 'srv132', 'srv440', 'srv69'],
         ['compute_fp_105', 'compute_fp_37', 'srv262', 'srv676'],
         ['compute_fp_12', 'srv262', 'srv440', 'srv637'],
         ['649', '654.roms_s-1007B', 'compute_fp_111', 'srv137'],
         ['627', 'srv160', 'srv676', 'srv767'],
         ['649', 'compute_fp_96', 'srv602', 'srv64'],
         ['compute_fp_100', 'compute_fp_12', 'srv132', 'srv78'],
         ['627', 'compute_int_105srv366', 'srv394', 'srv470'],
         ['compute_int_751', 'srv132', 'srv394', 'srv64'],
         ['627', 'srv137', 'srv242', 'srv394'],
         ['compute_fp_101', 'srv279', 'srv676', 'srv717'],
         ['649', 'compute_fp_124', 'srv58', 'srv69'],
         ['482', 'srv394', 'srv602', 'srv676'],
         ['433', 'compute_fp_61', 'srv160', 'srv637'],
         ['compute_fp_100', 'compute_fp_101', 'srv279', 'srv461'],
         ['compute_fp_111', 'srv132', 'srv461', 'srv650'],
         ['compute_fp_37', 'compute_fp_96', 'srv242', 'srv78'],
         ['compute_fp_75', 'compute_fp_96', 'srv509', 'srv717'],
         ['compute_fp_37', 'compute_fp_96', 'srv279', 'srv78'],
         ['compute_fp_37', 'compute_fp_4', 'compute_int_28', 'srv470'],
         ['621', 'srv602', 'srv69', 'srv717'],
         ['srv242', 'srv650', 'srv676', 'srv767'],
         ['482', 'compute_fp_12', 'compute_fp_59', 'srv602'],
         ['433', 'compute_fp_37', 'srv440', 'srv602'],
         ['403.gcc-16B', '649', 'compute_int_751', 'srv64'],
         ['482', 'srv242', 'srv509', 'srv637'],
         ['654.roms_s-1007B', 'compute_fp_105', 'compute_fp_4', 'srv279'],
         ['649', 'compute_fp_100', 'compute_int_28', 'srv58'],
         ['627', 'compute_fp_12', 'srv160', 'srv262'],
         ['compute_int_105srv366', 'srv470', 'srv776', 'srv78'],
         ['482', '654.roms_s-1007B', 'compute_fp_100', 'srv440'],
         ['compute_fp_124', 'compute_fp_57', 'srv461', 'srv470'],
         ['srv132', 'srv602', 'srv64', 'srv676'],
         ['627', 'compute_fp_12', 'compute_int_751', 'srv58'],
         ['621', '649', 'compute_fp_111', 'srv394'],
         ['compute_fp_111', 'compute_fp_12', 'compute_fp_124', 'srv676'],
         ['627', 'compute_fp_12', 'srv160', 'srv717']
      ]
   
   return requested_apps



