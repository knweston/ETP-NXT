import os
import sys
import itertools
import argparse

def create_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

class SlurmScript:

    def __init__(self, job_name, job_dir, out_dir, cmd, time="12:00:00", mem="24G", num_threads="1"):
        self.job_name = job_name
        self.job_dir  = job_dir
        self.out_dir  = out_dir
        self.cmd      = cmd
        self.time     = time
        self.mem      = mem
        self.ntasks   = num_threads

    def get_header(self):
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



def generate_jobs(apps, output, executable, num_loops, trace_folder):
    # parse requested apps
    requested_apps = apps.split("_")
    
    if apps == "setptest":
        requested_apps = ["403", "410", "437", "450", "470", "471", "473", "482", "605", "607", "620", "623", "627", "628", "649", \
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
    
    elif apps == "nxetp":
        # MPKI > 1 under config 64/8-2,256/8-8,2048/16-20, rank1, DRAM=24
        requested_apps = ["410", "433", "450", "459", "470", "471", "473", "482", \
                          "605", "607", "620", "621", "623", "627", "628", "649", \
                          "bc.urand-3004B","bc.web-1627B", \
                          "bfs.kron-128B","bfs.road-19B","bfs.web-56B", \
                          "cc.road-36B","cc.urand-353B","cc.web-24B", \
                          "pr.web-3076B", "tc.twitter-4460B",\
                          "bodytrack", "canneal", "freqmine", "facesim", "fluidanimate", "streamcluster"
        ]

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
    elif apps == "nxetp-bonus":
        requested_apps = [
            "429","436","437","462","481","619","654","654simopt","657"
        ]

    elif apps == "cvp1":
        requested_apps = [
            "compute_int_96",\
            "srv10"
        ]
    
    elif apps == "gapmore":
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
            "pr.kron-90B","pr.twitter-168B",\
            "sssp.twitter-1116B","sssp.twitter-900B",\
            "tc.twitter-2097B","tc.twitter-34B",\
            "tc.urand-825B","tc.web-5088B","tc.web-2259B","tc.web-3876B"
        ]

    # get user's home directory
    home_dir   = os.path.dirname(os.getcwd())

    # parse code/trace/output/jobfile/config directories
    code_dir   = os.path.join(os.getcwd(), 'profile-sim')
    trace_dir  = os.path.join(home_dir, trace_folder)  # champsim-traces directory
    job_dir    = os.path.join(home_dir, 'data/slurm-jobs')
    output_dir = os.path.join(os.path.join(home_dir, 'data/profile'), output)
    config_dir = os.path.join(code_dir, 'genetic_idx_cfgs')

    # create jobfile/output directories if not exist
    create_dir(job_dir)
    create_dir(output_dir)

    job_list = []
    # construct commands
    for idx, app in enumerate(requested_apps):
        cmd = f"cp {trace_dir+'/'+app+'.champsimtrace.xz'} $TMPDIR\n"
        cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} 0 1500 {'$TMPDIR/'+app+'.champsimtrace.xz PROFILE'}\n"
        cmd += f"cp {'$TMPDIR/*-TRACE.txt'} {trace_dir+'/'}\n"

        new_script = SlurmScript(app+"_"+executable, job_dir, output_dir, cmd)
        new_script.create()
        job_list.append(new_script)
    
    return job_list
    

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-a", "--applications", required=True, type=str)
    parser.add_argument("-o", "--output_folder", required=True, type=str)
    parser.add_argument("-e", "--exec_file", required=True, type=str)
    parser.add_argument("-l", "--num_loops", required=False, default=1, type=int)
    parser.add_argument("-t", "--trace_folder", required=False, default="dpc3-traces", type=str)
    args = parser.parse_args()
    job_list = generate_jobs(args.applications, args.output_folder, args.exec_file, args.num_loops, args.trace_folder)
    for job in job_list:
        job.submit()


if __name__ == '__main__':
    main()