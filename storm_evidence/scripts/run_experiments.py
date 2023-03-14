import os
from unittest import skip
import pandas as pd


networks = ['asia','alarm',  'cancer', 'child', 'earthquake', 'insurance', 'sachs', 'survey', 'andes', 'hepar2']
ev_nodes_percentage = [1,5,10,30]

def run_experiments():
    for network in networks:
        for perc in ev_nodes_percentage:
            query_file = open(f'query_files/{perc}/{network}.query', 'r')
            #run query on evidence tailored model
            query = query_file.read()
            os.system(f'{query} > output/{perc}/{network}_output.txt')
            #run query on evidence agnostic model
            query_file_agn1 =open(f'query_files/{perc}/no_evidence/{network}_numerator.query', 'r')
            query_file_agn2 =open(f'query_files/{perc}/no_evidence/{network}_denominator.query', 'r')

            query_agn1 = query_file_agn1.read()
            query_agn2 = query_file_agn2.read()

            os.system(f'{query_agn1} > output/{perc}/no_evidence/{network}_numerator_output.txt')
            os.system(f'{query_agn2} > output/{perc}/no_evidence/{network}_denominator_output.txt')

    
def parse_results():
    compile_time_data = pd.DataFrame(columns=['network','agnostic','tailored','scatterclass'])
    mtbdd_nodes_data = pd.DataFrame(columns=['network','agnostic','tailored','scatterclass'])
    inference_time_data = pd.DataFrame(columns=['network','agnostic','tailored','scatterclass'])

    for network in networks:
        for perc in ev_nodes_percentage:
            #parse evidence agnostic network
            f1 = open(f'output/{perc}/no_evidence/{network}_numerator_output.txt')
            f2 = open(f'output/{perc}/no_evidence/{network}_denominator_output.txt')
            agnostic_output_1 = f1.read()
            agnostic_output_2 = f2.read()
            #parse inference time
            begin_inf_a1 = agnostic_output_1.find('Time for model checking: ') + 25
            end_inf_a1 = agnostic_output_1.find('s.', begin_inf_a1)
            time_inf_a1 = agnostic_output_1[begin_inf_a1:end_inf_a1].strip()

            begin_inf_a2 = agnostic_output_2.find('Time for model checking: ') + 25
            end_inf_a2 = agnostic_output_2.find('s.', begin_inf_a2)
            time_inf_a2 = agnostic_output_2[begin_inf_a2:end_inf_a2].strip()
            #parse compile time
            begin_comp_a = agnostic_output_1.find('Time for model construction: ') + 29
            end_comp_a = agnostic_output_1.find('s.', begin_comp_a)
            time_comp_a = agnostic_output_1[begin_comp_a:end_comp_a].strip()
            #parse MTBDD node number
            begin_line = agnostic_output_1.find('States: ') + 8
            end_line = agnostic_output_1.find('\n', begin_line)
            nodes_a = agnostic_output_1[begin_line:end_line].strip()

            #parse inference result
            begin_t1 = agnostic_output_1.find('Result (for initial states): ') + 29
            end_t1 = agnostic_output_1.find('\n', begin_t1)
            t1 = agnostic_output_1[begin_t1:end_t1]
            begin_t2 = agnostic_output_2.find('Result (for initial states): ') + 29
            end_t2 = agnostic_output_2.find('\n', begin_t2)
            t2 = agnostic_output_2[begin_t2:end_t2]
            try:
                t1 = float(t1)
                t2 = float(t2)
                f = open(f'output/{perc}/no_evidence/{network}_result.txt', 'w+')
                if t2 != 1:
                    time = (1-t1)/(1-t2)
                    f.write(str(time))
                else:
                    f.write(str(0.5))

            except ValueError:
                skip
            #parse evidence tailored network
            f1 = open(f'output/{perc}/{network}_output.txt')
            tailored_output = f1.read()
            #parse inference time
            begin_inf_t = tailored_output.find('Time for model checking: ') + 25
            end_inf_t = tailored_output.find('s.', begin_inf_t)
            time_inf_t = tailored_output[begin_inf_t:end_inf_t].strip()
            #parse compile time
            begin_comp_t = tailored_output.find('Time for model construction: ') + 29
            end_comp_t = tailored_output.find('s.', begin_comp_t)
            time_comp_t = tailored_output[begin_comp_t:end_comp_t].strip()
            #parse MTBDD node number
            begin_line = tailored_output.find('States:') + 8
            end_line = tailored_output.find('\n', begin_line)
            nodes_t = tailored_output[begin_line:end_line].strip()
            flag = False
            exp = ['model checking', 'model construction', 'States']
            for e in exp:
                if not e in agnostic_output_1 or not e in agnostic_output_2:
                    flag= True
                if not e in tailored_output: 
                    flag= True
            if not flag:
                time_inf_a = float(time_inf_a1)+ float(time_inf_a2) 
                if time_inf_a < 0.01:
                    time_inf_a = 0.01
                if float(time_inf_t) < 0.01:
                    time_inf_t=0.01

                df1 = pd.DataFrame([[f'{network}',time_inf_a, time_inf_t, f'qual{perc}']],columns=['network','agnostic','tailored','scatterclass'])    
                inference_time_data = pd.concat([inference_time_data,df1])  
                df2 = pd.DataFrame([[f'{network}',nodes_a, nodes_t, f'qual{perc}']],columns=['network','agnostic','tailored','scatterclass'])    
                mtbdd_nodes_data = pd.concat([mtbdd_nodes_data,df2])  
                df3 = pd.DataFrame([[f'{network}',time_comp_a, time_comp_t, f'qual{perc}']],columns=['network','agnostic','tailored','scatterclass'])    
                compile_time_data = pd.concat([compile_time_data,df3])  

    compile_time_data.to_csv('generated_files/compile_time.csv', index=False)
    inference_time_data.to_csv('generated_files/inference_time.csv', index=False)
    mtbdd_nodes_data.to_csv('generated_files/mtbdd_nodes.csv', index=False)


if __name__ == "__main__":
        run_experiments()
        parse_results()

