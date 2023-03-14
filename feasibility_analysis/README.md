### perform_feasibility_analysis.sh
	- runs all experiments and plots the results
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: ./perform_feasibility_analysis.sh [true/false]  ([] indicates that the argument is optional)

### csv_files
	- contains the csv files generated from Storm's results

### generated_plots
	- contains all the generated plots (also included in the paper)
	
### latex_source
	- contains the latex source needed to generate the plots

### pso-qcqp-gd-benchmarks
	- contains all the benchmarks fed into storm/prophesy and the results 
	
### storm
	- version of storm used to perform feasibility analysis with GD
