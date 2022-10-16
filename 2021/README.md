# Readme

This repository contains my solutions to the 2021 Advent of Code challenge.

This challenge consists of daily problems of varying difficulty released every morning from Dec 1st to Dec 25th. The late problems are generally more complicated than the first few. The organizer provides the instructions and an input file and the solution is validated by introducing the program output in the website.

The website of the challenge can be found at https://adventofcode.com/2021

## Status

All problems have been solved and as such this repository will not be modified anymore.

## Scripts
They must be run from the project's root folder
- `bash scripts/build.sh`:  Builds the project. Use environment variables:
  - `$BUILD_TYPE` to chose between `Release`(default) and `Debug`.
  - `$ADRESS_SANITIZER` to chose enable `ON` or disable `OFF`(default) the address sanitizer.
  - `$THREAD_SANITIZER` to chose enable `ON` or disable `OFF`(default) the thread sanitizer. This option is incompatible with the previous one.
- `bash scripts/run.sh xx build_type`: Runs tests and solutions for a specific day (`xx` is the day padded with zeros; eg: 01, 07, 19, 23). Build type can be chosen with the second argument (default is `Release`).
- `bash scripts/test.sh`:   Runs tests for all days. Project must have been built in Release mode.
- `bash scripts/test_sanitizer.sh`:   Runs tests and solutions for all days in debug mode. Project must have been previously built in Debug mode with sanitizer enabled.

## Data input
To change the problem input data you may modify file `data/xx/data.txt` where `xx` is the zero-padded day. All of them should work for any valid input, except for day 24, in which the input is hard-coded in.

Some solutions assume that there is a newline at the end of the file, some others don't care.
Just write it always to ensure it works.
