#!/usr/bin/env bash

set -e

sudo apt-get install -y libboost-serialization-dev libboost-container-dev valgrind

cd /tmp
rm -rf dynamic_bitset
git clone https://github.com/boostorg/dynamic_bitset.git
cd dynamic_bitset
mkdir build && cd build
cmake ..
make -j4 VERBOSE=1
sudo make install
cd /tmp
rm -rf dynamic_bitset

rm -rf googletest
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make -j4 VERBOSE=1
sudo make install
cd /tmp
rm -rf googletest

rm -rf fmt
https://github.com/fmtlib/fmt.git
cd fmt
mkdir build && cd build
cmake ..
make -j4 VERBOSE=1
sudo make install
cd /tmp
rm -rf fmt
