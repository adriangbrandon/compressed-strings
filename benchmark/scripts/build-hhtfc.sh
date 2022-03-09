#!/bin/bash

BASE_DIR=$HOME/compressed-strings/benchmark
BUILD_EXEC=$HOME/compressed-strings/build/Release/bin/build
dict_sizes=(1 2 4 8.6) # in gigabytes
bucket_sizes=(02 04 08 16 32)

for gb in ${dict_sizes[@]}; do
	if [ ! -d ${BASE_DIR}/data/hhtfc/${gb}gb ]; then
		mkdir -p ${BASE_DIR}/data/hhtfc/${gb}gb
	fi
done

for gb in ${dict_sizes[@]}; do
	for bs in ${bucket_sizes[@]}; do
		(set -x; $BUILD_EXEC 4 h ${bs} ${BASE_DIR}/data/${gb}gb.dat ${BASE_DIR}/data/hhtfc/${gb}gb/${bs}) &
	done
done

wait
echo "Build done"
