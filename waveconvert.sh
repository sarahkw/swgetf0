#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 rawfile wavefile"
    exit 1
fi

RAWFILE=$1
WAVEFILE=$2

sox -c 1 -b 16 -e signed-integer -r 96000 -t raw $RAWFILE -r 44100 $WAVEFILE
