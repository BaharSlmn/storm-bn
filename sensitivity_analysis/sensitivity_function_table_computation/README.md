### make_table.py
	- runs all the benchmarks with more than two parameters (pn)
	- output: table containing information about the computed sensitivity functions 
	- takes one boolean argument (optional)
		- 'true': rerun the experiments, parse the results and plot them (if not set, the existing results will be plotted)
	- command to run the script: python3 make_table.py [true/false]  ([] indicates that the argument is optional)

### storm_pars.sh
	- shell script that is used in make_table.py to run all the benchmarks using storm
	
### alarm, child, hailfinder, hepar2, insurance, water, win95pts
	- contain the benchmarks (drn_files) fed into storm and the results (-out.txt files)
