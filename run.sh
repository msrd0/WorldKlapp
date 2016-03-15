#!/bin/bash -e

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j7
cd ../httpd
LD_LIBRARY_PATH=../build ../build/klapp-httpd
