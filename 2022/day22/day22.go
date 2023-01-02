// Package day22 solves day 22 of AoC2022.
package day22

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 22
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(world [][]Cell, path []Instruction) (int, error) {
	if len(world) == 0 {
		return 0, errors.New("empty world")
	}
	heading := Right
	x := array.Find(world[0], Walkable, fun.Eq[Cell])
	y := 0

	for idx, instr := range path {
		heading = Steer(heading, instr.Turn)
		if err := Advance(world, &x, &y, heading, instr.Distance); err != nil {
			return 0, fmt.Errorf("error processing instruction %d: %v", idx, err)
		}
	}

	fmt.Printf("Row: %d, Col: %d, Heading: %v (%d) \n", y, x, heading, heading)

	return 1000*(y+1) + 4*(x+1) + int(heading), nil
}

func pretty(world [][]Cell, x, y int, h Heading) string { // nolint: unused
	s := []string{}
	for i := range world {
		if i != y {
			s = append(s, string(array.Map(world[i], func(c Cell) rune { return rune(c) })))
			continue
		}
		line := ""
		for j, r := range world[i] {
			if j != x {
				line = fmt.Sprintf("%s%c", line, r)
				continue
			}
			line = fmt.Sprintf("%s%v", line, h)
		}
		s = append(s, line)
	}
	return strings.Join(s, "\n")
}

// Part2 solves the second half of today's problem.
func Part2(input [][]Cell) (int64, error) {
	return 1, nil
}

// ------------ Implementation ---------------------

// Cell is an enum type to specify the type of every location in the world.
type Cell rune

// Enum. Their values are chosen to make reading theinput simpler.
const (
	OffWorld Cell = ' '
	Walkable Cell = '.'
	Wall     Cell = '#'
)

// Turn is an enum type for direction to turn.
type Turn int8

// Enum. Straight only appears on the first move.
const (
	Straight         Turn = 0
	CounterClockwise Turn = -1
	Clockwise        Turn = +1
)

// Heading is an enum type for the heading to move towards.
type Heading int8

// Do not change:
// - They are ordered in clock-wise order. This makes steering simpler.
// - Their numerical values are defined in the problem statement.
const (
	Right Heading = iota
	Down
	Left
	Up
)

func (h Heading) String() string {
	switch h {
	case Right:
		return ">"
	case Down:
		return "v"
	case Up:
		return "^"
	case Left:
		return "<"
	}
	panic("ureachable")
}

// Instruction is a combination of turn to make, and a distance to attempt to
// traverse.
type Instruction struct {
	Turn     Turn
	Distance int
}

// Steer takes a Heading and a Turn and produces a new Heading.
func Steer(h Heading, t Turn) Heading {
	h = Heading(int8(h)+int8(t)) % 4
	if h < 0 {
		h += 4
	}
	return h
}

// Advance takes a state (world and location) and applies a displacement (heading and distance).
func Advance(world [][]Cell, x, y *int, h Heading, d int) error {
	switch h {
	case Left:
		advanceHorizontally(world, x, y, -d)
	case Right:
		advanceHorizontally(world, x, y, d)
	case Up:
		advanceVertically(world, x, y, -d)
	case Down:
		advanceVertically(world, x, y, d)
	default:
		return fmt.Errorf("unexpected heading: %d", h)
	}

	return nil
}

func advanceHorizontally(world [][]Cell, x, y *int, d int) {
	line := world[*y]
	for d != 0 {
		begin := *x
		end := fun.Clamp(-1, *x+d+fun.Sign(d), len(line))

		// Find obstruction
		it := findObstructionH(line, begin, end)
		advance := it - begin - fun.Sign(d)

		// Out of this world
		if it == -1 || it == len(line) || line[it] == OffWorld {
			begin = 0
			end = it
			if d < 0 {
				begin = len(line) - 1
			}

			wrappedX := findStartH(line, begin, end)

			// Cannot wrap around: there is a wall
			if line[wrappedX] == Wall {
				*x = end - fun.Sign(d)
				return
			}

			// Wrap-around
			*x = wrappedX
			d -= advance + fun.Sign(d)
			continue
		}

		// No obstruction
		if it == end {
			*x += advance
			return
		}

		// Hit a wall
		if line[it] == Wall {
			*x += advance
			return
		}
	}
}

