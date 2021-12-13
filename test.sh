#!/bin/bash

# Usage:
# $ test run.sh <0-padded day>

export EXIT_CODE=0

for output in $(ls | egrep '^[0-9]{2}$')
do
	export DAY=${output}

	echo "------------------------------Testing day ${DAY}------------------------------"

	export EXECUTABLE=build/Release/${DAY}/aoc_2021_${DAY}

	./${EXECUTABLE} -t

	export LOCAL_EXIT_CODE=$?
	echo "Day ${DAY} exit code: ${LOCAL_EXIT_CODE}"
	echo

	export EXIT_CODE=$(expr $EXIT_CODE + ${LOCAL_EXIT_CODE})
done

echo "================================================================================"
echo
echo "Exiting with code ${EXIT_CODE}"
echo
exit $EXIT_CODE