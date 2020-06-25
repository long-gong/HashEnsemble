#!/usr/bin/env bash

set -e

sudo apt-get install -y libboost-serialization-dev libboost-container-dev valgrind

rm -rf googletest
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make -j4
sudo make install
cd /tmp
rm -rf googletest

rm -rf fmt
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir build && cd build
cmake ..
make -j4
sudo make install
cd /tmp
rm -rf fmt
