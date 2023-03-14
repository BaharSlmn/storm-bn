import os
import pandas as pd

def make_pla_table():
    data = pd.DataFrame(columns=['coverage','accepting(%)','rejecting(%)', 'unknown(%)', '#regions', 'time', 'mem'])
    f = open(f'output/win95pts_PLA_p8c1.csv', 'r')
    lines = f.readlines()
    lines = lines[1:]

    for line in lines:
        values = line.split(',')
        coverage = values[1]
        sat = values[2]
        unsat = values[3]
        unknown = values[4]
        regions = values[5]
        time = values[6]
        mem = values[7]
        mem = mem.replace('\n', '')
        df = pd.DataFrame([[coverage, sat, unsat, unknown, regions, time, mem]],columns=['coverage','accepting(%)','rejecting(%)', 'unknown(%)', '#regions', 'time', 'mem'])            
        data = pd.concat([data,df],ignore_index=True)
    data.to_csv('generated_files/PLA_table.csv', index=False)
    print(data.tail(16))

if __name__ == "__main__":
        make_pla_table()

