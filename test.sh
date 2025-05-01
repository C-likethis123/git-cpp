#!/bin/bash
set -e

# remove old build if any
if [ -f "app/gyt" ]; then
    rm -rf app/gyt
fi

# Print the selected build type
echo "Selected build type: $BUILD_TYPE"
echo "Building the project... This will take a while to install dependencies for the first time."

# Run CMake with the selected build type
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_TESTS=ON -G Ninja

# Build the project
ninja

# Tests
ctest

# symlink - so I can run it like gyt [arguments....]
sudo rm /usr/local/bin/gyt
sudo ln -s "$(pwd)/app/gyt" /usr/local/bin/gyt
