#!/bin/bash

# install AVR gcc toolchain via homebrew
brew tap osx-cross/avr
brew install avr-gcc

# install simavr via homebrew
brew install --HEAD simavr

# build bazel from source
URL="https://github.com/bazelbuild/bazel/releases/download/3.1.0/bazel-3.1.0-installer-darwin-x86_64.sh"
wget -O install.sh $URL
chmod +x install.sh
./install.sh --user
rm -f install.sh
