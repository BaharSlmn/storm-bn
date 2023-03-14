#!/bin/bash

network=$1
number=$2
total=$3
nodes=(${@:4})
nodesstr=$(IFS=,)
output=$network
mkdir -p $output
prop="\""
for node in ${nodes[@]} do
   prop=$prop$node"1\"|\""
done
prop=${prop::-2}
echo $network
echo "P=? [F "$prop"]"
storm-pars --explicit-drn  $output/$network\_$number.drn --prop "P=? [F "$prop"]" --bisimulation > $output/$network-$number-out.txt
