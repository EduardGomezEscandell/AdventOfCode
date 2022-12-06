package day06

import (
	"errors"
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
	if len(data) < 4 {
		return 0, errors.New("input must be at least 4 bytes")
	}

	prev := [3]byte{} // prev contains the previous three bytes recieved.
	copy(prev[:], data[:3])

	for i, v := range data[3:] {
		window := [4]byte{v, prev[0], prev[1], prev[2]}
		array.Sort(window[:], fun.Lt[byte])
		repeats := array.AdjacentReduce(window[:], fun.Eq[byte], fun.Count[byte])
		if repeats == 0 {
			return i + 4, nil
		}
		prev[i%3] = v
	}
	return 0, errors.New("could not find four consecutive unique bytes in sequence")
}

// modulo computes the positive modulo, as oposed to Go's remainder,
// which is negative for negative x.
func modulo(x, n int) int {
	return ((x % n) + n) % 3
}

func Part2(b []byte) (int, error) {
	return 0, nil
}

/// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	b, err := ReadDataFile()
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

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}
