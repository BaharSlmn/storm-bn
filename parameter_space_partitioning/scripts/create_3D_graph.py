import os
import sys

def parse_boolean(value):
    value = value.lower()

    if value in ["true", "yes", "y", "1", "t"]:
        return True
        
    return False

def create_3D_graph(flag):

    if flag:
        #run storm command    
        f1 = open(f'alarm-red-green-plots/3D/alarm_pla_graph.query', 'r')
        command = f1.read()
        os.system(f'{command} > alarm-red-green-plots/3D/alarm_solution_function.txt')        
    
    #parse the result from storm
    f2 = open(f'alarm-red-green-plots/3D/alarm_solution_function.txt', 'r')
    result = f2.read()
    begin_c = result.find('Result (initial states): ') + 25
    end_c = result.find('\n', begin_c)
    sol_funct_new = result[begin_c:end_c].strip()

    f3 = open(f'alarm-red-green-plots/3D/alarm_pla_graph_3D.tex', 'r')
    tex_file = f3.read()
    begin_s = tex_file.find('meta={z+0.6}] {') + 15
    end_s = tex_file.find(';', begin_s)
    sol_funct_old = tex_file[begin_s:end_s].strip()
    tex_file.replace(sol_funct_old, sol_funct_new)
    with open('alarm-red-green-plots/3D/alarm_pla_graph_3D.tex', 'w') as f:
        f.write(tex_file)


if __name__ == "__main__":

    #flag variable that indicates whether the user wants to rerun the experiments: False is the default value
    flag = False
    if len(sys.argv) > 1:
        flag = parse_boolean(sys.argv[1])
    
    create_3D_graph(flag)

