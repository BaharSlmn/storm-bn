import sys
import os
from time import time
import pandas as pd
import re

def parse_boolean(value):
    value = value.lower()
    if value in ["true", "yes", "y", "1", "t"]:
        return True
    return False

def run_psdd_storm_experiments(flag):
    networks = ['andes','win95pts']
    data = pd.DataFrame(columns=['network_method','construction_time_s','inference_time_s'])
    for network in networks:
        time_p, time_s = 0, 0
        if flag:
            #compute construction time using psdd_nips
            start_nips = time()
            #os.system(f'psdd_nips/build/psdd --uai_file=psdd_nips/bndir/{network}.uai -m1 --psdd_output=psdd_nips/output/{network}.psdd  --vtree_output=psdd_nips/output/{network}.vtree')
            end_nips = time()
            time_nips = f'{end_nips - start_nips:4f}'
            psdd_nips_str = f'{network}, time: {time_nips}s.\n'
            f = open(f'psdd_nips/{network}_results.csv', 'w')
            f.write(psdd_nips_str)
            f.close()
        f_nips = open(f'psdd_nips/{network}_results.csv', 'r')
        results_nips = f_nips.read()
        begin_nips = results_nips.find('time: ') + 6
        end_nips = results_nips.find('s.', begin_nips)
        time_nips = results_nips[begin_nips:end_nips].strip()


        #run psdd and measure time
        start_p = time()
        os.system(f'psdd/build/psdd_inference --mar_query psdd/example/{network}.psdd psdd/example/{network}.vtree > psdd/output/{network}_result.txt')
        end_p = time()
        time_p = end_p - start_p
        network_method_psdd = f'{network} - PSDD'
        df = pd.DataFrame([[network_method_psdd, time_nips, time_p]], columns=['network_method','construction_time_s','inference_time_s'])
        data = pd.concat([data, df])

        if flag:
            #run storm --bisimulation queries
            command_bisim = f'storm --jani storm/{network}/{network}.jani --expvisittimes --build-all-labels --bisimulation > storm/output/{network}_bisim_result.txt'
            os.system(f'{command_bisim}')  
        
        #parse results for storm --bisimulation
        f_bisim_res = open(f'storm/output/{network}_bisim_result.txt', 'r')
        results_bisim = f_bisim_res.read()

        begin_time_bisim = results_bisim.find('Time for model checking: ') + 25
        end_time_bisim = results_bisim.find('s.', begin_time_bisim)
        time_bisim = results_bisim[begin_time_bisim : end_time_bisim].strip()

        time_constr_bisim = 0
        begin_constr_bisim = results_bisim.find('Time for model construction: ') + 29
        end_constr_bisim = results_bisim.find('s.', begin_constr_bisim)
        time_constr_bisim = results_bisim[begin_constr_bisim:end_constr_bisim].strip()

        #add row to dataframe
        network_method_bisim = f'{network} - Storm, bisimulation'
        df = pd.DataFrame([[network_method_bisim, time_constr_bisim, time_bisim]], columns=['network_method','construction_time_s','inference_time_s'])
        data = pd.concat([data, df])

        #run storm --engine dd queries
        f_dd = open(f'storm/{network}/{network}_dd.query', 'r')
        command_dd = f_dd.read()
        #compute number of queries - needed to compute the average time
        start_pctl = command_dd.find('--prop "') + 7
        end_pctl = command_dd.find(';" --eninge dd', start_pctl)
        pctl_formulas = command_dd[start_pctl:end_pctl].strip()
        formulas = pctl_formulas.split(';')
        if flag:
            #Inference using Storm
            os.system(f'{command_dd}') 
        
        #parse results for storm --engine dd
        f_dd_res = open(f'storm/output/{network}_dd_result.txt', 'r')
        results_dd = f_dd_res.read()
        time_overall_dd = 0
        time_avg_dd = 0
        for t in re.finditer('Time for model checking: ', results_dd):
            end_s = results_dd.find('s.', t.end())
            time_s = results_dd[t.end():end_s].strip()
            time_overall_dd += float(time_s)
        time_avg_dd = time_overall_dd / len(formulas)
        #construction time
        time_constr_dd = 0
        if 'Time for model construction' in results_dd:
            begin_constr_dd = results_dd.find('Time for model construction: ') + 29
            end_constr_dd = results_dd.find('s.', begin_constr_dd)
            time_constr_dd = results_dd[begin_constr_dd:end_constr_dd].strip()
        #add row to dataframe
        network_method_dd = f'{network} - Storm, MTBDD'
        df = pd.DataFrame([[network_method_dd, time_constr_dd, time_avg_dd]], columns=['network_method','construction_time_s','inference_time_s'])
        data = pd.concat([data, df])

    print(data)


if __name__ == "__main__":
        flag = False
        if len(sys.argv) > 1:
            flag = parse_boolean(sys.argv[1])
        run_psdd_storm_experiments(flag)
