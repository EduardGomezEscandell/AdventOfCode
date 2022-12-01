#!/bin/bash -e

export AOC_2022_ROOT=${AOC_2022_ROOT:-`pwd`}
export COVERAGE_DIR="${AOC_2022_ROOT:-AOC_2022_ROOT}/coverage"

mkdir -p "${COVERAGE_DIR}"
go test -coverpkg=./... -coverprofile="${COVERAGE_DIR}/coverage.out" -covermode=set ./... > /dev/null


go tool cover -func="${COVERAGE_DIR}/coverage.out" | tee "$COVERAGE_DIR/coverage.txt"

# Opening in browser when not on Gitub runner
if [[ -z "${GITHUB_WORKSPACE+x}" ]] ; then
    go tool cover -html="${COVERAGE_DIR}/coverage.out" > "$COVERAGE_DIR/coverage.html"
fi