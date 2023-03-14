import argparse


def get_tables(bif_content: str):
    probability_str = bif_content.split('probability ')[1:]
    table_dict = {}
    for prob in probability_str:
        node_and_parents = prob.split('(')[1].split(')')[0].strip()
        node = node_and_parents.split('|')[0].strip()
        parents = []
        if len(node_and_parents.split('|')) > 1:
            parents = node_and_parents.split('|')[1].split(',')
            parents = [p.strip() for p in parents]
        if len(parents) == 0:
            probabilities = prob.split('table')[1].split(';')[0].split(',')
            probabilities = {tuple(): [p.strip() for p in probabilities]}
        else:
            probabilities = {}
            lines = prob.split('{\n')[1].split('}')[0].split(';')[:-1]
            parent_valuations = {}
            i = 0
            for line in lines:
                parent_values = line.split('(')[1].split(')')[0].split(',')
                parent_values = [pv.strip() for pv in parent_values]
                parent_values = tuple(parent_values)
                parent_valuations[i] = parent_values
                i += 1
                prob_array = line.split(')',1)[1].split(',')
                prob_array = [p.strip() for p in prob_array]
                probabilities[parent_values] = prob_array

        temp_dict = {}
        temp_dict['parents'] = parents
        temp_dict['probabilities'] = probabilities
        if len(parents) >= 1:
            temp_dict['parent_valuations_in_order'] = parent_valuations

        table_dict[node] = temp_dict
    return table_dict


def add_parameters_to_cpt_by_parents(cpt, evaluation_of_parents, number_of_existing_parameters):
    number_of_params = number_of_existing_parameters

    if evaluation_of_parents != None:
        #print(evaluation_of_parents)
        for evaluation in evaluation_of_parents:
            #print(evaluation)
            evaluation = evaluation.split(',')
            evaluation = [e.strip() for e in evaluation]
            evaluation = tuple(evaluation)
            probability_row_old = cpt['probabilities'][evaluation]
            probability_row_new = [f'p{number_of_params}']
            for entry in probability_row_old[1:]:
                probability_row_new.append(f'{entry}*((1-p{number_of_params})/(1-{probability_row_old[0]}))')
            cpt['probabilities'][evaluation] = probability_row_new
            number_of_params += 1
    parameter_string = ''
    for i in range(number_of_params):
        parameter_string += f'parameter p{i}'+ '{\n}\n\n'
    return (cpt['probabilities']), parameter_string

def add_parameters_to_cpt_by_number(cpt, number):
    number_of_params = 0
    if number != None:
        if len(cpt['parents']) > 0:
            for i in range(number):
                evaluation = cpt['parent_valuations_in_order'][i]
                probability_row_old = cpt['probabilities'][evaluation]
                probability_row_new = [f'p{number_of_params}']
                for entry in probability_row_old[1:]:
                    probability_row_new.append(f'{entry}*((1-p{number_of_params})/(1-{probability_row_old[0]}))')
                cpt['probabilities'][evaluation] = probability_row_new
                number_of_params += 1
        else:
            probability_row_old = cpt['probabilities']
            probability_row_new = [f'p{number_of_params}']
            for entry in probability_row_old[1:]:
                probability_row_new.append(f'{entry}*((1-p{number_of_params})/(1-{probability_row_old[0]}))')
            cpt['probabilities'] = probability_row_new
            number_of_params += 1
    parameter_string = ''
    for i in range(number_of_params):
        parameter_string += f'parameter p{i}' + '{\n}\n\n'
    return (cpt['probabilities']), parameter_string

def add_params_to_bif(bif_file_path, node, evaluation_of_parents, number, output_path, number_of_existing_parameters=0):
    if (evaluation_of_parents != None) and (number != None):
        print('ERROR: You can either use evaluation_of_parents or number, but not both options!')

    if (evaluation_of_parents == None) and (number == None):
        print('ERROR: You have to use number or evaluation_of_parents!')
    f = open(bif_file_path, 'r')
    bif_content = f.read()
    f.close()
    table_dict = get_tables(bif_content)

    cpt = table_dict[node]
    if evaluation_of_parents != None:
        cpt['probabilities'], parameter_string = add_parameters_to_cpt_by_parents(cpt, evaluation_of_parents, number_of_existing_parameters)
    if number != None:
        cpt['probabilities'], parameter_string = add_parameters_to_cpt_by_number(cpt, number)

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
            new_bif_content += table_string + f'table {", ".join(node_dict["probabilities"])};\n' + '}\n\n'
    new_bif_content += parameter_string
    #print(new_bif_content)

    if output_path != None:
        g = open(output_path, 'w')
        g.write(new_bif_content)
        g.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Read Network from bif and add parameters')
    parser.add_argument('path', help='path to NON-PARAMETRIC bif-file', type=str)

    parser.add_argument('node', help='node, in which parameters should be added', type=str)
    parser.add_argument('--evaluation_of_parents', help='evaluation of parents marks the line in the CTL should be made parametric; not usable, when number is set', type=str, default=None, nargs='+')
    parser.add_argument('--number', help='number of lines that should be made parametric; not usable, when evaluation_of_parents is set', type=int, default=None)
    parser.add_argument('--existing_params',
                        help='number of already existing parameters',
                        type=int, default=0)
    parser.add_argument('--output_path', help='path to output-file', type=str)
    args = parser.parse_args()

    bif_file_path = args.path
    node = args.node
    # evaluation_of_parents is an array
    evaluation_of_parents = args.evaluation_of_parents
    number = args.number
    existing_params = args.existing_params
    output_path = args.output_path
    add_params_to_bif(bif_file_path, node, evaluation_of_parents, number, output_path, existing_params)
