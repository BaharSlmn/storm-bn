### perform_parameter_space_partitioning.sh
	- runs all experiments related to parameter space partitioning
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: ./perform_parameter_space_partitioning.sh [true/false]  ([] indicates that the argument is optional)

### scripts/create_2D_graph.py
	- creates the 2D graph for alarm
	- output: alarm_pla_graph_2D.pdf in the directory generated_plots

### scripts/create_3D_graph.py
	- creates the 3D graph for alarm
	- output: alarm_pla_graph_3D.pdf in the directory generated_plots

### scripts/run_pla_experiments.py
	- runs all PLA experiments and plots the results
	- output: pla_plot.pdf in the directory generated_plots
	
### scripts/make_pla_table.py
	- outputs the first few rows of the table containing information about parameter space partitioning for different coverage values 
	for the network win95pts

### alarm-red-green-plots
	- contains all data relevant to construct the 2D and 3D graphs for alarm
		
### generated_files
	- contains all the generated plots (also included in the paper) and the PLA table for win95pts
	
### output
	- contains the output data for the specific threshhold values (e.g. 0.3 for hailfinder)
	
### jani_files
	- contains all jani files fed into storm to perform the PLA experiments

### pctl_files
	- contains the pctl files that specify the properties to be checked for each network
	

