#!/bin/bash

TRAVIS_ROOT=`pwd`

# build simavr from source
sudo apt-get update
sudo apt-get install git gcc-avr binutils-avr avr-libc libelf-dev
git clone https://github.com/buserror/simavr.git
cd simavr
sudo chmod -R a+w /usr/local
make install-simavr
cd $TRAVIS_ROOT

# build bazel from source
URL="https://github.com/bazelbuild/bazel/releases/download/3.1.0/bazel-3.1.0-installer-linux-x86_64.sh"
wget -O install.sh $URL
chmod +x install.sh
./install.sh --user
rm -f install.sh
