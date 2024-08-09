#!/bin/bash

cd build
cmake ..
make
./hpsu_tests
cd ..