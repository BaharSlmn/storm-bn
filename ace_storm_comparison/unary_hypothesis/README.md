### run_unary_hyp_experiments.py
	- runs the experiments when considering only one hypothesis node
	- generates a csv file that contains information about the inference time for both storm and ace and a plot that visualizes the results
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: python3 run_disjunction_experiments.py [true]  ([] indicates that the argument is optional)


### unary_hyp_plot.tex
	- contains the latex source code used to visalize the results
