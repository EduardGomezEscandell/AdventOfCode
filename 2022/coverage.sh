#!/bin/bash -e

export AOC_2022_ROOT=`pwd`
export COVERAGE_DIR="${AOC_2022_ROOT}/coverage"

mkdir -p "${COVERAGE_DIR}"
go test -coverpkg=./... -coverprofile="${COVERAGE_DIR}/coverage.out" -covermode=set ./... > /dev/null


if [[ -z "${GITHUB_WORKSPACE}" ]] ; then
    go tool cover -html="${COVERAGE_DIR}/coverage.out"
else
    go tool cover -func="${COVERAGE_DIR}/coverage.out"
fi