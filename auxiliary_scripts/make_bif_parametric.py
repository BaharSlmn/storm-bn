import argparse
import json
from add_param_to_bif import get_tables
from random import shuffle
original_values = {}

def add_original_values(node, evaluation, original_value, parameter_number):
    global original_values
    if node not in original_values:
        original_values[node] = {}
    original_values[node][str(evaluation)] = {'parameter': f'p{parameter_number}' , 'original_value': original_value}

def make_row_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, evaluation, param_position=0):
    new_param = False

    if len(evaluation) > 0:

        if node not in parameter_name_dict.keys():
            parameter_name_dict[node] = {}

        if evaluation not in parameter_name_dict[node].keys():
            parameter_name_dict[node][evaluation] = number_of_added_parameters
            new_param = True
        parameter_number = parameter_name_dict[node][evaluation]

        old_value = cpt['probabilities'][evaluation][param_position]
        if not isfloat(old_value):
            print('ERROR: trying to parametrize an entry, that is not a float (so maybe already parametrized)')
        probability_row_old = cpt['probabilities'][evaluation]
        probability_row_new = []
        if old_value == '0.0' or old_value == '1.0' or old_value == '0' or old_value == '1' or len(evaluation) == 0:
            pass
            last_entry = '1-('
            for i in range(len(probability_row_old) - 1):
                probability_row_new.append(f'p{parameter_number}')
                last_entry += f'p{parameter_number}+'
                add_original_values(node, evaluation, probability_row_old[i], parameter_number)
                parameter_number += 1
            last_entry = last_entry[:-1] + ')'
            probability_row_new.append(last_entry)
            number_of_added_parameters += len(probability_row_old) - 2

        else:
            for i in range(len(probability_row_old)):
                if i == param_position:
                    probability_row_new.append(f'p{parameter_number}')
                    add_original_values(node, evaluation, probability_row_old[i], parameter_number)
                else:
                    probability_row_new.append(f'{probability_row_old[i]}*((1-p{parameter_number})/(1-{probability_row_old[param_position]}))')
        cpt['probabilities'][evaluation] = probability_row_new
        if new_param:
            number_of_added_parameters += 1
        update_name_dict_with_non_distinct(node, evaluation, parameter_name_dict, non_distinct)

    '''elif len(evaluation) < 0:
        if node not in parameter_name_dict.keys():
            parameter_name_dict[node] = number_of_added_parameters
            new_param = True
        parameter_number = parameter_name_dict[node]
        probability_row_old = cpt['probabilities'][evaluation]
        probability_row_new = [f'p{parameter_number}']
        for entry in probability_row_old[1:]:
            probability_row_new.append(f'{entry}*((1-p{parameter_number})/(1-{probability_row_old[0]}))')
        cpt['probabilities'][evaluation] = probability_row_new
        parameter_name_dict[node] = number_of_added_parameters

        update_name_dict_with_non_distinct(node, '', parameter_name_dict, non_distinct)
        if new_param:
            number_of_added_parameters += 1'''
    return number_of_added_parameters

def add_parameters_to_cpt_by_parents(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, evaluation_of_parents, param_position=0):
    if evaluation_of_parents != None:
        for evaluation in evaluation_of_parents:
            evaluation = make_tuple_from_string(evaluation)

            number_of_added_parameters = make_row_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, evaluation, param_position)

    return number_of_added_parameters

def make_tuple_from_string(tuple_string):
    if len(tuple_string[1:-1]) < 1:
        return tuple()
    tuple_string = tuple_string[1:-1].split(',')
    tuple_string = [e.strip() for e in tuple_string]
    result = tuple(tuple_string)
    return result

def update_name_dict_with_non_distinct(node, evaluation, parameter_name_dict, non_distinct):
    for nd in non_distinct:
        found = False
        for param in nd:
            if param['node'] == node and make_tuple_from_string(param['parent_evaluation']) == evaluation:
                found = True
        if found:
            if evaluation != '':
                parameter_name = parameter_name_dict[node][evaluation]
            else:
                parameter_name = parameter_name_dict[node]
            for param in nd:
                if param['parent_evaluation'] != '':
                    if param['node'] not in parameter_name_dict:
                        parameter_name_dict[param['node']] = {}
                    parameter_name_dict[param['node']][make_tuple_from_string(param['parent_evaluation'])] = parameter_name
                else:
                    parameter_name_dict[param['node']] = parameter_name
            break

