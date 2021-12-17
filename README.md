# Readme

This repository contains my solutions to the 2021 Advent of Code challenge.
The idea is to use C to solve the problems, at least until I grow tired of it and switch to Python.

# Scripts
They must be run from the project's root folder
- `bash scripts/build.sh`:  Builds the project. Use environment variables:
  - `$BUILD_TYPE` to chose between `Release`(default) and `Debug`.
  - `$ADRESS_SANITIZER` to chose enable `ON` or disable `OFF`(default) the address sanitizer.
- `bash scripts/run.sh xx build_type`: Runs tests and solutions to a specific day (`xx` is the day padded with zeros; eg: 01, 07, 19, 23). Build type can be chosen with the second argument (default is `Release`).
- `bash scripts/test.sh`:   Runs tests for all days. Project must have been built in Release mode.
- `bash scripts/test_sanitizer.sh`:   Runs tests and solutions for all days in debug mode. Project must have been previously built in Debug mode with sanitizer enabled.

# Data
To change the data you may modify file `data/xx/data.txt` where `xx` is the zero-padded day.

Some solutions assume that there is a single newline at the end of the file, so ensure it is there.

# Advent of code
The website of the challenge can be found at https://adventofcode.com/
