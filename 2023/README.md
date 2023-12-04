# 2023

## How to build
Install CMake, Git, GCC (≥13), and G++(≥13). If you're using the latest version of Ubuntu, you can use
```bash
./dependecies
```

Once the dependencies are installed, build with:
```bash
./make
```

The following environment variables control the build:
- BUILD_TYPE (default: Release): The build type
- ENABLE_SANITIZER (default: false): Wether to install sanitizers or not.
- BUILD_TESTS (default: false): Whether to build the tests or not.
- C (default: gcc-13): The executable for your C compiler
- CXX (default: g++-13): The executable for your C++ compiler

## How to run
To see options, use:

```
$ ./build/Release/cmd/aoc2023 --help
Usage:

aoc2023 -h
aoc2023 --help
    Shows this message and exits

aoc2023 -r
aoc2023 --run
    Run the solutions for the specified days

aoc2023 -a
aoc2023 --all
    Run all the solutions

aoc2023 -t
aoc2023 --time
    Run all the solutions many times to get an accurate profile
```

To run the tests, use:
```bash
./build/Release/test/test
```