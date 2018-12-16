#!/bin/bash

cd `dirname $0`
dir=$(pwd)

ulimit -c unlimited

if [ $# -lt 1 ]
then
    ${dir}/tutorial_simple
elif [ "$1" == "simple" ]
then
    ${dir}/tutorial_simple
elif [ "$1" == "interval" ]
then
    ${dir}/tutorial_interval
else 
    echo "unknown command, just 'simple/interval/geo' is supported"
fi

