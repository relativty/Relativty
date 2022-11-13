#!/bin/bash

mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd -

cp build/librelativty.so Relativty/bin/linux64/driver_relativty.so
