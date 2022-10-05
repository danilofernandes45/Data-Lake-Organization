#!/bin/bash
for i in $(seq 1 15)
do
	./local_search < ../Data/Metadatasets/Socrata/100/topic_vectors-100-3.txt
done
