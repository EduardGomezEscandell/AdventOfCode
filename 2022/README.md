# 2022

## How to run
You'll need `go` installed:
```bash
sudo apt-get -y update
sudo apt-get -y install golang-go 
```
It should also work on other OS's but it is only tested on Ubuntu 🤷‍♂️.
To run, go to `2022/` and run `go run main.go --day=13` for example. To run it from elsewhere you'll
need to set evironment variable `AOC_2022_ROOT` to the 2022 dir.
```bash
/path/to/repo/2022$ go run main.go --day=13
```
Use `--help` for a list of options.

## Running tests
Go to `2022/` and run 
```
AOC_2022_ROOT=`pwd` go test ./... --race
```
or do so with coverage included:
```bash
AOC_2022_ROOT=`pwd` /path/to/repo/2022$ bash coverage.sh
```
