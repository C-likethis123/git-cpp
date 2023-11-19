#!/bin/bash
set -e

# Set the default build type to Debug if no environment is specified
BUILD_TYPE=${BUILD_TYPE:-Debug}

# If the environment is specified as "prod", set the build type to Release
if [ "$1" == "-p" ]; then
    BUILD_TYPE=Release
fi

# Print the selected build type
echo "Selected build type: $BUILD_TYPE"

# Run CMake with the selected build type
cmake . -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
make

# Change to the "app" directory and run the application
# cd app
# ./adder_app init
