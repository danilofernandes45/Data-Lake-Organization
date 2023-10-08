#!/bin/bash
for i in "100" "300" "500"
do
    for j in $(seq 1 10)
    do
        ./sa --gamma 25 --time 100 --Ki 1 --Kf 31 --alpha 0.0094 -i ../Data/DataLakes/Socrata/Vectorized/$i/topic_vectors-$i-$j.txt
    done
done