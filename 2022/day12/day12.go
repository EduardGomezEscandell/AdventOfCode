// Package day12 solves day 12 of AoC2022.
package day12

import (
	"bufio"
	"bytes"
	"container/heap"
	"math"

	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/myheap"
)

const (
	today    = 12
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(world [][]uint8) (int, error) {
	if len(world) == 0 || len(world[0]) == 0 {
		return 0, errors.New("empty world")
	}

	cost := array.Map(world, func(row []uint8) []int { return array.Map(row, func(uint8) int { return math.MaxInt }) })

	best := findCell(world, 'S')
	end := findCell(world, 'E')

	world[best.i][best.j] = 'a'
	world[end.i][end.j] = 'z'

	candidates := myheap.New(func(a, b Idx) bool { return cost[a.i][a.j] < cost[b.i][b.j] })

	cost[best.i][best.j] = 0

	visit(world, cost, candidates, best)

	for candidates.Len() != 0 {
		best = heap.Pop(candidates).(Idx)
		if best == end {
			return cost[best.i][best.j], nil
		}
		visit(world, cost, candidates, best)
	}
	return 0, errors.New("failed to find a path to the end")
}

func visit(world [][]uint8, cost [][]int, candidates *myheap.Heap[Idx], idx Idx) {
	if idx.i > 0 {
		to := Idx{idx.i - 1, idx.j}
		consider(world, cost, candidates, idx, to)
	}

	if idx.i < len(world)-1 {
		to := Idx{idx.i + 1, idx.j}
		consider(world, cost, candidates, idx, to)
	}

	if idx.j > 0 {
		to := Idx{idx.i, idx.j - 1}
		consider(world, cost, candidates, idx, to)
	}

	if idx.j < len(world[0])-1 {
		to := Idx{idx.i, idx.j + 1}
		consider(world, cost, candidates, idx, to)
	}
}

func consider(world [][]uint8, cost [][]int, candidates *myheap.Heap[Idx], from, to Idx) {
	climb := int(world[to.i][to.j]) - int(world[from.i][from.j])
	if climb > 1 {
		return
	}
	c := cost[from.i][from.j] + 1
	// This only works because all edge costs are the same!
	if c < cost[to.i][to.j] {
		heap.Push(candidates, to)
		cost[to.i][to.j] = c
	}
}

type Idx struct {
	i, j int
}

func findCell(world [][]uint8, w uint8) Idx {
	for r, row := range world {
		for c, cell := range row {
			if cell != w {
				continue
			}
			return Idx{r, c}
		}
	}
	return Idx{-1, -1}
}

// Part2 solves the second half of the problem.
func Part2(world [][]uint8) (int, error) {
	return 0, nil
}

// ---------- Implementation ------------------
// ---------- Here be boilerplate ------------------

type problemResult struct {
	id  int
	res string
	err error
}

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	resultCh := make(chan problemResult)
	go func() {
		input, err := ParseInput()
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part1(input)
		if err != nil {
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %d", result), nil}
	}()

	go func() {
		input, err := ParseInput()
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part2(input)
		if err != nil {
			resultCh <- problemResult{1, "", err}
		}
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2: %d", result), nil}
	}()

	var results [2]problemResult
	for i := 0; i < 2; i++ {
		r := <-resultCh
		results[r.id] = r
	}

	for _, v := range results {
		if v.err != nil {
			return v.err
		}
		fmt.Fprintln(stdout, v.res)
	}

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ParseInput reads the data file and returns the list of
// reindeers and their calories.
func ParseInput() ([][]uint8, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	world := [][]uint8{}
	for i := 0; sc.Scan(); i++ {
		world = append(world, []uint8(sc.Text()))
	}

	if err := sc.Err(); err != nil {
		return nil, err
	}

	return world, nil
}

func nextLine(s *bufio.Scanner) (string, error) {
	read := s.Scan()
	if !read {
		return "", errors.New("Failed to read from scanner")
	}
	return s.Text(), nil
}
