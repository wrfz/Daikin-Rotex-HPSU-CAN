#!/bin/bash

apk add cmake make gcc build-base clang llvm-static llvm-dev clang-static clang-dev
if [ $? -eq 0 ]; then
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
fi