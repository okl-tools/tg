#!/bin/bash

cd ${0%/*}

mkdir build
cd build
cmake ..
make -j 8
