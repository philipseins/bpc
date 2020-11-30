#!/bin/bash

BASE_DIR="/home/suchao/bpc/traces/"
# echo $BASE_DIR
traces=$(ls $BASE_DIR)
for trace in $traces
do
    if [[ -f $BASE_DIR$trace ]]; then
	echo $trace >> result.ref
        ./predictor $BASE_DIR$trace >> result.ref
    fi
done
