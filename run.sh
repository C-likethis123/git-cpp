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
echo "Building the project... This will take a while to install dependencies for the first time."

# Run CMake with the selected build type
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
make

# symlink - so I can run it like gyt [arguments....]
sudo rm /usr/local/bin/gyt
sudo ln -s "$(pwd)/app/adder_app" /usr/local/bin/gyt


# Change to the "app" directory and run the application
# cd app
# ./adder_app init
