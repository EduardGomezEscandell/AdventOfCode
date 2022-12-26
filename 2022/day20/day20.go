// Package day20 solves day 20 of AoC2022.
package day20

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 20
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(input []int) (int64, error) {
	var i int
	in := array.Map(input, func(x int) entry {
		defer func() { i++ }()
		return entry{value: int64(x), position: i}
	})
	return solve(in, 1)
}

// Part2 solves the second half of today's problem.
func Part2(input []int) (int64, error) {
	var i int
	in := array.Map(input, func(x int) entry {
		defer func() { i++ }()
		return entry{value: 811589153 * int64(x), position: i}
	})
	return solve(in, 10)
}

// ------------ Implementation ---------------------

type entry struct {
	value    int64
	position int
}

func solve(data []entry, nmixes int) (int64, error) {
	for i := 0; i < nmixes; i++ {
		mix(data)
		// fmt.Println(pretty(data))
	}
	array.Sort(data, func(a, b entry) bool { return a.position < b.position })

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

// mix performs the mixing operation as explained in the problem statement.
func mix(data []entry) {
	N := len(data)
	for i := 0; i < N; i++ {
		first := data[i].position
		last := int(positiveMod(data[i].value+int64(data[i].position), int64(N-1)))

		// No changes to array: continuing
		if first%(N-1) == last%(N-1) {
			continue
		}

		// Rotating. Instead of actually performing the rotation,
		// we only change the 'position' field. The array stays
		// sorted in the original order.
		direction := -1 // Rotate left
		if first > last {
			direction = +1 // Rotate right
			first, last = last, first
		}
		array.Foreach(data, func(e *entry) {
			if first <= e.position && e.position <= last {
				e.position += direction
			}
		})
		data[i].position = last
		if direction == 1 {
			data[i].position = first
		}
	}
}

func positiveMod(x int64, n int64) int64 {
	v := x % n
	if v <= 0 {
		v += n
	}
	return v
}

func pretty(data []entry) string { // nolint: unused
	d := array.Map(data, fun.Identity[entry])
	array.Sort(d, func(a, b entry) bool { return a.position < b.position })
	return strings.Join(array.Map(d, func(e entry) string {
		return fmt.Sprintf("%d: %d", e.position, e.value)
	}), ", ")
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
