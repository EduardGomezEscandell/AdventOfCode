// Package day16 solves day 16 of AoC2022.
package day16

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 16
	fileName = "input.txt"
)

// Part1 solves the first half of the problem. The result is the
// sum of the size of unavailable ranges, minus the number of
// beacons in these ranges.
func Part1(map[ID]Valve) (int, error) {
	return 1, nil
}

// Part2 solves the second half of the problem. It is mostly a wrapper
// for the parallelisation, so look at part2Worker to see the logic in
// how to solve the problem.
func Part2(map[ID]Valve) (int, error) {
	return 1, nil
}

// ------------- Implementation ------------------.

type ID uint

type Valve struct {
	flowrate int
	paths    []ID
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	world, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(world)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(world)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and returns a map of the valves.
func ReadData() (map[ID]Valve, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	world := map[ID]Valve{}
	for sc.Scan() {

	}

	if err := sc.Err(); err != nil {
		return nil, err
	}

	return world, nil
}
