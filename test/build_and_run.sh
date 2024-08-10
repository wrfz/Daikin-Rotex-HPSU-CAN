#!/bin/bash

mkdir -p build
if [ $? -eq 0 ]; then
    cd build
    cmake ..
    if [ $? -eq 0 ]; then
        make
        if [ $? -eq 0 ]; then
            ./hpsu_tests
        fi
    fi
    cd ..
fi