#!/bin/bash
function eval() {
	for i in $(seq 1 15)
	do
		./sa --gamma 25 --time $2 --target 1.0 --Ki 1 --Kf 31 --alpha 0.0094 -i ../Data/DataLakes/Socrata/Vectorized/$1/topic_vectors-$1-$3.txt
	done
}

for i in $(seq 1 1)
do
	# eval $1 $2 $i > ../Data/Performance/Socrata/SA/$1/perform-$1-$i-2.csv
	eval $1 $2 $i > ../Data/Performance/Socrata/ILS/$1/perform-$1-$i-2.csv
done