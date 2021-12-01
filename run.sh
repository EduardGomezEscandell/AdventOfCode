#!/bin/bash

# Usage:
# $ bash run.sh <day> <build_type>

export DAY=${1:-test}
export BUILD_TYPE=${2:-Release}

export EXECUTABLE=build/${BUILD_TYPE}/${DAY}/aoc_2021_${DAY}

./${EXECUTABLE}