def add_parameters_to_cpt_by_number(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, number, param_position=0):

    if number != None:
        if len(cpt['parents']) > 0:
            for i in range(number):
                evaluation = cpt['parent_valuations_in_order'][i]
                number_of_added_parameters = make_row_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, evaluation, param_position)

        else:
            number_of_added_parameters = make_row_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, tuple(''), param_position)

    return number_of_added_parameters



def make_cpt_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct):
    if len(cpt['parents']) == 0:
        number = 1
    else:
        number = len(cpt['parent_valuations_in_order'])
    number_of_added_parameters = add_parameters_to_cpt_by_number(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, number)
    return number_of_added_parameters


def make_n_random_parameters(bif_location, n, non_distinct, output_file, output_file_original_values, print_to_console):
    f = open(bif_location, 'r')
    bif_content = f.read()
    f.close()
    table_dict = get_tables(bif_content)
    number_of_added_parameters = 0
    table_size = len(table_dict)
    random_table_order = list(range(table_size))
    shuffle(random_table_order)
    parameter_name_dict = {}
    random_row_orders = {}
    for i in random_table_order:
        l = len(table_dict[list(table_dict.keys())[i]]['probabilities'])
        random_row_order = list(range(l))
        shuffle(random_row_order)
        random_row_orders[i] = random_row_order
        round = 0
    amount_of_parameters = 0
    change = True

    while (amount_of_parameters < n and change == True):
        change = False
        for i in random_table_order:
            node = list(table_dict.keys())[i]
            cpt = table_dict[node]
            if round < len(list(cpt['probabilities'].keys())):
                evaluation = list(cpt['probabilities'].keys())[random_row_orders[i][round]]
                number_of_added_parameters = make_row_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, [], evaluation)
                if amount_of_parameters < non_distinct - 1:
                    number_of_added_parameters -= 1
                change = True
                amount_of_parameters += 1
                print(amount_of_parameters)
            if amount_of_parameters == n:
                break


        round += 1

    print_parametric_bif(bif_content, table_dict, number_of_added_parameters, output_file, output_file_original_values, print_to_console)


def make_whole_network_parametric(bif_location, output_file, output_file_original_values, print_to_console):
    f = open(bif_location, 'r')
    bif_content = f.read()
    f.close()
    table_dict = get_tables(bif_content)
    number_of_added_parameters = 0
    table_size = len(table_dict)
    random_table_order = list(range(table_size))
    shuffle(random_table_order)
    parameter_name_dict = {}
    non_distinct = []
    for node in table_dict.keys():
        cpt = table_dict[node]
        number_of_added_parameters = make_cpt_parametric(node, cpt, parameter_name_dict, number_of_added_parameters,
                                                         non_distinct)
    print_parametric_bif(bif_content, table_dict, number_of_added_parameters, output_file, output_file_original_values, print_to_console)

def make_n_cpts_parametric(bif_location, n, output_file, output_file_original_values, print_to_console, non_distinct_cpt):
    f = open(bif_location, 'r')
    bif_content = f.read()
    f.close()
    table_dict = get_tables(bif_content)
    number_of_added_parameters = 0
    max_number_of_added_parameters = 0
    table_size = len(table_dict)
    random_table_order = list(range(table_size))
    shuffle(random_table_order)
    parameter_name_dict = {}
    non_distinct = []
    amount_of_parametric_cpts = 0
    for i in random_table_order:
        if amount_of_parametric_cpts == n:
            break
        node = list(table_dict.keys())[i]
        cpt = table_dict[node]
        number_of_added_parameters = make_cpt_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct)
        max_number_of_added_parameters = max([number_of_added_parameters, max_number_of_added_parameters])
        amount_of_parametric_cpts += 1
        if non_distinct_cpt:
            print(non_distinct_cpt)
            number_of_added_parameters = 0
    print_parametric_bif(bif_content, table_dict, max_number_of_added_parameters, output_file, output_file_original_values, print_to_console)

