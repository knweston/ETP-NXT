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
    
    if apps == "setptest":
        requested_apps = ["403", "410", "437", "450", "470", "471", "473", "482", \
                          "605", "607", "620", "623", "627", "628", "649", \
                          "bc.urand-3004B","bc.web-1627B", \
                          "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
                          "cc.road-36B","cc.urand-353B","cc.web-24B", \
                          "pr.web-3076B", "tc.twitter-4460B",\
                          "bodytrack", "canneal", "vips", "facesim", "streamcluster"]
    
    elif apps == "setptrain":
        requested_apps = ["401","429","436","445","456","459","602","621","657", \
                          "bc.kron-2608B","bc.road-131B","pr.road-600B","sssp.road-66B", \
                          "sssp.web-1185B","tc.kron-4490B","tc.road-1B", \
                          "fluidanimate","freqmine", \
                          "xs.XL100hash-194B","xs.XL100nuclide-577B","xs.XL100unionized-28B","xs.XL64hash-126B"]

    elif apps == "setpother":
        requested_apps = ["bc.twitter-876B", "cc.twitter-85B", \
                          "xs.XL64unionized-28B","xs.XXL100unionized-24B"]
    
    elif apps == "setpfocus":
        requested_apps = ["403","410","437","450","470","471","473","482","605","607","620","623","627","628","649", \
                          "401","436","445","456","459","602","621","657", \
                          "fluidanimate","freqmine", \
                          "bodytrack","canneal","vips","facesim","streamcluster"]
    
    elif apps == "setpcore":
        requested_apps = ["403","473","482","605","bodytrack","canneal","streamcluster"]

    elif apps == "google":
        requested_apps = ["charlie_combined_1", "charlie_combined_2", "charlie_combined_3", 
                          "charlie_combined_4", "charlie_combined_5", "charlie_combined_6"]
    
    elif apps == "xidxtest":
        requested_apps = ["403", "436", "437", "459", "471", "473", "482", \
                          "602", "607", "620", "621", "627", "628", "649", \
                          "bc.road-131B", "bc.urand-3004B", "bc.web-1627B", \
                          "bfs.kron-128B", "bfs.web-56B", \
                          "cc.road-36B", "cc.twitter-85B", "cc.web-24B", \
                          "pr.road-600B", "pr.web-3076B", \
                          "bodytrack", "canneal", "facesim", "fluidanimate", "freqmine", "streamcluster"]
    
    elif apps == "xidxother":
        requested_apps = ["401","410","429","445","450","456","470","605","623","657", \
                          "bc.kron-2608B","bc.twitter-876B","bfs.road-19B",\
                          "cc.urand-353B","sssp.road-66B","sssp.web-1185B",\
                          "tc.kron-4490B","tc.road-1B","tc.twitter-4460B",\
                          "xs.XL100hash-194B","xs.XL100nuclide-577B","xs.XL100unionized-28B","xs.XL64hash-126B","xs.XL64unionized-28B","xs.XXL100unionized-24B",\
                          "vips"]
    
    elif apps == "betprev":
        requested_apps = ["400","401","445","456","464","600","641","657",\
                          "http","tomcat","blackscholes"]
    
    elif apps == "specmore":
        requested_apps = ["400.perlbench-50B",\
                        "401.bzip2-226B","401.bzip2-277B","401.bzip2-38B","401.bzip2-7B",\
                        "434.zeusmp-10B",\
                        "445.gobmk-17B","445.gobmk-2B","445.gobmk-30B","445.gobmk-36B",\
                        "456.hmmer-191B","456.hmmer-327B","456.hmmer-88B",\
                        "458.sjeng-1088B","458.sjeng-283B","458.sjeng-31B","458.sjeng-767B",\
                        "464.h264ref-57B","464.h264ref-64B","464.h264ref-97B",\
                        "603.bwaves_s-1740B","603.bwaves_s-2609B","603.bwaves_s-2931B","603.bwaves_s-891B",\
                        "625.x264_s-12B","625.x264_s-33B","625.x264_s-39B",\
                        "641.leela_s-1052B","641.leela_s-1083B","641.leela_s-149B","641.leela_s-334B","641.leela_s-602B","641.leela_s-800B","641.leela_s-862B",\
                        "644.nab_s-12459B",\
                        "648.exchange2_s-387B",\
                        "654.roms_s-1007B","654.roms_s-1070B","654.roms_s-1390B","654.roms_s-1613B","654.roms_s-293B","654.roms_s-523B"]

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
    
    elif apps == "cvp-betp":
        requested_apps = [
            "compute_fp_105",\
            "compute_fp_108",\
            "compute_fp_111",\
            "compute_fp_12",\
            "compute_fp_124",\
            "compute_fp_129",\
            "compute_fp_130",\
            "compute_fp_134",\
            "compute_fp_136",\
            "compute_fp_139",\
            "compute_fp_37",\
            "compute_fp_44",\
            "compute_fp_6",\
            "compute_fp_60",\
            "compute_fp_84",\
            "compute_fp_9",\
            "compute_fp_94",\
            "compute_fp_96",\
            "compute_int_0",\
            "compute_int_105",\
            "compute_int_141",\
            "compute_int_149",\
            "compute_int_150",\
            "compute_int_161",\
            "compute_int_174",\
            "compute_int_177",\
            "compute_int_20",\
            "compute_int_204",\
            "compute_int_248",\
            "compute_int_252",\
            "compute_int_268",\
            "compute_int_28",\
            "compute_int_297",\
            "compute_int_301",\
            "compute_int_354",\
            "compute_int_369",\
            "compute_int_370",\
            "compute_int_4",\
            "compute_int_41",\
            "compute_int_415",\
            "compute_int_424",\
            "compute_int_425",\
            "compute_int_434",\
            "compute_int_467",\
            "compute_int_479",\
            "compute_int_507",\
            "compute_int_525",\
            "compute_int_529",\
            "compute_int_536",\
            "compute_int_563",\
            "compute_int_58",\
            "compute_int_584",\
            "compute_int_610",\
            "compute_int_625",\
            "compute_int_637",\
            "compute_int_661",\
            "compute_int_726",\
            "compute_int_739",\
            "compute_int_788",\
            "compute_int_817",\
            "compute_int_829",\
            "compute_int_844",\
            "compute_int_856",\
            "compute_int_865",\
            "compute_int_875",\
            "compute_int_921",\
            "compute_int_925",\
            "compute_int_942",\
            "srv10",\
            "srv12",\
            "srv132",\
            "srv137",\
            "srv155",\
            "srv160",\
            "srv172",\
            "srv174",\
            "srv207",\
            "srv215",\
            "srv222",\
            "srv225",\
            "srv255",\
            "srv259",\
            "srv262",\
            "srv276",\
            "srv279",\
            "srv314",\
            "srv32",\
            "srv366",\
            "srv371",\
            "srv405",\
            "srv41",\
            "srv442",\
            "srv453",\
            "srv456",\
            "srv461",\
            "srv470",\
            "srv475",\
            "srv48",\
            "srv495",\
            "srv504",\
            "srv509",\
            "srv527",\
            "srv537",\
            "srv564",\
            "srv617",\
            "srv641",\
            "srv676",\
            "srv702",\
            "srv715",\
            "srv72",\
            "srv738",\
            "srv757",\
            "srv771",\
            "srv776",\
            "srv85",\
            "srv93"
        ]
    
    elif apps == "betpset":
        requested_apps = ["403", "410", "433", "450", "459", "470", "471", "473", "482", \
                          "605", "607", "620", "621", "623", "627", "628", "649", \
                          "bc.urand-3004B","bc.web-1627B", \
                          "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
                          "cc.road-36B","cc.urand-353B","cc.web-24B", \
                          "pr.web-3076B", "tc.twitter-4460B",\
                          "bodytrack", "canneal", "freqmine", "facesim", "fluidanimate", "streamcluster"]
    
    elif apps == "betpset-adj":
        requested_apps = ["403", "410", "433", "450", "459", "471", "473", "482", \
                          "607", "620", "621", "623", "627", "628", \
                          "bc.urand-3004B", \
                          "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
                          "cc.road-36B","cc.urand-353B",\
                          "pr.web-3076B", "tc.twitter-4460B",\
                          "canneal", "freqmine", "facesim", "fluidanimate"]
                          
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