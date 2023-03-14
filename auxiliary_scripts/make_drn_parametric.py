import argparse
import json
import itertools

def is_float(string):
    try:
        float(string)
        return True
    except ValueError:
        return False


def separate_name_and_value(statename):
    attribute = statename[:-1]
    value = statename[-1]
    if not value.isnumeric():
        value = None
        attribute = statename
    return attribute, value
def read_drn_file(filepath: str):
    with open(filepath, "r") as read_file:
        drn_file = read_file.read()

    drn_states_text = drn_file.split('state ')[1:]
    header = drn_file.split('state ')[0]

    type = (header.split('@parameters')[0].split('@type:')[1]).strip()

    parameters = (header.split('@parameters')[1]).split('@')[0].strip()

    if parameters != '':
        print('ERROR: drn file should not be parametric! ')
        return -1
    placeholders = ''
    reward_models = header.split('@reward_models')[1].split('@')[0].strip()

    nr_states = header.split('@nr_states')[1].split('@')[0].strip()
    nr_choices = header.split('@nr_choices')[1].split('@')[0].strip()
    drn_states_array = [[y.strip() for y in x.split('\n') if y != ''] for x in drn_states_text]
    drn = Drn(type, parameters, placeholders, reward_models, nr_states, nr_choices)
    for state_array in drn_states_array:

        state_number, name = state_array[0].split(' ',1)
        new_state = State(state_number, name)
        for transition in state_array[2:]:
            child_number, prob = transition.split(':')
            child_number = child_number.strip()
            prob = prob.strip()
            new_state.add_child(child_number, prob)
        drn.states[state_number] = new_state
    for state in drn.states.values():

        for child_number in state.children:
            drn.states[child_number.strip()].add_parent(state)

    #for state in drn.states.values():
    #    print(state)
    return drn


class State():
    def __init__(self, number, name):
        self.number = number
        self.name = name
        self.parents = []
        self.children = []
        self.probs = {}
        attribute, value = separate_name_and_value(name)
        if value != None:
            self.valuation = {attribute: value}
        else:
            self.valuation = {}
    def add_parent(self, parent):
        self.parents.append(parent.number)
        for attribute, value in parent.valuation.items():
            if attribute in self.valuation and value != self.valuation[attribute]:
                self.valuation[attribute] = None
            else:
                self.valuation[attribute] = value
    def add_child(self, child_number, prob):
        self.children.append(child_number)
        self.probs[child_number] = prob


    def __str__(self):
        return f'''number: {self.number}
name = {self.name}
parents = {self.parents}
children = {self.children}
probs = {self.probs}
valuation = {self.valuation}
'''

