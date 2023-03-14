### run_disjunction_experiments.py
	- runs all the experiments when considering the disjunction of multiple hypothesis nodes
	- generates a csv file that contains information about the inference time for both storm and ace depending on the number of the hypothesis nodes and a plot that visualizes the results
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: python3 run_disjunction_experiments.py [true]  ([] indicates that the argument is optional)


### disjunction_plot.tex
	- latex source code used to visalize the results
