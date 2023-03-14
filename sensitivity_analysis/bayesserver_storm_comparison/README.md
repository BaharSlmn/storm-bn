### perform_sensitivity_analysis.py
	- runs all benchmarks and plots the results (the file "plot.pdf" is created)
	- takes one boolean argument (optional)
		- if set to 'true': rerun the experiments, parse the results and then plot them
		- otherwise, the existing results will be plotted
	- command to run the script: python3 sensitivity_analysis_on_all_networks.py [true/false]  ([] indicates that the argument is optional)

### storm
	- contains all the benchmarks (drn_files) fed into storm, the queries (prop_files) and the corresponding results (output_files)
	
### bayesserver
	- contains all the benchmarks (network_files) fed int bayesserver and the corresponding results (output_files)
	
### bayesserver-9.5
	- the version of bayesserver that is used to run the experiments
	
### plot.tex 
	- the latex source that is called to plot the results 
