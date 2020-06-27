#!/usr/bin/env bash

set -e

# check whether boost was installed, if not, install it
brew list boost || brew install boost


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
