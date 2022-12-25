// Package day20 solves day 20 of AoC2022.
package day20

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"strconv"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 20
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(input []int) (int64, error) {
	in := array.Map(input, func(x int) entry { return entry{value: int64(x)} })
	return solve(in, 1)
}

// Part2 solves the second half of today's problem.
func Part2(input []int) (int64, error) {
	in := array.Map(input, func(x int) entry {
		return entry{value: 811589153 * int64(x)}
	})
	return solve(in, 10)
}

// ------------ Implementation ---------------------

type entry struct {
	value   int64
	visited bool
}

func solve(data []entry, nmixes int) (int64, error) {
	for i := 0; i < nmixes; i++ {
		if i != 0 {
			array.Foreach(data, func(e *entry) { e.visited = false })
		}
		mix(data)
	}

	zero := array.FindIf(data, func(n entry) bool { return n.value == 0 })
	if zero == -1 {
		return 0, fmt.Errorf("Could not find item with value 0")
	}

	var acc int64
	for _, x := range []int{1000, 2000, 3000} {
		idx := (zero + x) % len(data)
		acc += data[idx].value
	}
	return acc, nil
}

func mix(data []entry) {
	var i int64
	N := int64(len(data))
	for i < N {
		if data[i].visited {
			i++
			continue
		}
		data[i].visited = true

		value := data[i].value
		source := int64(i)

		destination := (source + value) % (N - 1)
		if destination <= 0 {
			destination += (N - 1)
		}

		if source == destination%(N-1) {
			continue
		}

		direction := 1
		if source > destination {
			direction = -1
			source, destination = destination, source
		}
		array.Rotate(data[source:destination+1], direction)
	}
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := Part1(input)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(input)
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

// ReadData reads the data file and a list of all cubes.
func ReadData() ([]int, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	cubes := []int{}

	for sc.Scan() {
		num, err := strconv.Atoi(sc.Text())
		if err != nil {
			return nil, err
		}
		cubes = append(cubes, num)
	}

	return cubes, sc.Err()
}
