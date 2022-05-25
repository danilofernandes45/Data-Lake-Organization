#!/bin/bash
for i in $(seq 1 30)
do
	./local_search < Metadatasets/synthetic_data_lake.txt
done
