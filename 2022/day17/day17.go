// Package day17 solves day 17 of AoC2022.
package day17

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 17
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1() (int, error) {
	return 1, nil
}

// Part2 solves the second half of the problem.
func Part2() (int, error) {
	return 1, nil
}

// ------------- Implementation ------------------.

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	_, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1()
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2()
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
func ReadData() (int, error) {
	b, err := ReadDataFile()
	if err != nil {
		return 0, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	type protovalve struct {
		name     string
		flowrate int
		paths    []string
	}

	for sc.Scan() {

	}

	if err := sc.Err(); err != nil {
		return 0, err
	}

	return 1, nil
}
