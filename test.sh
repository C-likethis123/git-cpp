#!/bin/bash
set -e

#Set the default build type to Debug if no environment is specified
BUILD_TYPE=${BUILD_TYPE:-Debug}

#If the environment is specified as "prod", set the build type to Release
if [ "$1" == "-p" ];
then
    BUILD_TYPE=Release
fi

#remove old build if any
if [ -f "app/gyt" ];
then
    rm -rf app/gyt
fi

#Print the selected build type
echo "Selected build type: $BUILD_TYPE"
echo "Building the project... This will take a while to install dependencies for the first time."

rm -rf ../tests/gitrepo/.git
cp -R ../tests/gitrepo/.notgit ../tests/gitrepo/.git

#Run CMake with the selected build type
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DBUILD_TESTS=ON -G Ninja

#Build the project
ninja

# test
ctest --output-on-failure -R catfile

#symlink - so I can run it like gyt[arguments....]
sudo rm /usr/local/bin/gyt
sudo ln -s "$(pwd)/app/gyt" /usr/local/bin/gyt
