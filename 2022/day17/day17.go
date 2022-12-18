// Package day17 solves day 17 of AoC2022.
package day17

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 17
	fileName = "input.txt"
)

type cell uint8

const (
	air   cell = 0
	earth cell = 1
)

type rock = [][]cell

func rocks() []rock {
	const (
		o = 0
		X = 1
	)
	return []rock{
		[][]cell{{X, X, X, X}},
		[][]cell{
			{o, X, o},
			{X, X, X},
			{o, X, o},
		},
		[][]cell{
			{X, X, X},
			{o, o, X},
			{o, o, X},
		},
		[][]cell{{X}, {X}, {X}, {X}},
		[][]cell{
			{X, X},
			{X, X},
		},
	}
}

type time uint

// part1 solves the first half of the problem.
func part1(winds []int8) (int, error) {
	return Solve(winds, 2022)
}

// Solve solves the problem.
func Solve(winds []int8, nrocks int) (int, error) {
	wind := func(wtime time) int {
		return int(winds[wtime%time(len(winds))])
	}
	rocks := rocks()

	rockStack := array.MapReduce(rocks, func(r rock) int { return len(r) }, fun.Add[int], 0)
	nstacks := nrocks / len(rocks)
	height := rockStack*(nstacks+1) + 3 // Worse case scenario

	const width = 7
	world := array.Generate2D(height, width, func() cell { return air })

	// Drop it like it's hot
	var maxY int
	var windTime time
	for r := 0; r < nrocks; r++ {
		y := maxY + 3
		x := 2
		rock := rocks[r%len(rocks)]
		for {
			w := wind(windTime)
			windTime++
			if !overlap(world, rock, x+w, y) {
				x += w
			}
			if !overlap(world, rock, x, y-1) {
				y--
				continue
			}
			break
		}

		e := ovewrite(world, rock, x, y)
		if e != nil {
			return 0, e
		}
		maxY = fun.Max(maxY, y+len(rock))
	}

	return maxY, nil // We subtract the floor we added
}

func prettyPrint(world [][]cell, r rock, X, Y int) string {
	// Draing world
	N := Y + len(r)

	s := make([][]byte, N)
	for y := 0; y < N; y++ {
		s[y] = array.Map(world[y], func(c cell) byte {
			switch c {
			case air:
				return '.'
			case earth:
				return '#'
			}
			return '?'
		})
	}

	// Overwriting rock
	for i := 0; Y+i < len(world) && i < len(r); i++ {
		for j := 0; X+j < len(world[0]) && j < len(r[0]); j++ {
			if v := r[i][j]; v != air {
				s[Y+i][X+j] = '@'
			}
		}
	}

	floor := array.Generate(len(world[0]), func() byte { return '=' })
	s = append(array.Reverse(s), floor)

	return strings.Join(array.Map(s, func(b []byte) string { return string(b) }), "\n") + "\n"
}

func overlap(world [][]cell, rock [][]cell, X, Y int) bool {
	if Y < 0 || Y+len(rock) > len(world) {
		return true
	}
	if X < 0 || X+len(rock[0]) > len(world[0]) {
		return true
	}

	var o int
	i := Y
	for _, krow := range rock {
		j := X
		for _, k := range krow {
			o += int(world[i][j] * k)
			j++
		}
		i++
	}
	return o != 0
}

func ovewrite[T generics.Number](matrix [][]T, kernel [][]T, X, Y int) error {
	if Y < 0 || Y+len(kernel) > len(matrix) {
		return fmt.Errorf("cannot write kernel into location i=%d. Want 0 <= i <= i+%d <= %d", Y, len(kernel), len(matrix))
	}
	if X < 0 || X+len(kernel[0]) > len(matrix[0]) {
		return fmt.Errorf("cannot write kernel into location j=%d. Want <= j <= j+%d <= %d", X, len(kernel[0]), len(matrix[0]))
	}

	i := Y
	for _, krow := range kernel {
		j := X
		for _, k := range krow {
			matrix[i][j] = fun.Max(k, matrix[i][j])
			j++
		}
		i++
	}
	return nil
}

// Part2 solves the second half of the problem.
func Part2(wind []int8) (int, error) {
	return 1, nil
}

// ------------- Implementation ------------------.

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	wind, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := part1(wind)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(wind)
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

// ReadData reads the data file and returns a map of the valves.
func ReadData() ([]int8, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	var input []int8
	if sc.Scan() {
		var err error
		input = array.Map([]byte(sc.Text()), func(r byte) int8 {
			switch r {
			case '<':
				return -1
			case '>':
				return 1
			}
			if err != nil {
				err = fmt.Errorf("unexpected character: %c (0x%x)", r, r)
			}
			return 0
		})

		if err != nil {
			return nil, err
		}
	} else {
		return nil, errors.New("empty input")
	}

	if sc.Scan() {
		return nil, errors.New("unexpected second line in input")
	}

	return input, sc.Err()
}
