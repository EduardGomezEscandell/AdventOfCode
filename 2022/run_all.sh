#!/bin/bash
set -eu

executable="/tmp/aoc2022"

# Building
go build -o "${executable}"

# Executing every day
ndays=$(${executable} --count-days)

day=0
while [[ day -lt ndays ]]; do
    echo "::group::Day ${day}"

    ${executable} --day="${day}"
    ((++day))

    echo
    echo "::endgroup::"    
done