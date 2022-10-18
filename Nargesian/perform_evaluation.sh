#!/bin/bash
function eval() {
	for i in $(seq 1 15)
	do
		./local_search --gamma 25 --time 300 --Kr 1 --Kp 50 --eps 0.01 -i ../Data/DataLakes/Socrata/$1/topic_vectors-$1-$2.txt
	done
}

for i in $(seq 1 10)
do
	eval $1 $i > ../Data/Performance/Socrata/Nargesian/$1/perform-$1-$i.csv
done
