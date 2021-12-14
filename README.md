# Readme

This repository contains my solutions to the 2021 Advent of Code challenge.
The idea is to use C to solve the problems, at least until I grow tired of it and switch to Python.

# Scripts
- `bash build.sh`:  Builds the project. Use environment variable `$BUILD_TYPE` to chose between `Release`(default) and `Debug`.
- `bash run.sh xx`: Runs tests and solutions to a specific day (`xx` is the day padded with zeros; eg: 01, 07, 19, 23). Build type can be chosen the same way.
- `bash test.sh`:   Runs tests for all days. Project must have been built in Release mode.
- `bash test_valgrind.sh`:   Runs tests for all days through valgrind to spot memory issues. Project must have been built in Debug mode. Valgrind needs be installed beforehand.

# Data
To change the data you may modify file `data/xx/data.txt` where `xx` is the zero-padded day.

Some solutions assume that there is a single newline at the end of the file, so ensure it is there.

# Advent of code
The website of the challenge can be found at https://adventofcode.com/
