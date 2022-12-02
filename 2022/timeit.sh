#!/bin/bash
set -e
executable=${executable:-"go run main.go"}

ndays=$(${executable} --count-days)
day=0
while [[ day -lt ndays ]]; do
    time ( ${executable} --day="${day}" 2>&1) 2> /tmp/time
    cat /tmp/time | grep "real" | sed 's/real\s\(.*\)/time: \1/'
    echo
    ((++day))
done