import sys
import pandas as pd
import os

filepaths = {
    'child' : 60,
    'alarm' : 85,
    'hepar2' : 135,
    'insurance': 140,
    'win95pts' : 200,
    'water': 255,
    'hailfinder': 380
}
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

    if flag:
        #rerun the experiments using Storm 
        os.system('./storm_pars.sh child 60 230 GruntingReport LowerBodyO2 RUQO2 CO2Report XrayReport')
        os.system('./storm_pars.sh alarm 85 509 BP')
        os.system('./storm_pars.sh hailfinder 380 2656 R5Fcst')
        os.system('./storm_pars.sh hepar2 135 1453 ggtp')
        os.system('./storm_pars.sh insurance 140 984 PropCost')
        os.system('./storm_pars.sh win95pts 200 574 Problem1')
        os.system('./storm_pars.sh water 255 10083 C_NI_12_45 CKNI_12_45 CNON_12_45 CKNN_12_45 CBODN_12_45 CNOD_12_45 CKND_12_45 CBODD_12_45')


    data = pd.DataFrame(columns=['network','nr_params','time [s]', 'nr_states', 'nr_transitions', 'degree', 'nr_summands'])
    for network, number in filepaths.items():
        for filepath,b in {f'{network}/{network}-{number}-out.txt' : 'yes'}.items():

            f = open(filepath,'r')
            content = f.read()
            begin_prop = content.find("--prop") +6
            end_prop = content.find("\n",begin_prop)
            property = content[begin_prop:end_prop]
            begin_time = content.find("Time for model checking") +25
            end_time = content.find("\n",begin_time)
            time = content[begin_time:end_time].strip()
            time = time[:-1]
            begin_states = int(content.find("States:") + 7)
            end_states = int(content.find("\n",begin_states))
            nr_states = content[begin_states:end_states].strip()
            begin_transitions = int(content.find("Transitions:") + 12)
            end_transitions = int(content.find("\n", begin_transitions))
            nr_transitions = content[begin_transitions:end_transitions].strip()
            begin_function = int(content.find("Result (initial states): ") )#+ 25)
            end_function = int(content.find("\nTime",begin_function))
            function = content[begin_function:end_function]
            number_of_params = 0
            for i in range(number):
                if content.find(f"p{i}",begin_function) != -1:
                    number_of_params += 1
            pos = begin_function
            number_of_p = 0
            max_number_of_p = 0
            nr_terms = 0
            while pos < len(content):
                next_plus = content.find('+',pos)
                nr_terms += 1
                if next_plus == -1:
                    next_plus = len(content)
                while pos < next_plus:
                    next_p = content.find('p',pos,next_plus)
                    if next_p == -1:
                        pos = next_plus +1
                    else:
                        number_of_p +=1 
                        pos = next_p+1
                        
                if max_number_of_p < number_of_p:
                    max_number_of_p = number_of_p
                number_of_p = 0
            df = pd.DataFrame([[network, number, time, nr_states, nr_transitions, max_number_of_p, nr_terms]],columns=['network','nr_params','time [s]', 'nr_states', 'nr_transitions', 'degree', 'nr_summands'])            
            data = pd.concat([data,df], ignore_index=True)
    print(data)
    #data.to_csv('solution_function_computation.csv')
