// Package day22 solves day 22 of AoC2022.
package day22

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"

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
	return solve(world, path, Toroidal{})
}

// Part2 solves the second half of today's problem.
func Part2(world [][]Cell, path []Instruction, faceSize int) (int, error) {
	cube := newCubicTopology(world, faceSize)
	return solve(world, path, cube)
}

func solve(world [][]Cell, path []Instruction, top Topology) (int, error) {
	if len(world) == 0 {
		return 0, errors.New("empty world")
	}
	heading := Right
	x := array.Find(world[0], Walkable, fun.Eq[Cell])
	y := 0

	for idx, instr := range path {
		heading = Steer(heading, instr.Turn)
		if err := Advance(world, &x, &y, &heading, instr.Distance, top); err != nil {
			return 0, fmt.Errorf("error processing instruction %d: %v", idx, err)
		}
	}

	return 1000*(y+1) + 4*(x+1) + int(heading), nil
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
	Clockwise        Turn = 1
	Reverse          Turn = 2
	CounterClockwise Turn = 3
)

// Heading is an enum type for the heading to move towards.
type Heading int8

// Do not change, their values are not arbitrary:
// - They are ordered clock-wise. This makes steering simpler.
// - Their numerical values are defined in the problem statement.
const (
	Right Heading = 0
	Down  Heading = 1
	Left  Heading = 2
	Up    Heading = 3
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

// Advance takes a state (world and location) and applies a displacement (heading and distance).
func Advance(world [][]Cell, x, y *int, h *Heading, d int, top Topology) error {
	for d != 0 {
		switch *h {
		case Left:
			advanceHorizontally(world, x, y, &d, h, top)
		case Right:
			advanceHorizontally(world, x, y, &d, h, top)
		case Up:
			advanceVertically(world, x, y, &d, h, top)
		case Down:
			advanceVertically(world, x, y, &d, h, top)
		default:
			return fmt.Errorf("unexpected heading: %d", h)
		}
	}

	return nil
}

func advanceHorizontally(world [][]Cell, x, y, d *int, h *Heading, top Topology) {
	dir := 1
	if *h == Left {
		dir = -1
	}

	begin := *x
	end := fun.Clamp(-1, (*x)+(*d+1)*dir, len(world[*y]))

	// Find obstruction
	it := findObstructionH(world, *y, begin, end)
	advance := it - begin - dir

	// Out of this world
	if it == -1 || it == len(world[*y]) || (it != end && world[*y][it] == OffWorld) {
		wrappedY, wrappedX, wrappedH := top.wrapHorizontally(world, *y, *x, *h)

		// Cannot wrap around: there is a wall
		if world[wrappedY][wrappedX] == Wall {
			*x = it - dir
			*d = 0
			return
		}

		// Wrap-around
		*x = wrappedX
		*y = wrappedY
		*h = wrappedH
		*d -= fun.Abs(advance) + 1
		return
	}

	// No obstruction
	if it == end {
		*x += advance
		*d = 0
		return
	}

	// Hit a wall
	if world[*y][it] == Wall {
		*x += advance
		*d = 0
		return
	}
}

func advanceVertically(world [][]Cell, x, y, d *int, h *Heading, top Topology) {
	dir := 1
	if *h == Up {
		dir = -1
	}

	begin := *y
	end := fun.Clamp(-1, (*y)+(*d+1)*dir, len(world))

	// Find obstruction
	it := findObstructionV(world, *x, begin, end)
	advance := it - begin - dir

	// Out of this world
	if it == -1 || it == len(world) || world[it][*x] == OffWorld {
		wrappedY, wrappedX, wrappedH := top.wrapVertically(world, *y, *x, *h)

		// Cannot wrap around: there is a wall
		if world[wrappedY][wrappedX] == Wall {
			*y = it - dir
			*d = 0
			return
		}

		// Wrap-around
		*x = wrappedX
		*y = wrappedY
		*h = wrappedH
		*d -= fun.Abs(advance) + dir
		return
	}

	// No obstruction
	if it == end {
		*y += advance
		*d = 0
		return
	}

	// Hit a wall
	if world[it][*x] == Wall {
		*y += advance
		*d = 0
		return
	}
}

func findObstructionH(world [][]Cell, row, begin, end int) (col int) {
	s := fun.Sign(end - begin)
	col = begin
	for ; col != end; col += s {
		if world[row][col] != Walkable {
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

// Steer takes a Heading and a Turn and produces a new Heading.
func Steer(h Heading, t Turn) Heading {
	return clampHeading(int(h) + int(t))
}

// clampHeading takes an integer representing a heading, and removes
// any full revolution, i.e. it takes the positive value of x mod 4).
func clampHeading(x int) Heading {
	m := x % int(4)
	if m < 0 {
		m += int(4)
	}
	return Heading(m)
}

// headingToDiff takes a heading and returns the amount that
// the row and column must vary in order to advance one step
// in that direction.
func headingToDiff(h Heading) (drow, dcol int) {
	switch h {
	case Up:
		return -1, 0
	case Down:
		return 1, 0
	case Right:
		return 0, 1
	case Left:
		return 0, -1
	}
	panic("unreachable")
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

	p2, err := Part2(world, path, 50)
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
