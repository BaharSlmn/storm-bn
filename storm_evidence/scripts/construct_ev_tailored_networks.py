import os
import pandas as pd
import math
import random

networks = ['asia','alarm',  'cancer', 'child', 'earthquake', 'insurance', 'sachs', 'survey', 'andes', 'hepar2']
ev_nodes_percentage = [5]


def construct_ev_tailored_networks(): 
    
    for network in networks:
        var_file = open(f'var_files/{network}.var')
        content = var_file.readlines()
        var_number = sum(1 for line in open(f'var_files/{network}.var'))
        bif_file = open(f'bif_files/no_evidence/{network}.bif')
        bif_content = bif_file.read()
        for perc in ev_nodes_percentage:
            ev_list = []
            ev_val_list = []
            vars_to_consider = math.ceil(perc/100 * var_number)
            #create list with random numbers to select variables randomly
            var_random_list = random.sample(range(var_number-1), vars_to_consider)
            var_random_list.sort()
            for index in var_random_list:
                var_line = content[index]
                #parse var line
                ind_v = var_line.find('-')
                var_name = var_line[:ind_v].strip()    
                ev_list.append(var_name)
                end_val = var_line.find('\n')
                ev_val_numb = int(var_line[ind_v+1:end_val].strip())
                ev_val_list.append(ev_val_numb)

            ev_string = f'evidence first {{\n'
            for var in ev_list:
                ev_string += f'\t{var}=0;\n'
            ev_string += f'}}\n'
            hyp_random = random.sample(range(var_random_list[vars_to_consider-1]+1, var_number), 1)
            hyp_line = content[hyp_random[0]]
            #parse var line
            end_h = hyp_line.find('-')
            hypothesis = hyp_line[:end_h].strip()
            hyp_value = hyp_line[end_h+1:].strip()
            
            #create query
            create_query_ev(hypothesis, network, perc)
            create_query_no_ev(hypothesis, int(hyp_value), ev_list, ev_val_list, network, perc)


            hyp_string = f'hypothesis second {{\n\t{hypothesis}=0;\n}}'

            #create bif file with hypothesis and evidence
            perc_bif_file = open(f'bif_files/{perc}/{network}.bif', 'w')
            perc_bif_file.write(f'{bif_content}{ev_string}{hyp_string}')



def create_query_ev(hypothesis:str, network:str, perc:int):
    query_file = open(f'query_files/{perc}/{network}.query', 'w')
    query_str = f'storm --jani jani_files/{perc}/{network}.jani --prop "P=? [G({hypothesis}=0 | {hypothesis}=-1)]"'
    #for var in var_list:
    query_file = open(f'query_files/{perc}/{network}.query', 'w')
    query_file.write(query_str)
    query_file.close()

def create_query_no_ev(hypothesis, hyp_value, ev_list, ev_val_list, network, perc):
    query_file_1 = open(f'query_files/{perc}/no_evidence/{network}_numerator.query', 'w')
    query_file_2 = open(f'query_files/{perc}/no_evidence/{network}_denominator.query', 'w')
    query_str = f'storm --jani jani_files/no_evidence/{network}.jani --prop "P=? [F('

    for i in range (len(ev_list)):
        for val in range(1, ev_val_list[i]):
            query_str += f'{ev_list[i]}={val}|'

    query_str_2 = query_str

    for val in range (1, hyp_value):
        query_str += f'{hypothesis}={val}|'
    query_str_1 = query_str[:-1]
    query_str_2 = query_str_2[:-1]
    query_str_1 += ')]"'
    query_str_2 += ')]"'
    query_file_1.write(query_str_1)
    query_file_2.write(query_str_2)

if __name__ == "__main__":
    print('Construct evidence-tailored networks!')
    construct_ev_tailored_networks()
