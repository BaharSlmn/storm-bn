import sys
import os
import pandas as pd

def parse_boolean(value):
    value = value.lower()
    if value in ["true", "yes", "y", "1", "t"]:
        return True
    return False

def run_disjunction_experiments(flag):
    networks = ['hailfinder' ,'mildew', 'win95pts']
    no_hypothesis = [2,4,8,16]
    data = pd.DataFrame(columns=['network','h_size','inference_time(s)', 'scatterclass'])

    for i in range(len(networks)):
            for j in no_hypothesis:
                #inference times
                time_a, time_s = 0, 0
                # inference using Ace
                if os.path.exists(f'ace/{networks[i]}/{networks[i]}_disjunction_{j}.net'):
                    if flag:
                        os.system(f'../ace_v3/compile ace/{networks[i]}/{networks[i]}_disjunction_{j}.net > ace/output/{networks[i]}_disjunction_{j}_result.txt')
                        os.system(f'../ace_v3/evaluate ace/{networks[i]}/{networks[i]}_disjunction_{j}.net > ace/output/{networks[i]}_disjunction_{j}_result.txt' )   
                    f1 = open(f'ace/output/{networks[i]}_disjunction_{j}_result.txt', 'r')
                    ace_content = f1.read()
                    begin_a = ace_content.find('Total Inference Time (ms) : ') + 28 
                    end_a = ace_content.find('\n', begin_a)
                    time_a = float(ace_content[begin_a:end_a].strip()) / 1000     
                else:
                    time_a = 1000
                df1 = pd.DataFrame([[networks[i], j, time_a, f'{networks[i][:4]}-ace']],columns=['network','h_size','inference_time(s)', 'scatterclass'])    
                data = pd.concat([data,df1])                               
                #Inference using Storm
                f = open(f'storm/{networks[i]}/{networks[i]}_disjunction_{j}_storm_command.query', 'r')
                command = f.read()
                if flag:
                    os.system(f'{command}')      
                f2 = open(f'storm/output/{networks[i]}_disjunction_{j}_result.txt', 'r')
                storm_content = f2.read()
                key = 'Time for model checking:'
                begin_s = storm_content.find(key) + len(key)+1
                end_s = storm_content.find('\n', begin_s)
                time_s = storm_content[begin_s:end_s].strip()
                time_s = time_s[:-2]
                df2 = pd.DataFrame([[networks[i], j, time_s, f'{networks[i][:4]}-storm']],columns=['network','h_size','inference_time(s)', 'scatterclass'])    
                data = pd.concat([data,df2])   
    data.to_csv('multiple-disjunction-ace-storm.csv', index=False)


if __name__ == "__main__":
        flag = False
        if len(sys.argv) > 1:
            flag = parse_boolean(sys.argv[1])
        run_disjunction_experiments(flag)
        os.system('pdflatex disjunction_plot.tex')
        os.system('rm disjunction_plot.aux')
        os.system('rm disjunction_plot.log')
