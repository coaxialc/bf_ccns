#!/bin/bash

function experiment
{
	mkdir ./pit_stats ./data_stats
	group_size=1
	seed=80
	
	while true; do
        
	  ./waf --run " ${1} $group_size $seed "
	
	  if [ $? = 1 ]; then
	    break
	  fi
	
	group_size=$[group_size+1]
	seed=$[seed+1]
	done	
	
	python parse.py
	gnuplot plotInstructions -persist
}

function move
{
	mkdir $1
	mv data_stats $1
	mv "./Group size - Average pit size.png" $1
	mv "./Group size - Data number.png" $1
	mv "./Group size - Interest number.png" $1
	mv gs_avgpit.txt $1
	mv gs_data.txt $1
	mv gs_interests.txt $1
	mv pit_stats $1
	mv results.txt $1
}

rm -rf results.txt ./1221-results ./1755-results ./3257-results ./3967-results ./6461-results

declare -a topology=(1221 1755 3257 3967 6461)

for i in ${topology[@]} 
do
    experiment "scratch/app2 $i.txt"
    move "./$i-results/"  
done