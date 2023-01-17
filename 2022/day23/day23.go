// Package day23 solves day 23 of AoC2022.
package day23

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"math"
	"strings"
	"sync"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 23
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(world map[Location]Elf) (int, error) {
	anyMoves := true
	for step := 0; anyMoves && step < 10; step++ {
		world, anyMoves = automataStep(world, step)
	}

	area := newBoundingBox(world).area()
	elfCount := len(world)

	return area - elfCount, nil
}

// Part2 solves the second half of today's problem.
func Part2(world map[Location]Elf) (int, error) {
	anyMoves := true
	var step int
	for anyMoves {
		world, anyMoves = automataStep(world, step)
		step++
	}
	return step, nil
}

// ------------ Implementation ---------------------

type direction int8

const ( // Counter-clockwise ordering. Do not change.
	north direction = iota
	west
	south
	east
)

// rotate takes a direction and rotates it by a multiple of 90 degrees.
// Positive values go counter-clockwise, negatives go clockwise.
func rotate(d direction, delta int) direction {
	r := (int(d) + delta) % 4
	if r < 0 {
		r += 4
	}
	return direction(r)
}

var directionPriority = [4]direction{north, south, west, east}

// Location is a struct holding the row and column an elf is located in.
type Location struct {
	Row, Col int
}

// move takes a location and moves it one step in the specified direction.
func move(l Location, d direction) Location {
	switch d {
	case north:
		return Location{Row: l.Row - 1, Col: l.Col}
	case south:
		return Location{Row: l.Row + 1, Col: l.Col}
	case west:
		return Location{Row: l.Row, Col: l.Col - 1}
	case east:
		return Location{Row: l.Row, Col: l.Col + 1}
	}
	panic("Unreachable")
}

// Elf as in the problem statement. Only info needed is the cell it came
// from in order to revert steps when tow elves want to move to the same
// place.
type Elf struct {
	cameFrom Location
}

type boundingBox struct {
	minRow, maxRow, minCol, maxCol int
}

func newBoundingBox(world map[Location]Elf) boundingBox {
	b := boundingBox{
		minRow: math.MaxInt,
		maxRow: math.MinInt,
		minCol: math.MaxInt,
		maxCol: math.MinInt,
	}

	for loc := range world {
		b.minRow = utils.Min(b.minRow, loc.Row)
		b.maxRow = utils.Max(b.maxRow, loc.Row)
		b.minCol = utils.Min(b.minCol, loc.Col)
		b.maxCol = utils.Max(b.maxCol, loc.Col)
	}

	return b
}

func (b boundingBox) area() int {
	return (b.maxCol - b.minCol + 1) * (b.maxRow - b.minRow + 1)
}

// automataStep advances one step. Returns the stepped world and whether anyone moved.
// Given N elves:
// - Time complexity is O(N·log(N))
// - Memory complexity is O(N).
func automataStep(input map[Location]Elf, round int) (map[Location]Elf, bool) {
	revertMe := make([]Location, 0, len(input)) // Time is more constrained than memory: we over-allocate
	output := make(map[Location]Elf, len(input))

	var moveCount int

	for from := range input {
		dest := automatonSelectTarget(input, round, from)

		if from == dest {
			output[dest] = Elf{}
			continue
		}

		if _, occupied := output[dest]; occupied {
			// Conflict: someone else moved there first.
			// We stay put and tell the other to revert their move.
			output[from] = Elf{cameFrom: from}
			revertMe = append(revertMe, dest)
			continue
		}
		// Destination is free, let's move. We can revert later if conflict arises.
		output[dest] = Elf{cameFrom: from}
		moveCount++
	}

	// Move back everyone who entered into conflict after having moved.
	for _, loc := range revertMe {
		src := output[loc].cameFrom
		delete(output, loc)
		output[src] = Elf{}
	}
	moveCount -= len(revertMe)

	return output, moveCount != 0
}

// automatonSelectTarget choses where an automaton wants to move to.
// Given N elves:
// - Time complexity is O(N)
// - Memory complexity is O(1).
func automatonSelectTarget(input map[Location]Elf, round int, elfLoc Location) Location {
	neighbourhood := [3][3]bool{}
	var neighbours int
	for r := -1; r <= 1; r++ {
		for c := -1; c <= 1; c++ {
			if r == 0 && c == 0 {
				continue // Ignoring itself
			}

			loc := Location{
				Row: elfLoc.Row + r,
				Col: elfLoc.Col + c,
			}

			if _, o := input[loc]; !o {
				continue
			}

			neighbourhood[r+1][c+1] = true
			neighbours++
		}
	}

	if neighbours == 0 {
		return elfLoc // No need to move
	}

	if neighbours > 5 {
		return elfLoc // No room to move (Need at least 3 free spots in a row)
	}

	for i := 0; i < 4; i++ {
		direction := directionPriority[(round+i)%4]
		elf := Location{1, 1}

		inFront := move(elf, direction)
		if occupied := neighbourhood[inFront.Row][inFront.Col]; occupied {
			continue
		}

		frontLeft := move(inFront, rotate(direction, -1))
		if occupied := neighbourhood[frontLeft.Row][frontLeft.Col]; occupied {
			continue
		}

		frontRight := move(inFront, rotate(direction, +1))
		if occupied := neighbourhood[frontRight.Row][frontRight.Col]; occupied {
			continue
		}

		return move(elfLoc, direction) // Move in front
	}

	return elfLoc
}

func pretty(world map[Location]Elf) string { // nolint: unused
	b := newBoundingBox(world)
	s := make([]string, 0, b.maxRow-b.minRow+1)
	l := Location{Row: 0, Col: 0}
	for l.Row = b.minRow; l.Row <= b.maxRow; l.Row++ {
		line := make([]rune, 0, b.maxCol-b.minCol+1)
		for l.Col = b.minCol; l.Col <= b.maxCol; l.Col++ {
			e, occupied := world[l]
			switch {
			case !occupied:
				line = append(line, '.')
			case e.cameFrom == l:
				line = append(line, 'O')
			default:
				switch {
				case e.cameFrom.Col == l.Col && e.cameFrom.Row+1 == l.Row:
					line = append(line, 'v')
				case e.cameFrom.Col == l.Col && e.cameFrom.Row-1 == l.Row:
					line = append(line, '^')
				case e.cameFrom.Row == l.Row && e.cameFrom.Col+1 == l.Col:
					line = append(line, '>')
				case e.cameFrom.Row == l.Row && e.cameFrom.Col-1 == l.Col:
					line = append(line, '<')
				default:
					line = append(line, 'X')
				}
			}
		}

		s = append(s, string(line))
	}

	return strings.Join(s, "\n")
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem's solution.
func Main(stdout io.Writer) error {
	data, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(data)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(data)
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

// ReadData reads the data file and returns a sparse representation
// of the elf matrix. It uses a DOK("Dictionary of Keys") format for
// this matrix.
func ReadData() (data map[Location]Elf, err error) { // nolint: revive
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	ch := make(chan Location)
	var wg sync.WaitGroup

	for row := 0; sc.Scan(); row++ {
		row := row
		line := sc.Text()
		wg.Add(1)
		go func() {
			defer wg.Done()
			for col, char := range line {
				if char != '#' {
					continue
				}
				ch <- Location{Row: row, Col: col}
			}
		}()
	}

	go func() {
		wg.Wait()
		close(ch)
	}()

	elves := map[Location]Elf{}
	for where := range ch {
		elves[where] = Elf{}
	}

	return elves, sc.Err()
}
