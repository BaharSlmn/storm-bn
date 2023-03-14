#!/bin/bash

flag=$1
#run the experiments using storm and parse the results
python3 scripts/run_experiments.py $flag

#create the plots
cd generated_files/
declare -a plots=("inference_time" "compile_time"  "mtbdd_nodes")
for plot in "${plots[@]}"; do
	pdflatex ../tex_files/$plot.tex
	rm $plot.log
	rm $plot.aux
done
