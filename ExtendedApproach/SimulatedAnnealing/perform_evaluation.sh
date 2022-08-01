#!/bin/bash
for i in $(seq 1 30)
do
	./local_search < ../../Data/Metadatasets/tweets_data_lake.txt
done
