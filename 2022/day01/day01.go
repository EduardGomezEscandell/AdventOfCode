// Package day 01 solves 1.
package day01

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"math"
	"sort"
	"strconv"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 1
	fileName = "input.txt"
)

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

// Part1 solves the first half of the problem.
func Part1(input [][]uint) (uint, error) {
	if len(input) < 1 {
		return 0, nil
	}

	var max uint
	for _, reindeer := range input {
		cals := array.Reduce(reindeer, fun.Add[uint])
		if cals > max {
			max = cals
		}
	}

	return max, nil
}

// Part2 solves the second half of the problem.
func Part2(input [][]uint) (uint, error) {
	arraySum := func(v []uint) uint {
		return array.Reduce(v, fun.Add[uint])
	}

	if len(input) < 4 {
		return array.ScanReduce(input, arraySum, fun.Add[uint]), nil
	}

	best3 := array.Scan(input[:3], arraySum)
	sort.Slice(best3, func(i, j int) bool {
		return best3[i] < best3[j]
	})

	for _, reindeer := range input[3:] {
		cals := arraySum(reindeer)
		if cals > best3[0] {
			best3[0] = cals
		}
		sort.Slice(best3, func(i, j int) bool {
			return best3[i] < best3[j]
		})
	}

	return arraySum(best3), nil
}

var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

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
