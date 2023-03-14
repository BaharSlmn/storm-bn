#!/bin/bash

#build storm
cd storm
if [ ! -d "build" ]; then
	mkdir build
fi
cd build
cmake .. 
make
cd ../..

#run the experiments
flag=$1
g++ scripts/experiment.cpp -o experiment
./experiment $flag

cd generated_plots

declare -a plots=("pso_gd" "qcqp_gd" "hailfinder_feasibility" "hepar2_feasibility"  "alarm_feasibility" "sachs_feasibility")
 
# Read the array values with space
for plot in "${plots[@]}"; do
	pdflatex ../latex_source/$plot.tex > /dev/null
	rm $plot.aux
	rm $plot.log 
done

cd ..
rm experiment
