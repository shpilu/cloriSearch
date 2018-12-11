#!/bin/bash

cd `dirname $0`
dir=$(pwd)

ulimit -c unlimited
${dir}/tutorial
