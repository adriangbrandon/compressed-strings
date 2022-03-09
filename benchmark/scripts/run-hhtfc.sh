#!/bin/bash

BASE_DIR=$HOME/compressed-strings/benchmark
TEST_EXEC=$HOME/compressed-strings/build/Release/bin/test-single
dict_sizes=(1 2 4 8.6) # in gigabytes
bucket_sizes=(02 04 08 16 32)

for gb in ${dict_sizes[@]}; do
	mkdir -p ${BASE_DIR}/results/hhtfc/${gb}gb
done

for gb in ${dict_sizes[@]}; do
	echo "Running tests for ${gb} GB dictionary"
	for bs in ${bucket_sizes[@]}; do
		echo "Bucketsize = ${bs}"
		$TEST_EXEC ${BASE_DIR}/data/hhtfc/${gb}gb/${bs}.hhtfc ${BASE_DIR}/data/${gb}gb.dat > ${BASE_DIR}/results/hhtfc/${gb}gb/${bs}.txt
	done
done
echo "Tests done"
