// Package day12 solves day 12 of AoC2022.
package day12

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"math"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/dstruct"
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

	cost := algo.Map(world, func(row []uint8) []int { return algo.Map(row, func(uint8) int { return math.MaxInt }) })

	// We start at the S cell
	start := findCell(world, 'S')
	world[start.i][start.j] = 'a'
	cost[start.i][start.j] = 0

	// We end at the E cell
	end := findCell(world, 'E')
	world[end.i][end.j] = 'z'

	// Dijkstra time!
	candidates := dstruct.NewHeap(func(a, b idx) bool { return cost[a.i][a.j] < cost[b.i][b.j] })
	allowed := func(jump int) bool { return jump < 2 }
	visit(world, cost, candidates, allowed, start)
	for i := 0; candidates.Len() != 0; i++ {
		if i > len(world)*len(world[0]) {
			break
		}
		best := candidates.Pop()
		if best == end {
			return cost[best.i][best.j], nil
		}
		visit(world, cost, candidates, allowed, best)
	}
	return 0, errors.New("failed to find a path to the end")
}

// Part2 solves the second half of the problem.
func Part2(world [][]uint8) (int, error) {
	if len(world) == 0 || len(world[0]) == 0 {
		return 0, errors.New("empty world")
	}

	cost := algo.Map(world, func(row []uint8) []int { return algo.Map(row, func(uint8) int { return math.MaxInt }) })

	// S cell is just like any other 'a' cell now
	s := findCell(world, 'S')
	world[s.i][s.j] = 'a'

	// We start at the E cell
	start := findCell(world, 'E')
	world[start.i][start.j] = 'z'
	cost[start.i][start.j] = 0

	// Dijkstra time!
	candidates := dstruct.NewHeap(func(a, b idx) bool { return cost[a.i][a.j] < cost[b.i][b.j] })
	allowed := func(jump int) bool { return jump > -2 }
	visit(world, cost, candidates, allowed, start)
	for candidates.Len() != 0 {
		best := candidates.Pop()
		if world[best.i][best.j] == 'a' {
			return cost[best.i][best.j], nil
		}
		visit(world, cost, candidates, allowed, best)
	}
	return 0, errors.New("failed to find a path from the end")
}

// ---------- Implementation ------------------.
func visit(world [][]uint8, cost [][]int, candidates dstruct.Heap[idx], allowed func(int) bool, at idx) {
	if at.i > 0 {
		to := idx{at.i - 1, at.j}
		consider(world, cost, candidates, allowed, at, to)
	}

	if at.i < len(world)-1 {
		to := idx{at.i + 1, at.j}
		consider(world, cost, candidates, allowed, at, to)
	}

	if at.j > 0 {
		to := idx{at.i, at.j - 1}
		consider(world, cost, candidates, allowed, at, to)
	}

	if at.j < len(world[0])-1 {
		to := idx{at.i, at.j + 1}
		consider(world, cost, candidates, allowed, at, to)
	}
}

func consider(world [][]uint8, cost [][]int, candidates dstruct.Heap[idx], allowed func(int) bool, from, to idx) {
	climb := int(world[to.i][to.j]) - int(world[from.i][from.j])
	if !allowed(climb) {
		return
	}
	c := cost[from.i][from.j] + 1
	// This only works because all edge costs are the same!
	if c < cost[to.i][to.j] {
		candidates.Push(to)
		cost[to.i][to.j] = c
	}
}

type idx struct {
	i, j int
}

func findCell(world [][]uint8, w uint8) idx {
	for r, row := range world {
		for c, cell := range row {
			if cell != w {
				continue
			}
			return idx{r, c}
		}
	}
	return idx{-1, -1}
}

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
			return
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
