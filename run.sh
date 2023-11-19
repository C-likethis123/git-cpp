set -ex

cmake . -DCMAKE_BUILD_TYPE=Release
make
cd app
./adder_app init