func advanceVertically(world [][]Cell, x, y *int, d int) {
	for d != 0 {
		begin := *y
		end := fun.Clamp(-1, *y+d+fun.Sign(d), len(world))

		// Find obstruction
		it := findObstructionV(world, *x, begin, end)
		advance := it - begin - fun.Sign(d)

		// Out of this world
		if it == -1 || it == len(world) || world[it][*x] == OffWorld {
			begin = 0
			end = it
			if d < 0 {
				begin = len(world) - 1
			}

			wrappedY := findStartV(world, *x, begin, end)

			// Cannot wrap around: there is a wall
			if world[wrappedY][*x] == Wall {
				*y = end - fun.Sign(d)
				return
			}

			// Wrap-around
			*y = wrappedY
			d -= advance + fun.Sign(d)
			continue
		}

		// No obstruction
		if it == end {
			*y += advance
			return
		}

		// Hit a wall
		if world[it][*x] == Wall {
			*y += advance
			return
		}
	}
}

func findObstructionH(line []Cell, begin, end int) (it int) {
	s := fun.Sign(end - begin)
	it = begin
	for ; it != end; it += s {
		if line[it] != Walkable {
			return
		}
	}
	return
}

func findStartH(line []Cell, begin, end int) (it int) {
	s := fun.Sign(end - begin)
	it = begin
	for ; it != end; it += s {
		if line[it] != OffWorld {
			return
		}
	}
	return
}

func findObstructionV(world [][]Cell, col, begin, end int) (row int) {
	s := fun.Sign(end - begin)
	row = begin
	for ; row != end; row += s {
		if world[row][col] != Walkable {
			return
		}
	}
	return
}

func findStartV(world [][]Cell, col, begin, end int) (row int) {
	s := fun.Sign(end - begin)
	row = begin
	for ; row != end; row += s {
		if world[row][col] != OffWorld {
			return
		}
	}
	return
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	world, path, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := Part1(world, path)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(world)
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

// ReadData reads the data file and generates a world map and a path.
func ReadData() ([][]Cell, []Instruction, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	// Section 1: world map
	world := [][]Cell{}
	maxLen := 0

	for idx := 0; sc.Scan(); idx++ {
		text := sc.Text()
		if len(text) == 0 {
			break // End of section
		}

		world = append(world, []Cell(text))
		maxLen = fun.Max(len(text), maxLen)
	}

	world = array.Map(world, func(line []Cell) []Cell {
		padding := maxLen - len(line)
		return append(line, array.Generate(padding, func() Cell { return OffWorld })...)
	})

	// Section 2: path
	read := sc.Scan()
	if !read {
		return nil, nil, errors.New("unexpected termination of input: missing path")
	}

	path, err := parsePath(sc.Text())
	if err != nil {
		return nil, nil, err
	}

	if sc.Scan() {
		return nil, nil, errors.New("unexpected extra line after path")
	}

	return world, path, sc.Err()
}

func parsePath(text string) ([]Instruction, error) {
	path := []Instruction{}

	i := Instruction{Turn: Straight}
	var eofValid bool
	for idx, r := range text {
		switch {
		case '0' <= r && r <= '9':
			i.Distance = i.Distance*10 + int(r-'0')
			eofValid = true
		case r == 'R':
			path = append(path, i)
			i = Instruction{Turn: Clockwise}
			eofValid = false
		case r == 'L':
			path = append(path, i)
			i = Instruction{Turn: CounterClockwise}
			eofValid = false
		default:
			return nil, fmt.Errorf("unexpected character in path: character %c in position %d", r, idx)
		}
	}

	if !eofValid {
		return nil, errors.New("unexpected end of path: last instruction not finished")
	}

	path = append(path, i)

	return path, nil
}
