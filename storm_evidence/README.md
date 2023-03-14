### run_storm_experiments.sh
	- runs all experiments using storm to compare inference on evidence-tailored and on evidence-agnostic networks
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: ./run_storm_experiments.sh [true]  ([] indicates that the argument is optional)

### scripts/construct_ev_tailored_networks.py
	- creates the bif files containing random evidence and hypothesis nodes and the corresponding query files

### scripts/run_experiments.py
	- reruns all experiments and saves the results in csv files 

### generated_files
	- contains all generated csv files that contain the information regarding inference time, construction time, and the number of nodes for the results, as well as the plots that visualize the results.

### jani_files
	- contains all jani files fed into storm to perform the experiments
	- each folder represents the chosen percentage of evidence nodes compared to the overall number of nodes
	
### output
	- contains the results
	
### query_files
	- contains the query files corresponding to the generated bif files, for both evidence-agnostic and evidence-tailored networks.
	
### tex_files
	- contains the latex scripts used to visualize the results 
