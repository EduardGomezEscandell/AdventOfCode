// Package day00 solves problem 1 of the previous year. It is a sample problem.
package day00

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
	today    = 0
	fileName = "input.txt"
)

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	inp, err := parseInput()
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

// Part1 solves the first half of the problem.
func Part1(inp []uint) (uint, error) {
	return array.AdjacentReduce(inp, fun.Lt[uint], fun.Count[uint]), nil
}

// Part2 solves the second half of the problem.
func Part2(inp []uint) (uint, error) {
	if len(inp) < 3 {
		return 0, nil
	}

	count := uint(0)
	curr := array.Reduce(inp[0:3], fun.Add[uint])
	for i := 3; i < len(inp); i++ {
		prev := curr
		curr = prev - inp[i-3] + inp[i]
		if curr > prev {
			count++
		}
	}

	return count, nil
}

func parseInput() (data []uint, err error) {
	b, err := input.ReadDataFile(today, fileName)
	if err != nil {
		return data, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	for sc.Scan() {
		p, err := strconv.ParseUint(sc.Text(), 10, 64)
		if err != nil {
			return data, err
		}
		if p >= math.MaxUint {
			return data, fmt.Errorf("Narrowing conversion: %d does not fit in a uint", p)
		}
		data = append(data, uint(p))
	}

	return data, sc.Err()
}
