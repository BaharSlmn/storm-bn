import argparse

def compute_number_of_parameters(function: str, maximum: int) -> int:
    counter = 0
    for i in range(maximum+1):
        if f'p{i}:' in function or f'p{i}=' in function:
            counter += 1
    return counter

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='compute number of added parameters, that occur in the given function')
    parser.add_argument('path', help='path to function of drn file with added parameters of the form a0,a1,a2,a3', type=str)
    parser.add_argument('maximum',
                        help='amount of parameters that were added randomly to drn file', type=int)
    args = parser.parse_args()

    drn_file_path = args.path
    maximum = args.maximum
    f = open(drn_file_path, 'r')
    text = f.read()
    if (len(text)>0):
        if len(text.split('Point found: {')) > 1:
            function = text.split('Point found: {')[1]
            function = function.split('This')[0]
            print(compute_number_of_parameters(function, maximum))
        elif len(text.split("Code 124")) > 1:
            print(-1)
        elif len(text.split('Parameters:')) > 1:
            function = text.split('Parameters:')[1]
            #function = function.split('This')[0]
            print(compute_number_of_parameters(function, maximum))
        else:
            print(-1)
        #print(drn_file_path)
