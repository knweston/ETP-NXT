import os
import sys
import itertools
import argparse
from autoslurmutils import parse_apps

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
    requested_apps = parse_apps(apps)

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
        cmd += f"{code_dir+'/run_champsim.sh'} {code_dir+'/bin/'+executable} 0 1200 {'$TMPDIR/'+app+'.champsimtrace.xz PROFILE'}\n"
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