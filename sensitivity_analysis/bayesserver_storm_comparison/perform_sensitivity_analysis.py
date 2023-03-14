import os
import pandas as pd
import sys


networks = ['alarm', 'asia', 'cancer', 'earthquake', 'hailfinder', 'hepar2', 'insurance', 'sachs', 'water', 'win95pts']

networks_p =  [('alarm',85), ('child', 60), ('hailfinder', 380), ('hepar2', 135), ('insurance', 140), ('water',255), ('win95pts', 200)]

def parse_boolean(value):
    value = value.lower()

    if value in ["true", "yes", "y", "1", "t"]:
        return True
        
    return False

if __name__ == "__main__":

    #flag variable that indicates whether the user wants to rerun the experiments: False is the default value
    flag = False
    if len(sys.argv) > 1:
        flag = parse_boolean(sys.argv[1])

    data = pd.DataFrame(columns=['network','scatterclass','storm_bisim', 'bayesserver'])

    for network in networks:   
        for j in range(1,3): 
            #run experiments using Bayesserver
            if flag:
                os.system(f"python3 bayesserver/network_files/p{j}/{network}_sensitivity.py > bayesserver/output_files/p{j}/{network}_result.txt")

            #run experiments using Storm
            if flag:
                prop_file = f"storm/prop_files/p{j}/{network}.prop"
                os.system(f"bash {prop_file} > storm/output_files/p{j}/{network}_result.txt")

            #parse output file for Bayesserver
            f1 = open(f'bayesserver/output_files/p{j}/{network}_result.txt','r')
            content_b = f1.read()
            begin_time_b = content_b.find("Time for Sensitivity Analysis:") + 31
            end_time_b = content_b.find("\n",begin_time_b)
            time_b = content_b[begin_time_b:end_time_b].strip()
            time_b = time_b[:-1]

            #parse output file for Storm
            f2 = open(f'storm/output_files/p{j}/{network}_result.txt','r')
            content_s = f2.read()
            begin_time_s = content_s.find("Time for model checking") +25
            end_time_s = content_s.find("\n",begin_time_s)
            time_s = content_s[begin_time_s:end_time_s].strip()
            time_s = time_s[:-2]
            begin_p = content_s.find("_") +1
            end_p = content_s.find("\n",begin_p)
            params = content_s[begin_p:end_p]
            scatterclass = f'qual{j}'

            #need to add some noise if value is zero
            if float(time_b) < 0.001:
                time_b = 0.0001  

            if float(time_s) < 0.001:
                time_s = 0.0001
            df1 = pd.DataFrame([[network, scatterclass, time_s, time_b]],columns=['network','scatterclass','storm_bisim', 'bayesserver'])            
            data = pd.concat([data,df1])
    
    #run storm for pn files
    for network,params in networks_p:
            prop_file = f"storm/prop_files/pn/{network}_{params}.prop"
            if flag:
                os.system(f"bash {prop_file} > storm/output_files/pn/{network}{params}_result.txt")
            #parse the results    
            f_p = open(f'storm/output_files/pn/{network}{params}_result.txt','r')
            content_p = f_p.read()
            begin_time_p = content_p.find("Time for model checking: ") +25
            end_time_p = content_p.find("\n",begin_time_p)
            time_p = content_p[begin_time_p:end_time_p].strip()
            time_p = time_p[:-2]
            #find scatterclass
            scatterclass = 1
            if params < 50:
                scatterclass = 10
            elif params < 100:
                scatterclass = 50
            elif params < 200:
                scatterclass = 100
            elif params < 300:
                scatterclass = 200
            else:
                scatterclass = 300
            df2 = pd.DataFrame([[network, f'qual{scatterclass}' , time_p, 1200]],columns=['network','scatterclass','storm_bisim', 'bayesserver'])            
            data = pd.concat([data,df2])

    data.to_csv('storm_bayesserver_comparison.csv')

    os.system('pdflatex plot.tex')
    os.system('rm plot.aux')
    os.system('rm plot.log')
    os.system('rm storm_bayesserver_comparison.csv')