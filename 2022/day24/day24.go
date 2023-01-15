// Package day24 solves day 24 of AoC2022.
package day24

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 24
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1() (int, error) {
	return 1, nil
}

// Part2 solves the second half of today's problem.
func Part2() (int, error) {
	return 1, nil
}

// ------------ Implementation ---------------------

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem's solution.
func Main(stdout io.Writer) error {
	_, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1()
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2()
	if err != nil {
		return fmt.Errorf("error in part 2: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file.
func ReadData() (data []int, err error) { // nolint: revive
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	for row := 0; sc.Scan(); row++ {

	}

	return nil, sc.Err()
}
