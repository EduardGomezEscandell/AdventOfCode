#!/bin/bash

# Usage:
# $ bash build.sh <project_dir>

export PROJECT_DIR=${1:-test}
export BUILD_TYPE=${BUILD_TYPE:-Release}

cd $PROJECT_DIR
export AOC_PROJECT_NAME=aoc_2021_${PWD##*/}

echo "Project: $AOC_PROJECT_NAME"
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
