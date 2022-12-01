// Package day01 solves day 1 of AoC2022	.
package day01

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"math"
	"strconv"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 1
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(input [][]uint) (uint, error) {
	caloryCounts := array.Map(input, accumulate)
	return array.Best(caloryCounts, fun.Gt[uint]), nil
}

// Part2 solves the second half of the problem.
func Part2(input [][]uint) (uint, error) {
	caloryCounts := array.Map(input, accumulate)
	top3 := array.BestN(caloryCounts, 3, fun.Gt[uint])
	return accumulate(top3), nil
}

// accumulate takes a slice and returns the sum of its members.
func accumulate(arr []uint) uint {
	return array.Reduce(arr, fun.Add[uint])
}

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
func ParseInput() (data [][]uint, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return data, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	data = append(data, []uint{})
	for sc.Scan() {
		line := sc.Text()

		// Parsing blank lines
		if len(line) == 0 {
			data = append(data, []uint{})
			continue
		}

		// Parsing integers
		p, err := strconv.ParseUint(line, 10, 64)
		if err != nil {
			return data, err
		}
		if p >= math.MaxUint {
			return data, fmt.Errorf("Narrowing conversion: %d does not fit in a uint", p)
		}
		idx := len(data) - 1
		data[idx] = append(data[idx], uint(p))
	}

	if err := sc.Err(); err != nil {
		return data, err
	}

	if len(data) == 1 && len(data[0]) == 0 {
		return [][]uint{}, nil
	}

	return data, nil
}
