#!/usr/bin/env bash

set -e

sudo add-apt-repository universe
sudo apt-get update
sudo apt-get install -y libboost-serialization-dev libboost-container-dev valgrind


install_dir=$(dirname $(pwd))/external

if [ ! -d "${install_dir}" ]; then
    mkdir -p "${install_dir}"
fi

cd /tmp
rm -rf fmt
git clone https://github.com/fmtlib/fmt.git
cd fmt
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DFMT_TEST=Off
make -j4
make install
cd /tmp
rm -rf fmt
