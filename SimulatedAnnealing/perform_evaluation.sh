#!/bin/bash
function eval() {
	for i in $(seq 1 15)
	do
		./sa --gamma 25 --time 300 --Kr 7 --Ki 92 --Kf 35 --alpha 0.009 -i ../Data/DataLakes/Socrata/$1/topic_vectors-$1-$2.txt
	done
}

for i in $(seq 1 10)
do
	eval $1 $i > ../Data/Performance/Socrata/SA/$1/perform-$1-$i.csv
done