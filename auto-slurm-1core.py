import os
import sys
import itertools
import argparse

def create_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

class SlurmScript:

    def __init__(self, cluster, job_name, job_dir, out_dir, cmd, time="5:00:00", mem="4G", num_threads="1"):
        self.cluster  = cluster
        self.job_name = job_name
        self.job_dir  = job_dir
        self.out_dir  = out_dir
        self.cmd      = cmd
        self.time     = time
        self.mem      = mem
        self.ntasks   = num_threads

    def get_header(self):
        if self.cluster == "atlas":
            header = (
                f'#!/bin/bash\n'
                f'#SBATCH --job-name={self.job_name}\n'
                f'#SBATCH --output={self.out_dir}/{self.job_name}-%j.txt\n'
                f'#SBATCH --ntasks-per-node={self.ntasks}\n'
                f'#SBATCH --mem={self.mem}\n'
                f'\n'
            )
            return header
        elif self.cluster == "grace" or self.cluster == "terra":
            header = (
                f'#!/bin/bash\n'
                f'#SBATCH --job-name={self.job_name}\n'
                f'#SBATCH --output={self.out_dir}/{self.job_name}-%j.txt\n'
                f'#SBATCH --time={self.time}\n'
                f'#SBATCH --nodes=1\n'
                f'#SBATCH --ntasks-per-node={self.ntasks}\n'
                f'#SBATCH --mem={self.mem}\n'
                f'\n'
            )
            return header
    
    def create(self):
        script_path = self.job_dir+'/'+self.job_name+'.sh'
        with open(script_path, 'w') as f:
            f.write(self.get_header())
            f.write(self.cmd)
            print(f'Sbatch script is created: {script_path}', file=sys.stderr)


    def submit(self, prod=True):
        script_path = self.job_dir+'/'+self.job_name+'.sh'
        cmd = f'sbatch {script_path}'
        print(cmd)
        if prod:
            os.system(cmd)



def generate_jobs(apps, job_output, result_output, cluster, executable, num_loops, config_file, num_warmup, num_simulate, trace_folder):
    # parse requested apps
    requested_apps = apps.split("_")
    
    elif apps == "xsbench":
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
                        "xs.XXL100hash-552B",\
                        "xs.XXL100hash-563B",\
                        "xs.XXL100hash-94B",\
                        "xs.XXL100nuclide-165B",\
                        "xs.XXL100nuclide-30B",\
                        "xs.XXL100nuclide-324B",\
                        "xs.XXL100nuclide-487B",\
                        "xs.XXL100nuclide-648B",\
                        "xs.XXL100nuclide-650B",\
                        "xs.XXL100nuclide-861B",\
                        "xs.XXL100unionized-122B",\
                        "xs.XXL100unionized-155B",\
                        "xs.XXL100unionized-188B",\
                        "xs.XXL100unionized-24B",\
                        "xs.XXL64hash-205B",\
                        "xs.XXL64hash-24B",\
                        "xs.XXL64nuclide-24B",\
                        "xs.XXL64nuclide-531B",\
                        "xs.XXL64nuclide-566B",\
                        "xs.XXL64unionized-122B",\
                        "xs.XXL64unionized-155B",\
                        "xs.XXL64unionized-188B",\
                        "xs.XXL64unionized-24B"]

    elif apps == "nxetp":
        # MPKI > 1 under config 64/8-2,256/8-8,2048/16-20, rank1, DRAM=24
        requested_apps = ["410", "433", "450", "459", "470", "471", "473", "482", \
                          "605", "607", "620", "621", "623", "627", "628", "649", \
                          "bc.urand-3004B","bc.web-1627B", \
                          "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
                          "cc.road-36B","cc.urand-353B","cc.web-24B", \
                          "pr.web-3076B", "tc.twitter-4460B",\
                          "bodytrack", "canneal", "freqmine", "facesim", "fluidanimate", "streamcluster"]

    elif apps == "nxetp-specmore":
        # MPKI > 1 under config 64/8-2,256/8-8,2048/16-20, rank1, DRAM=24
        requested_apps = [
            "401.bzip2-226B","401.bzip2-277B",\
            "434.zeusmp-10B",\
            "456.hmmer-327B",\
            "603.bwaves_s-1740B","603.bwaves_s-2609B","603.bwaves_s-2931B","603.bwaves_s-891B",\
            "625.x264_s-39B",\
            "654.roms_s-1007B","654.roms_s-1070B","654.roms_s-1390B","654.roms_s-1613B","654.roms_s-293B","654.roms_s-523B"
        ]

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
            "compute_int_96"
            "srv10",\
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
            "srv172",\
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
            "srv93"
        ]
    
    # Note: ferret is ok at 1B, but consistently fails at 10B run (at instruction 1750000001) 
    # due to "src/ooo_cpu.cc:1653: void O3_CPU::execute_store(uint32_t, uint32_t, uint32_t): Assertion `0' failed."
    # this bug is seen in all indexing types, including nohash

    # get user's home directory
    home_dir   = os.path.dirname(os.getcwd())

    # parse code/trace/job_output/jobfile/config directories
    code_dir   = os.getcwd()
    trace_dir  = os.path.join(home_dir, trace_folder)  # champsim-traces directory
    job_dir    = os.path.join(home_dir, 'data/slurm-jobs')
    job_output_dir = os.path.join(os.path.join(home_dir, 'data/result'), job_output)
    config_dir = os.path.join(code_dir, 'genetic_idx_cfgs')

    # create jobfile/output directories if not exist
    create_dir(job_dir)
    create_dir(job_output_dir)

    job_list = []
    # construct commands
    for idx, app in enumerate(requested_apps):
        
        if trace_folder == "goog-traces":
            cmd = f"cp {trace_dir+'/'+app+'.champsimtrace.gz'} $TMPDIR\n"
            cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.gz'}"
        else:
            cmd = f"cp {trace_dir+'/'+app+'.champsimtrace.xz'} $TMPDIR\n"
            cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.xz'}"
        
        if result_output == "":
            cmd += f" {executable}" # default result folder name = exec file name
        else:
            cmd += f" {result_output}"

        if apps == "cloud":
            cmd += f" -cloudsuite"
        cmd += f"\n"

        new_script = SlurmScript(cluster, app+"_"+executable, job_dir, job_output_dir, cmd)
        new_script.create()
        job_list.append(new_script)
    
    return job_list
    

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--applications", required=True, type=str)
    parser.add_argument("-o", "--job_output_folder", required=True, type=str)
    parser.add_argument("-e", "--exec_file", required=True, type=str)
    parser.add_argument("-c", "--cluster_name", required=False, default="grace", type=str)
    parser.add_argument("-f", "--config", required=False, default="none", type=str)
    parser.add_argument("-l", "--num_loops", required=False, default=1, type=int)
    parser.add_argument("-w", "--num_warmup", required=False, default=1, type=int)
    parser.add_argument("-s", "--num_simulate", required=False, default=1000, type=int)
    parser.add_argument("-t", "--trace_folder", required=False, default="dpc3-traces", type=str)
    parser.add_argument("-r", "--result_folder", required=False, default="", type=str)
    args = parser.parse_args()
    job_list = generate_jobs(args.applications, args.job_output_folder, args.result_folder, args.cluster_name, args.exec_file, args.num_loops, args.config, args.num_warmup, args.num_simulate, args.trace_folder)
    for job in job_list:
        job.submit()


if __name__ == '__main__':
    main()