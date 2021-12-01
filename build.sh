#!/bin/bash

# Usage:
# $ bash build.sh

export PROJECT_DIR=$(pwd)
export BUILD_TYPE=${BUILD_TYPE:-Release}

echo "Build type: $BUILD_TYPE"
echo

export BUILD_DIR=$PROJECT_DIR/build/
export SOURCE_DIR=$PROJECT_DIR


mkdir $BUILD_DIR 2> /dev/null
cd $BUILD_DIR

mkdir $BUILD_TYPE 2> /dev/null
cd $BUILD_TYPE

cmake $SOURCE_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DAOC_PROJECT_NAME=$AOC_PROJECT_NAME
cmake --build . -- -j $(nproc)

cd $PROJECT_DIR
mv $BUILD_DIR/$BUILD_TYPE/compile_commands.json .