// Package day11 solves day 11 of AoC2022.
package day11

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 11
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in string) (int, error) {
	return 0, nil
}

// Part2 solves the second half of the problem.
func Part2(in string) (int, error) {
	return 0, nil
}

// ------------- Implementation ------------------

// ------------- Here be boilerplate ------------------
/// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	inp, err := ParseInput()
	if err != nil {
		return err
	}

	result, err := Part1(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %v\n", result)

	result, err = Part2(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %v\n", result)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ParseInput reads the data file and returns the list of
// reindeers and their calories.
func ParseInput() (data string, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return data, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	for sc.Scan() {
		// line := sc.Text()
	}

	if err := sc.Err(); err != nil {
		return data, err
	}

	if len(data) == 0 {
		return "", nil
	}

	return data, nil
}