class Drn():
    def __init__(self, type, parameters, placeholders, reward_models, nr_states, nr_choices):
        self.states = {}
        self.type = type
        self.parameters = parameters
        self.placeholders = {}
        self.reward_models = reward_models
        self.nr_states = nr_states
        self.nr_choices = nr_choices
        self.number_of_params = 0
        self.number_of_placefolder = 0

    def make_entry_parametric_by_parent_value(self, state_name, parent_names, parent_values, number):
        if self.number_of_params == number:
            return

        transitions = self.find_transitions_for_making_parametric_by_parent_value(state_name, parent_names, parent_values)
        #print(state_name)
        #print(transitions)
        #print(transitions)
        #print(transitions)
        added = False
        for param_child, parent_list in transitions.items():
            for parent in parent_list:
                old_param_child_prob = self.states[parent].probs[param_child]
                if old_param_child_prob == '1.0' or old_param_child_prob == '1':
                    break
                elif not is_float(old_param_child_prob):
                    continue
                else:
                    added = True
                for child in self.states[parent].children:
                    if child == param_child:
                        self.states[parent].probs[child] = f'p{self.number_of_params}'

                    else:
                        old_prob = self.states[parent].probs[child]
                        self.states[parent].probs[child] = f'{old_prob} * ((1 - p{self.number_of_params})/(1 - {old_param_child_prob}))'
                        #print(self.states[parent].probs[child])
                #print(parent)
                #print(self.states[parent].probs)
        if added:
            self.number_of_params += 1
    def make_CPT_parametric(self, attribute, number):
        #print(attribute)
        parent_names = attribute["parent_names"]
        for parent_values in list(itertools.product(*attribute["possible_parent_values"])): # zip(*(attribute["possible_parent_values"])):
            #print(attribute["node"])
            #print(parent_names)
            #print(list(parent_values))
            self.make_entry_parametric_by_parent_value(attribute['node'],parent_names,list(parent_values), number)
    def find_transitions_for_making_parametric_by_parent_value(self, state_name, parent_names, parent_values):
        #print('###### BEGIN OF FINDING TRANSISTION')
        #print(state_name)
        state_numbers_with_name = []
        for number, state in self.states.items():
            if state_name == state.name or f"{state_name} deadlock" == state.name or f"deadlock {state_name}" == state.name:
                state_numbers_with_name.append(number)
        #print('state_numbers_with_name')
        #print(state_numbers_with_name)
        #print(state_numbers_with_name)
        #print('STATE_NAME')
        #print(state_name)
        parent_state_numbers_with_correct_values = {}
        #print(state_numbers_with_name)
        for child_number in state_numbers_with_name:
            parent_state_numbers_with_correct_values[child_number] = []

            for number in self.states[child_number].parents:
                #print(f'number: {number}')
                #print('valutation')
                #print(self.states[number].valuation)
                correct_parent_valuation = True
                for parent, parent_value in zip(parent_names, parent_values):
                    #print(number)
                    #print(self.states[number].valuation)
                    #print(f'parent: {parent}')
                    #print(f'parent_value: {parent_value}')
                    if parent not in self.states[number].valuation:
                        correct_parent_valuation = False

                    elif self.states[number].valuation[parent] == None:
                        correct_parent_valuation = False
                    elif self.states[number].valuation[parent] != parent_value:
                        correct_parent_valuation = False
                if correct_parent_valuation:
                    parent_state_numbers_with_correct_values[child_number].append(number)
        #print(parent_state_numbers_with_correct_values)
        return parent_state_numbers_with_correct_values

    def write_to(self, output_file: str, number):
        header = f'''@type: {self.type}
@parameters
{' '.join(['p'+str(i) for i in range(self.number_of_params)])}
@reward_models
{self.reward_models}
@nr_states
{self.nr_states}
@nr_choices
{self.nr_choices}
'''
        body = "@model"
        for state in self.states.values():

            state_str = f'''
state {state.number} {state.name}
\taction 0
'''
            for child in state.children:
                state_str += f'\t\t{child} : {state.probs[child]}\n'
            body += state_str[:-1]
        if number != -1:
            output_location = output_file[:-4]+ '_'+str(number) + '.drn'
        else:
            output_location = output_file[:-4]+ '_all' + '.drn'
        g = open(output_location, 'w')
        g.write(header + body)

def make_drn_parametric(config_path: str, number: int, output_path: str):
    with open(config_path, "r") as read_file:
        config = json.load(read_file)
    network = config['network']
    drn_location = config['drn_location']
    if output_path == None:
        output_path = config['output_file']
    CPTs = config['parameters']['CPT']
    by_parent_evaluation = config['parameters']['by_parent_evaluation'].copy()
    drn = read_drn_file(drn_location)
    for param in CPTs:
        drn.make_CPT_parametric(param, number)
    for param in by_parent_evaluation:
        drn.make_entry_parametric_by_parent_value(param['node'], param['parent_names'], param['parent_values'], number)

    drn.write_to(output_path, number)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='make drn file parametric')
    parser.add_argument('config_path', help='path to config_file', type=str)
    parser.add_argument('--number', help='number of params, that should be added', type=int, default=-1)
    parser.add_argument('--begin', help='begin number of params, that should be added', type=int, default=-1)
    parser.add_argument('--end', help='end number of params, that should be added', type=int, default=-1)
    parser.add_argument('--step', help='step number of params, that should be added', type=int, default=-1)
    parser.add_argument('--output_path', help='output_path', type=str, default = None)

    args = parser.parse_args()

    config_path = args.config_path
    output_path = args.output_path
    number = args.number
    begin = args.begin
    end = args.end
    step = args.step
    if begin != -1 and end != -1 and step != -1:
        count = 0
        i = 1
        while i <= end + 1 :
            make_drn_parametric(config_path, i, output_path)
            count += 1
            i = 2 ** count
    else:
        make_drn_parametric(config_path, number, output_path)
