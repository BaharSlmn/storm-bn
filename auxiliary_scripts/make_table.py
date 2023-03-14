import re
import pandas as pd
import math

filepaths = {
    'Rain' : 7
}


data = pd.DataFrame(columns=['network','time', 'bisimulation','property','nr_params', 'nr_states', 'nr_transitions', 'degree', 'nr_summands'])
for network, number in filepaths.items():
    print(network)
    for filepath,b in {f"dbn/{network}/{network}-{number}-out.txt": 'no'}.items(): #, f'../storm_pars_compute_function_test_bisimulation/{network}/{network}-{number}-out.txt' : 'yes'}.items():
        print(filepath)
        print(b)
       # filepath = f"dbn/{network}/{network}-{number}-out.txt"
        f = open(filepath,'r')
        content = f.read()
        begin_prop = content.find("--prop") +6
        end_prop = content.find("\n",begin_prop)
        property = content[begin_prop:end_prop]
        begin_time = content.find("Time for model construction: ") +29
        end_time = content.find("\n",begin_time)
        time = content[begin_time:end_time].strip()
        begin_states = int(content.find("States:") + 7)
        end_states = int(content.find("\n",begin_states))
        nr_states = content[begin_states:end_states].strip()
        print(f"States: {nr_states}")
        begin_transitions = int(content.find("Transitions:") + 12)
        end_transitions = int(content.find("\n", begin_transitions))
        nr_transitions = content[begin_transitions:end_transitions].strip()
        print(f"Transitions: {nr_transitions}")
        begin_function = int(content.find("Result (initial states): ") )#+ 25)
        end_function = int(content.find("\nTime",begin_function))
        function = content[begin_function:end_function]
        print(function)
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
                next_p = content.find('p', pos, next_plus)
                if next_p == -1:
                    pos = next_plus +1
                else:
                    next_p2 = content.find('p', next_p+1, next_plus)
                    sub_str = ""
                    exponent = 1
                    if next_p2 == -1:
                        sub_str = content[next_p: next_plus]
                    else:
                        sub_str = content[next_p: next_p2]
                    if sub_str.find('^') != -1:
                        sub_str = re.sub('[a-zA-Z]+[0-9]+', '', sub_str)
                        sub_str = sub_str.replace('^','')
                        exponent = re.findall('\d+', sub_str)[0]
                        pos = int(next_p) +2 + int(math.log10(int(exponent)))+1

                    else:
                        pos = next_p + 1
                    number_of_p += int(exponent)

            if max_number_of_p < number_of_p:
                max_number_of_p = number_of_p
            number_of_p = 0

        print(f"Maximum Degree: {max_number_of_p}")
        print(f"Number of Summands : {nr_terms}")
        print(f"Number of parameters: {number_of_params}")
        data = data.append({'network': network,'time': time, 'bisimulation': b, 'property': property,'nr_params':number_of_params, 'nr_states': nr_states, 'nr_transitions': nr_transitions, 'degree':max_number_of_p, 'nr_summands':nr_terms},ignore_index=True)
print(data)
data.to_csv('solution_function_compute.csv')
