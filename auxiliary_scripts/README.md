### make_big_parametric.py
	- adds parameters to bif file

The script can be called as follows:
	- python3 make_bif_parametric.py --config_path <path to json file>

Explanation of the json file:

You always need this:

"network" : "cancer",
"bif_location" : "bif_files/cancer.bif",
"output_file" : "parametric_files/cancer5",

In the parameter part you can choose just the things you want to use each time, but you can also combine them all like shown here:

      "parameters" : {

        "non_distinct" : [
          [
            {
              "node" : "Cancer",
              "parent_evaluation" : "(low,False)"
            },
            {
              "node" : "Cancer",
              "parent_evaluation" : "(high,False)"
            }

          ]
        ],
        "CPT" :  ["Pollution", "Cancer"],
        "by_number_of_rows" : [
          {
            "node" :  "Xray",
            "number_of_rows" :  1,
            "parameter_position" : 0
          }
        ],
        "by_parent_evaluation" : [
          {
            "node" : "Dyspnoea",
            "parent_evaluation" : ["(False)"],
            "parameter_position" : 1
          }
        ]
      }

In non_distinct you create a list of lists. In each of the lists, the parameters are non-distinct.

'CPT' tells the script, which CPTs should be completely parametric.

'by_number_of_rows' adds parameters to the first 'number_of_rows'-many rows in the CPT and you can choose the position of the parameter. It does not need to be the first entry anymore.

'by_parent_evaluation' makes the rows described by the parent_evaluations parametric. Here you can give multiple parent_evaluations in the list. Again, the parameter_position can be chosen. (It should of course be a position that actually exists. So if there are only two probabilities in each row, just 0 and 1 are options.)

If you use --config_path, no other parameters in the console will be used.

Additionally the script can be called with the following parameters:

	- making random CPTs parametric: python3 make_bif_parametric.py --bif_location <location of bif file> --random_cpts <number of random_cpts> --random_cpts_non_distinct <flag, that tells if there should be non-distinct parameters, default = False> 

	- making random entries parametric: python3 make_bif_parametric.py --bif_location <location of bif file> --random_parameters <number of random parameters> --random_parameters_non_distinct <amount of non-distinct parameters, default = 0> 

	- making the whole network parametric: python3 make_bif_parametric.py --bif_location <location of bif file> --whole_network True 

In these three cases you can also use these parameters:

--output_file <path to output file for paramteric bif-file>

--output_file_original_values <path to output file for json file containing original parameter values>

--print_to_console <False -> not output is printed to the console. default = True>


### make_table.py
	- output: a table containing the properties of the computed sensitivity functions for all given networks 
	- command to run the script: python3 make_table.py

### make_drn_parametric.py
	- add parameters to the drn file

The script can be called as follows:
	- python3 make_drn_parametric.py --config_path <path to json file>

