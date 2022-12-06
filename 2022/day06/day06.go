// Package day06 solves day 6 of AoC2022.
package day06

import (
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 6
	fileName = "input.txt"
)

// Part1 solves part 1 of today's problem.
func Part1(data []byte) (int, error) {
	return solve(data, 4)
}

// Part2 solves part 2 of today's problem.
func Part2(data []byte) (int, error) {
	return solve(data, 14)
}

func solve(data []byte, n int) (int, error) {
	if len(data) < n {
		return 0, fmt.Errorf("input must be at least %d bytes", n)
	}

	window := make([]byte, n) // window contains prev+the new byte
	prev := make([]byte, n-1) // prev contains the previous bytes received.

	copy(prev[:], data[:n-1])
	for i, v := range data[n-1:] {
		// Assembling window
		window[0] = v
		copy(window[1:], prev)
		// Sorting window and finding consecutive identical pairs
		array.Sort(window, fun.Lt[byte])
		repeats := array.AdjacentReduce(window, fun.Eq[byte], fun.Count[byte])
		// If no repeats, we're done
		if repeats == 0 {
			return i + n, nil
		}
		// Update prev and continue
		prev[i%(n-1)] = v
	}
	return 0, fmt.Errorf("could not find %d consecutive unique bytes in sequence", n)
}

/// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	b, err := input.ReadDataFile(today, fileName)
	if err != nil {
		return err
	}

	result, err := Part1(b)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %v\n", result)

	result, err = Part2(b)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %v\n", result)

	return nil
}
