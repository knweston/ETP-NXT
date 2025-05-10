import os
import sys
import itertools
import argparse
from autoslurmutils import parse_apps

def create_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)

class SlurmScript:

    def __init__(self, cluster, job_name, job_dir, out_dir, cmd, time="5:00:00", mem="1G", num_threads="1"):
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
    requested_apps = parse_apps(apps)
    
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

    deadblock_tracepath = "/scratch/user/kevin.weston/llc-traces/dead-blks"

    job_list = []
    # construct commands
    for idx, app in enumerate(requested_apps):
        if (os.path.isfile(deadblock_tracepath+'/'+app+"-DEAD-BLKS.txt")):
            cmd = f"cp {deadblock_tracepath+'/'+app+'-DEAD-BLKS.txt'} $TMPDIR\n"
        else:
            cmd = ""
        
        if trace_folder == "goog-traces":
            cmd += f"cp {trace_dir+'/'+app+'.champsimtrace.gz'} $TMPDIR\n"
            # cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.gz'}"
        elif trace_folder == "cste-traces":
            cmd += f"cp {trace_dir+'/'+app+'.trace.xz'} $TMPDIR\n"
            # cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.trace.xz'}"
        else:
            cmd += f"cp {trace_dir+'/'+app+'.champsimtrace.xz'} $TMPDIR\n"
            # cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.xz'}"

        cmd += f"\nfor i in {'{0..'+str(num_loops-1)+'}'}\n"
        cmd += f"do\n"
        
        if trace_folder == "goog-traces":
            # cmd += f"cp {trace_dir+'/'+app+'.champsimtrace.gz'} $TMPDIR\n"
            cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.gz'}"
        elif trace_folder == "cste-traces":
            # cmd += f"cp {trace_dir+'/'+app+'.trace.xz'} $TMPDIR\n"
            cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.trace.xz'}"
        else:
            # cmd += f"cp {trace_dir+'/'+app+'.champsimtrace.xz'} $TMPDIR\n"
            cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} {num_warmup} {num_simulate} {config_dir + '/' + config_file} {'$TMPDIR/'+app+'.champsimtrace.xz'}"
        
        cmd += ' "$i"'

        if result_output == "":
            cmd += f" {executable}" # default result folder name = exec file name
        else:
            cmd += f" {result_output}"

        if trace_folder == "cste-traces":
            cmd += f" -cloudsuite"

        cmd += f"\n"
        cmd += f"done\n"

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
    parser.add_argument("-w", "--num_warmup", required=True, type=int)
    parser.add_argument("-s", "--num_simulate", required=False, default=1000, type=int)
    parser.add_argument("-t", "--trace_folder", required=False, default="dpc3-traces", type=str)
    parser.add_argument("-r", "--result_folder", required=False, default="", type=str)
    args = parser.parse_args()
    job_list = generate_jobs(args.applications, args.job_output_folder, args.result_folder, args.cluster_name, args.exec_file, args.num_loops, args.config, args.num_warmup, args.num_simulate, args.trace_folder)
    for job in job_list:
        job.submit()


if __name__ == '__main__':
    main()