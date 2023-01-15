// Package day25 solves day 25 of AoC2022.
package day25

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 25
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1([]string) (string, error) {
	return "Hello, world!", nil
}

// Part2 solves the second half of today's problem.
func Part2([]string) (string, error) {
	return "Hello, world!", nil
}

// ------------ Implementation ---------------------

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem's solution.
func Main(stdout io.Writer) error {
	data, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(data)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %s\n", p1)

	p2, err := Part2(data)
	if err != nil {
		return fmt.Errorf("error in part 2: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 2: %s\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file.
func ReadData() (data []string, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	data = []string{}
	for row := 0; sc.Scan(); row++ {
		data = append(data, sc.Text())
	}

	return data, sc.Err()
}
