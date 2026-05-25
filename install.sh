set -euxo pipefail

cmake -B build .
cmake --build build
sudo cmake --install build
krunner --replace