#!/bin/bash

set -euxo pipefail

cmake -B build .
cmake --build build
sudo cmake --install build

# kquitapp6 krunner
krunner --replace