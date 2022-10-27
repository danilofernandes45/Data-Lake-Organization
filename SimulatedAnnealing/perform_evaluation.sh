#!/bin/bash
function eval() {
	for i in $(seq 1 15)
	do
		./sa --gamma 25 --time $2 --Ki 78 --Kf 4 --alpha 0.0045 -i ../Data/DataLakes/Socrata/$1/topic_vectors-$1-$3.txt
	done
}

for i in $(seq 1 10)
do
	eval $1 $2 $i > ../Data/Performance/Socrata/SA/$1/perform-$1-$i.csv
done