import os
import sys

def parse_boolean(value):
    value = value.lower()
    if value in ["true", "yes", "y", "1", "t"]:
        return True
    return False
    
def parse_pla_results(network, info_key):
    #contains start and end exps for each category
    info = ""
    is_timed_out = False
    f = open(f'output/{network}_storm_result.csv', 'r')
    content = f.read()

    dict = {
        'Fraction of satisfied area:' : '%',
        'Fraction of unsatisfied area:' : '%',
        'Unknown fraction:' : '%',
        'Total number of regions:' : '\n',
        'Time for model checking:' : '\n',
        'peak memory usage:' : 'MB'
    }

    if info_key not in content:
       return "-2.0"

    if "Received signal 15" in content:
        is_timed_out = True
        
    begin_c = content.find(info_key) + len(info_key) + 1
    end_c = content.find(dict[info_key], begin_c)
    line = content[begin_c:end_c].strip()
    if not is_timed_out:
        return line
    else:
        return "-2.0"

def make_pla_command(network, below_or_above, threshold, pctl_formula, refinement_factor, timeout, number_of_params): 
    command = ""
    parameters_intervals = ""

    for i in range(number_of_params):
        parameters_intervals += f'\"0<=p{i}<=1\"'
        if i < number_of_params-1:
            parameters_intervals += ","
    command = f'timeout {timeout}m storm-pars --jani jani_files/{network}.jani --prop \"P{below_or_above}{threshold}{pctl_formula}\" --region {parameters_intervals} --timemem --refine {refinement_factor} --bisimulation'
    print(command)
    return command

def run_pla_commands(step, start, limit, network, below_or_above, threshold, pctl_formula, timeout, number_of_params):
    output,sat,unsat,unknown, number_of_regions, time, memory = "", "", "", "", "","",""
    current_refinement_factor = start
    while current_refinement_factor >= limit and current_refinement_factor > 0:
        output_line = ""
        command = make_pla_command(network, below_or_above, threshold, pctl_formula, current_refinement_factor, timeout, number_of_params)
        os.system(f'{command} > output/{network}_storm_result.csv' )        
        sat = parse_pla_results(network, 'Fraction of satisfied area:')
        unsat = parse_pla_results(network, 'Fraction of unsatisfied area:')
        unknown = parse_pla_results(network, 'Unknown fraction:')
        number_of_regions = parse_pla_results(network, 'Total number of regions:')
        time = parse_pla_results(network, 'Time for model checking:')
        time = time[:-2]
        memory = parse_pla_results(network, 'peak memory usage:')
        coverage = round((1 - current_refinement_factor), 3)
        output_line = f'{current_refinement_factor}, {coverage}, {sat}, {unsat}, {unknown}, {number_of_regions}, {time}, {memory}\n'
        #check if process was killed
        if "-2.0" in output_line:
            break
        else:
            output += output_line 
            current_refinement_factor = round((current_refinement_factor - step), 3)
    return output


def run_pla_on_all_networks(flag):
    networks = ['win95pts_PLA_p8c1' ,'hepar2_PLA_p8c1', 'hailfinder_PLA_8']
    thresholds = [0.3, 0.3, 0.3]
    below_or_above = ['<=','<=', '<=']
    if flag:
        for i in range(len(networks)):
            pctl_file = open(f"pctl_files/{networks[i]}.pctl", "r")
            pctl_formula = pctl_file.read()
            output = run_pla_commands(0.01, 1, 0.01, networks[i], below_or_above[i], thresholds[i], pctl_formula, 30, 8)
            os.system(f'rm output/{networks[i]}_storm_result.csv')
            f = open(f'output/{networks[i]}.csv', 'w')
            f.write(f'refinementfactor,covered,sat,unsat,unknownpercent,regionnum,time,mem\n')
            f.write(output)

if __name__ == "__main__":
    #flag variable that indicates whether the user wants to rerun the experiments: False is the default value
    flag = False
    if len(sys.argv) > 1:
        flag = parse_boolean(sys.argv[1])
    run_pla_on_all_networks(flag)
