#!/bin/bash
function eval() {
    for i in $(seq 1 50)
    do
        ./sa --gamma 25 --time $3 --target $4 --Ki 1 --Kf 31 --alpha 0.0094 -i ../Data/DataLakes/Socrata/$1/topic_vectors-$1-$2.txt
    done
}

eval 100 3 300 0.3615 > data_tttplot-100-3.txt
eval 100 5 300 0.4576 > data_tttplot-100-5.txt
