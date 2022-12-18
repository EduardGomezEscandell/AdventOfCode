// Package day17 solves day 17 of AoC2022.
package day17

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"math"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 17
	fileName = "input.txt"
)

type rock struct {
	shape  []byte
	width  int
	height int
}

type Long int64
type time Long

func rocks() []rock {
	const (
		o = 0
		X = 1
	)
	return []rock{ // Note that they're upside down
		{shape: []byte{0b_1111_000}, width: 4, height: 1}, // 1111···
		{shape: []byte{
			0b_010_0000,
			0b_111_0000,
			0b_010_0000,
		}, width: 3, height: 3},
		{shape: []byte{
			0b_111_0000,
			0b_001_0000,
			0b_001_0000,
		}, width: 3, height: 3},
		{shape: []byte{
			0b_1_000000,
			0b_1_000000,
			0b_1_000000,
			0b_1_000000,
		}, width: 1, height: 4},
		{shape: []byte{
			0b_11_00000,
			0b_11_00000,
		}, width: 2, height: 2},
	}
}

// part1 solves the first half of the problem.
func part1(winds []int8) (Long, error) {
	return Solve(winds, 2022)
}

// Part2 solves the second half of the problem.
func part2(winds []int8) (Long, error) {
	// return Solve(winds, 1_000_000_000_000)
	return 1, nil
}

// ------------- Implementation ------------------.

// Solve solves the problem.
func Solve(winds []int8, nrocks Long) (Long, error) {
	wind := func(wtime time) int {
		return int(winds[wtime%time(len(winds))])
	}
	rocks := rocks()
	world := make([]byte, 1000)

	var maxY Long    // Maximum Y coordinate of any settled rock
	var yOffset Long // The y-coordinate that cell[0] represents
	var windTime time
	for r := Long(0); r < nrocks; r++ {
		y := int(maxY-yOffset) + 3
		x := 2
		rck := rocks[r%Long(len(rocks))]
		// fmt.Println(prettyPrint(world, rck, x, y))
		for {
			w := wind(windTime)
			windTime++
			if !overlap(world, rck, x+w, y) {
				x += w
			}
			if !overlap(world, rck, x, y-1) {
				// fmt.Println(prettyPrint(world, rck, x, y))
				y--
				continue
			}
			break
		}

		var err error
		var newoffset int
		world, newoffset, err = addRock(world, rck, x, y)
		if err != nil {
			return 0, err
		}
		maxY = fun.Max(maxY, Long(y+rck.height)+yOffset)
		yOffset += Long(newoffset)
		// fmt.Println(prettyPrint(world, rock{}, 0, int(maxY-yOffset)))
	}

	return maxY, nil
}

func addRock(world []byte, r rock, x, y int) ([]byte, int, error) {
	minSize := y + r.height + 10 // 100 rows of buffer space seems enough
	slack := len(world) - minSize
	if slack < 0 {
		world = append(world, make([]byte, len(world))...)
	}

	if err := overwrite(world, r, x, y); err != nil {
		return nil, 0, err
	}

	// If there is a full row, we drop everyting under it
	var offset int
	for i := r.height - 1; i >= 0; i-- {
		y := y + i
		const fullRow = math.MaxUint8 >> 1
		if world[y] == fullRow {
			offset = y + 1
			world = world[offset:]
			break
		}
	}

	return world, offset, nil
}

func prettyPrint(world []byte, r rock, X, Y int) string {
	// Drawing world
	N := Y + r.height

	s := make([][]byte, N)
	for y := 0; y < N; y++ {
		s[y] = []byte(".......")
		row := world[y]
		for x := 6; row != 0; x-- {
			if row&1 == 1 {
				s[y][x] = '#'
			}
			row >>= 1
		}
	}

	// Overwriting rock
	for i := 0; Y+i < len(world) && i < r.height; i++ {
		y := Y + i
		row := r.shape[i] >> X
		for x := 6; row != 0; x-- {
			if row&1 == 1 {
				s[y][x] = '@'
			}
			row >>= 1
		}
	}

	floor := array.Generate(7, func() byte { return '=' })
	s = append(array.Reverse(s), floor)

	return strings.Join(array.Map(s, func(b []byte) string { return string(b) }), "\n") + "\n"
}

func overlap(world []byte, r rock, X, Y int) bool {
	if Y < 0 || Y+r.height > len(world) {
		return true
	}
	if X < 0 || X+r.width > 7 {
		return true
	}

	for i, rockrow := range r.shape {
		rockrow := rockrow >> X
		if world[Y+i]&rockrow != 0 {
			return true
		}
	}
	return false
}

func overwrite(world []byte, r rock, X, Y int) error {
	if Y < 0 || Y+r.height > len(world) {
		return fmt.Errorf("cannot write kernel into location i=%d. Want 0 <= i <= i+%d <= %d", Y, r.height, len(world))
	}
	if X < 0 || X+r.width > 7 {
		return fmt.Errorf("cannot write kernel into location j=%d. Want <= j <= j+%d <= 7", X, r.width)
	}

	for i, rockrow := range r.shape {
		world[Y+i] |= rockrow >> X
	}
	return nil
}

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

	p2, err := part2(wind)
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
