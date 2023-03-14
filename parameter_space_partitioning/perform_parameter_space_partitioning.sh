#!/bin/bash

#read command line argument that indicates whether the experiments need to be rerun (flag=true) or not (flag=false)
flag=$1

#create 2D plot
python3 scripts/create_2D_graph.py $flag
cd generated_files
pdflatex --enable-write18 --extra-mem-top=100000000 --synctex=1 ../alarm-red-green-plots/2D/alarm_pla_graph_2D.tex 
rm alarm_pla_graph_2D.aux
rm alarm_pla_graph_2D.log
rm alarm_pla_graph_2D.synctex.gz
cd ..

#create 3D plot
python3 scripts/create_3D_graph.py $flag
cd generated_files
pdflatex --enable-write18 --extra-mem-top=100000000 --synctex=1 ../alarm-red-green-plots/3D/alarm_pla_graph_3D.tex 
rm alarm_pla_graph_3D.aux
rm alarm_pla_graph_3D.log
rm alarm_pla_graph_3D.synctex.gz
cd ..

#run experiments for the networks 'win95pts', 'hailfinder', 'hepar2'
python3 scripts/run_pla_experiments.py $flag
cd generated_files
pdflatex ../scripts/pla_plot.tex 
rm pla_plot.aux
rm pla_plot.log
cd ..

#print PLA table
python3 scripts/make_pla_table.py