def make_bif_parametric(config_path, output_file_original_values, print_to_console):
    with open(config_path, "r") as read_file:
        config = json.load(read_file)
    network = config['network']
    bif_location = config['bif_location']
    output_file = config['output_file']
    non_distinct = config['parameters']['non_distinct'].copy()
    CPTs = config['parameters']['CPT']
    by_number_of_rows = config['parameters']['by_number_of_rows'].copy()
    by_parent_evaluation = config['parameters']['by_parent_evaluation'].copy()
    parameter_name_dict = {}
    number_of_added_parameters = 0
    f = open(bif_location, 'r')
    bif_content = f.read()
    f.close()
    table_dict = get_tables(bif_content)
    for node in CPTs:
        cpt = table_dict[node]
        number_of_added_parameters = make_cpt_parametric(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct)
    for param in by_number_of_rows:

        node = param['node']
        cpt = table_dict[param['node']]
        number_of_added_parameters = add_parameters_to_cpt_by_number(node, cpt, parameter_name_dict,
                                                                     number_of_added_parameters, non_distinct, param['number_of_rows'], param['parameter_position'])
    for param in by_parent_evaluation:

        node = param['node']

        cpt = table_dict[param['node']]
        evaluation_of_parents = param['parent_evaluation']
        number_of_added_parameters = add_parameters_to_cpt_by_parents(node, cpt, parameter_name_dict, number_of_added_parameters, non_distinct, evaluation_of_parents, param['parameter_position'])

    print_parametric_bif(bif_content, table_dict, number_of_added_parameters, output_file, output_file_original_values, print_to_console)

def print_parametric_bif(bif_content, table_dict, number_of_added_parameters, output_file, output_file_original_values, print_to_console):
    global original_values
    new_bif_content = bif_content.split('probability', 1)[0]
    for node_name, node_dict in table_dict.items():
        parent_string = ''
        if len(node_dict['parents']) > 0:
            parent_string = '| ' + ', '.join(node_dict['parents']) + ' '
        table_string = f'probability ( {node_name} {parent_string})' + '{\n'
        if len(node_dict['parents']) > 0:
            for parents_valuation, probs in node_dict['probabilities'].items():
                table_string += f'({", ".join(parents_valuation)}) '
                table_string += f'{", ".join(probs)};\n'
            new_bif_content += table_string + '}\n\n'
        else:
            new_bif_content += table_string + f'table {", ".join(node_dict["probabilities"][tuple()])};\n' + '}\n\n'
    parameter_string = ''
    for i in range(number_of_added_parameters):
        parameter_string += f'parameter p{i}' + '{\n}\n\n'
    new_bif_content += parameter_string

    if output_file != None:

        f = open(output_file, 'w')
        f.write(new_bif_content)
        f.close()
    if output_file_original_values != None:
        with open(output_file_original_values, "w") as write_file:
            json.dump(original_values, write_file, indent=4)
    if print_to_console:
        print('##### new bif-file #####')
        print(new_bif_content)
        print('##### original parameter values #####')
        print(original_values)

def isfloat(value):
  try:
    float(value)
    return True
  except ValueError:
    return False


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Make bif file parametric')
    parser.add_argument('--config_path', help='path to config file in json format', type=str, default=None)
    parser.add_argument('--random_cpts', help='number of random cpts', type=int, default=None)
    parser.add_argument('--bif_location', help='path to bif file', type=str, default=None)
    parser.add_argument('--output_file', help='path to output file', type=str, default=None)
    parser.add_argument('--output_file_original_values', help='path to output file for original values of parameters in json format', type=str, default=None)
    parser.add_argument('--random_parameters', help='number of random parameters', type=int, default=None)
    parser.add_argument('--random_parameters_non_distinct', help='number of random parameters, which should be non_distinct', type=int, default=0)
    parser.add_argument('--random_cpts_non_distinct', help='flag, that tells if there should be non-distinct parameters',
                        type=bool, default=False)
    parser.add_argument('--whole_network', help='flag, which tells if whole network should be parametric', type=str, default=None)
    parser.add_argument('--print_to_console', help='flag, which tells if output should be (additionally) printed to console', type=bool,
                        default=False)



    args = parser.parse_args()
    config_path = args.config_path
    n_cpts = args.random_cpts
    n = args.random_parameters
    non_distinct = args.random_parameters_non_distinct
    non_distinct_cpt = args.random_cpts_non_distinct
    bif_location = args.bif_location
    output_file = args.output_file
    output_file_original_values = args.output_file_original_values
    whole_network = args.whole_network
    print_to_console = args.print_to_console

    if config_path != None:
        make_bif_parametric(config_path, output_file_original_values, print_to_console)
    elif n_cpts != None:
        if bif_location == None:
            print('ERROR: bif location has to be set')
        else:
            make_n_cpts_parametric(bif_location, n_cpts, output_file, output_file_original_values, print_to_console, non_distinct_cpt)
    elif n != None:
        if bif_location == None:
            print('ERROR: bif location has to be set')
        make_n_random_parameters(bif_location, n, non_distinct, output_file, output_file_original_values, print_to_console)
    elif whole_network:
        make_whole_network_parametric(bif_location, output_file, output_file_original_values, print_to_console)

