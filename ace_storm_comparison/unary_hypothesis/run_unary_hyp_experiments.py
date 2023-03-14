import sys
import os
from time import time
import pandas as pd
from os.path import exists
import re

def parse_boolean(value):
    value = value.lower()
    if value in ["true", "yes", "y", "1", "t"]:
        return True
    return False

def run_unary_experiments(flag):
    networks = ['alarm', 'andes', 'asia', 'barley', 'cancer', 'child', 'earthquake', 'hailfinder', 'hepar2', 'insurance', 'mildew', 'pathfinder', 'sachs', 'survey', 'water', 'win95pts']
    data = pd.DataFrame(columns=['network','ace_inference_time','storm_nobisim_mar_time', 'scatterclass'])
    for network in networks:
        time_a, time_s = 0, 0
        if flag:
            os.system(f'../ace_v3/compile ace/net_files/{network}.net > ace/output/{network}_result.txt')
        try:
            if flag:
                os.system(f'../ace_v3/evaluate ace/net_files/{network}.net > ace/output/{network}_result.txt')
            f1 = open(f'ace/output/{network}_result.txt', 'r')
            ace_content = f1.read()
            begin_a = ace_content.find('Total Inference Time (ms) : ') + 28 
            if begin_a == -1:
                time_a = 100000
            else:    
                end_a = ace_content.find('\n', begin_a)
                time_a = float(ace_content[begin_a:end_a].strip())  
        except Exception:
            time_a = 300000

        #Inference using Storm
        try:
            if flag:
                os.system(f'storm --jani storm/jani_files/{network}.jani --expvisittimes --build-all-labels  > storm/output/{network}_result.txt')      
            f2 = open(f'storm/output/{network}_result.txt', 'r')
            storm_content = f2.read()
            if not re.search('Time for model checking: ', storm_content):
                #process is killed
                time_s = 100000
            else:
                begin_s = storm_content.find('Time for model checking: ') + 25
                end_s = storm_content.find('s.', begin_s)
                time_s = storm_content[begin_s:end_s].strip()
                time_s = float(time_s)*1000
                if float(time_s) <= 0.1:
                    time_s = 0.1
        except Exception:
            time_s = 300000

        df = pd.DataFrame([[network, time_a, time_s, 'qual3']],columns=['network','ace_inference_time','storm_nobisim_mar_time', 'scatterclass'])    
        data = pd.concat([data,df])  

        #update latex source
        f_l  = open(f'unary_hyp_plot.tex', 'r')
        latex_content = f_l.read()
        #check whether network name is shown in the plot
        if re.search(f'{network}', latex_content):
            key = f'coords={network}'
            #parse text to find old coordinates
            begin_l = latex_content.find(key)
            end_l = latex_content.find('\n', begin_l)
            line = latex_content[begin_l:end_l].strip()
            begin_l2 = line.find('coordinates {(')+14
            end_l2 = line.find(')', begin_l2)
            coords_old_str = line[begin_l2:end_l2].strip()
            coords_old = coords_old_str.split(',')
            x1_old = coords_old[0]
            x2_old = coords_old[1]
            line_new = line.replace(x1_old,str(time_s))
            line_new = line_new.replace(x2_old,str(time_a))
            print(line)
            print(line_new)
            latex_content = latex_content.replace(line, line_new)
            #replace coordinates
            with open('unary_hyp_plot.tex', 'w') as f:
               f.write(latex_content)

    data.to_csv('unary_hypothesis_ace_storm.csv', index=False)

if __name__ == "__main__":
        flag = False
        if len(sys.argv) > 1:
            flag = parse_boolean(sys.argv[1])
        run_unary_experiments(flag)
        os.system('pdflatex unary_hyp_plot.tex')
        os.system('rm unary_hyp_plot.aux')
        os.system('rm unary_hyp_plot.log')