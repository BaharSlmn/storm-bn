import sys
import os

def parse_boolean(value):
    value = value.lower()

    if value in ["true", "yes", "y", "1", "t"]:
        return True
        
    return False

def create_2D_graph(flag):
    if flag:
        f = open(f'alarm-red-green-plots/2D/alarm_pla_graph.query', 'r')
        command = f.read()
        #run storm command and save the output in alarm_pla_graph.txt
        os.system(f'{command}')   
        #run script to generate the alarm_pla_graph.tex file 
        os.system(f'python3 alarm-red-green-plots/2D/pl.py --file  alarm-red-green-plots/2D/alarm_pla_graph.txt > alarm-red-green-plots/2D/alarm_pla_graph_2D.tex')

if __name__ == "__main__":

    #flag variable that indicates whether the user wants to rerun the experiments: False is the default value
    flag = False
    if len(sys.argv) > 1:
        flag = parse_boolean(sys.argv[1])
        
    create_2D_graph(flag)

