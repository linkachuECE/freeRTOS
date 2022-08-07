#!/bin/bash

make clean
make

if [[ $1 == "run" ]]; then
    ./build/posix_demo
elif [[ $1 == "debug" ]]; then
    gdb ./build/posix_demo
fi