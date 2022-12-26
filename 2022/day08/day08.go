// Package day08 solves day 2 of AoC2022	.
package day08

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"sync"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 8
	fileName = "input.txt"
)

// Height is a tree height.
type Height int8

// Part1 solves the first half of the problem.
func Part1(forest [][]Height) (uint, error) {
	minHeights := ComputeMinimumHeights(forest)

	// A tree is visible if it is taller than the minimum height in its location
	visible := array.ZipReduce(forest, minHeights, func(fRow []Height, mhRow []Height) uint {
		return array.ZipReduce(fRow, mhRow, fun.Gt[Height], fun.Count[uint], 0)
	}, fun.Add[uint], 0)

	return visible, nil
}

// Part2 solves the second half of the problem.
func Part2(input [][]Height) (uint, error) {
	return findBestScenery(input), nil
}

// ---------------- Implementation -----------------------

// ComputeMinimumHeights computes the minimum height necessary for a tree to be visible
// at each location.
func ComputeMinimumHeights(forest [][]Height) [][]Height {
	depth := len(forest)
	if depth == 0 {
		return [][]Height{}
	}
	width := len(forest[0])

	// same shape as forest
	minHeights := array.Generate(depth, func() []Height { return make([]Height, width) })

	// Left->right scan
	for r := 0; r < depth; r++ {
		var acc Height = -1
		for c := 0; c < width; c++ {
			minHeights[r][c] = acc
			acc = fun.Max(acc, forest[r][c])
		}
	}

	// Right->left scan
	for r := 0; r < depth; r++ {
		var acc Height = -1
		for c := width - 1; c >= 0; c-- {
			minHeights[r][c] = fun.Min(acc, minHeights[r][c])
			acc = fun.Max(acc, forest[r][c])
		}
	}

	// Top->bottom scan
	for c := 0; c < width; c++ {
		var acc Height = -1
		for r := 0; r < depth; r++ {
			minHeights[r][c] = fun.Min(acc, minHeights[r][c])
			acc = fun.Max(acc, forest[r][c])
		}
	}

	// Bottom->top scan
	for c := 0; c < width; c++ {
		var acc Height = -1
		for r := depth - 1; r >= 0; r-- {
			minHeights[r][c] = fun.Min(acc, minHeights[r][c])
			acc = fun.Max(acc, forest[r][c])
		}
	}

	return minHeights
}

// findBestScenery finds the best scenery. Rows are
// computed in parallel.
func findBestScenery(forest [][]Height) uint {
	ch := make(chan uint)

	var wg sync.WaitGroup
	for i := range forest {
		wg.Add(1)
		i := i
		go func() {
			defer wg.Done()
			var best uint
			for j := range forest[i] {
				best = fun.Max(best, ComputeScenery(forest, i, j))
			}
			ch <- best
		}()
	}

	go func() {
		wg.Wait()
		close(ch)
	}()

	return charray.Best(ch, fun.Gt[uint])
}

// ComputeScenery computes the scenery score for a single location
// in the forest.
func ComputeScenery(forest [][]Height, row, col int) uint {
	depth := len(forest)
	width := len(forest[0])

	h := forest[row][col]
	var l, r, u, d uint // left, right, up, down

	// Scanning down
	for i := row + 1; i < depth; i++ {
		d++
		if h <= forest[i][col] {
			break
		}
	}

	// Scanning up
	for i := row - 1; i >= 0; i-- {
		u++
		if h <= forest[i][col] {
			break
		}
	}

	// Scanning right
	for i := col + 1; i < width; i++ {
		r++
		if h <= forest[row][i] {
			break
		}
	}

	// Scanning left
	for i := col - 1; i >= 0; i-- {
		l++
		if h <= forest[row][i] {
			break
		}
	}

	return l * r * u * d
}

// ------------- Here be boilerplate ------------------

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
	fmt.Fprintf(stdout, "Result of part 1: %d\n", result)

	result, err = Part2(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", result)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ParseInput reads the data file and returns the list of
// reindeers and their calories.
func ParseInput() (data [][]Height, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return data, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	data = [][]Height{}

	for sc.Scan() {
		line := sc.Text()
		arr := array.Map([]rune(line), func(r rune) Height { return Height(r - '0') })
		data = append(data, arr)
	}

	if err := sc.Err(); err != nil {
		return data, err
	}

	return data, nil
}
