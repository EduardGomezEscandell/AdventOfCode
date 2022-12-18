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
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/lrucache"
)

const (
	today    = 17
	fileName = "input.txt"
)

func part1(winds []int8) (Long, error) {
	return Solve(winds, 2022)
}

func part2(winds []int8) (Long, error) {
	return Solve(winds, 1_000_000_000_000)
}

// Solve solves the problem.
func Solve(winds []int8, nrocks Long) (Long, error) {
	rocks := rocks()

	// Rocks. Only the part of the world that is accessible is
	// stored. Once a row is filled, everything under it is erased,
	// and the offset stored.
	world := make([]byte, 1000)

	// The y-coordinate that world[0] represents. Only matters after
	// we drop inaccessible rows.
	var yOffset Long

	// State cache. We'll use this to detect cycles.
	// We only fill it with small states (i.e. at most 4 rows)
	// so we don't need the cache to be massive.
	lru := lrucache.New[state, progress](100)

	prog := progress{
		y: 0,
		n: 0,
	}

	// Current wind index. It will flip back to 0 once the entire
	// wind directive is explored.
	var windID int

	for ; prog.n < nrocks; prog.n++ {
		// Current rock drop. It will flip back to 0 once the entire
		// wind directive is explored.
		rockID := int(prog.n % Long(len(rocks)))

		// Location of bottom-left corner of the current rock.
		x := 2
		y := int(prog.y-yOffset) + 3

		for {
			// Side-move
			w := int(winds[windID])
			windID = (windID + 1) % len(winds)
			if !overlap(world, rocks[rockID], x+w, y) {
				x += w
			}
			// Drop
			if !overlap(world, rocks[rockID], x, y-1) {
				y--
			} else {
				break
			}
		}

		// Adding rock to world, possibly dropping newly inaccessible rows.
		var err error
		var droppedRows int
		world, droppedRows, err = addRock(world, rocks[rockID], x, y)
		if err != nil {
			return 0, err
		}

		// Updating maximum y coordinate
		prog.y = fun.Max(prog.y, Long(y+rocks[rockID].height)+yOffset)
		yOffset += Long(droppedRows)

		// Seeing if we've had the same state before, but only consider
		// simple states (i.e. at most 4 rows have rocks)
		if int(prog.y-yOffset) >= 4 {
			continue
		}

		// Compressing worldstate so we can cache it
		s := compressState(world, rockID, windID)

		cached, hit := lru.Get(s)
		if !hit {
			// Cache miss. Add state to cache and continue.
			lru.Set(s, prog)
			continue
		}

		// Cache hit! We've detected a cycle, so we can fast-forward.
		lru.Set(s, prog)

		// Cycle progress
		deltaY := prog.y - cached.y
		deltaRock := prog.n - cached.n

		// Counting how many cycles are left
		rocksLeft := nrocks - prog.n
		epochsLeft := (rocksLeft / deltaRock) - 1
		if epochsLeft < 1 {
			// We're too close to the end, continuing normal execution
			continue
		}

		// Fast-forwarding as many cycles as possible
		prog.n += epochsLeft * deltaRock
		prog.y += epochsLeft * deltaY
		yOffset += epochsLeft * deltaY
	}

	return prog.y, nil
}

// ------------- Implementation ------------------.

// Long is used for large Y coordinates and large rock counts.
type Long int64

// state contains the state of the world and condensed it into a struct.
// Can only contain worldstates where fewer than 5  rows contain rocks.
type state struct {
	world  [4]byte
	rockID int
	windID int
}

// Takes state of the world and condenses it into a struct.
// Assumes that the state is compressible, i.e. fewer than 5
// rows contain rocks.
func compressState(world []byte, rockID int, windID int) state {
	st := state{rockID: rockID, windID: windID}
	copy(st.world[:], world)
	return st
}

// progress contains the number of dropped rocks (n) and
// the largest y coordinate.
type progress struct {
	n Long
	y Long
}

// rock contains the geometry of a rock as a bit mask.
type rock struct {
	shape  []byte
	width  int
	height int
}

// rocks initializes the rock list, as defined on the problem
// statement.
func rocks() []rock {
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

// overlap returns true if:
// - the world's rocks and the given rock overlap
// - the rock hits a sidewall
// - the rock hits the floor.
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

// overwrite writes a rock into the world.
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

// addRock writes a rock into world, possibly dropping newly inaccessible rows.
// returns the world without the dropped rows, the number of rows dropped and
// possibly an error,.
func addRock(world []byte, r rock, x, y int) ([]byte, int, error) {
	minSize := y + r.height + 10 // 100 rows of buffer space seems enough
	slack := len(world) - minSize
	if slack < 0 {
		world = append(world, make([]byte, len(world))...)
	}

	if err := overwrite(world, r, x, y); err != nil {
		return nil, 0, err
	}

	// If there is a full row, we drop everything under it
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

// prettyPrint is a helper function to make debugging easier.
func prettyPrint(world []byte, r rock, X, Y int) string { // nolint: unused
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

// ReadData reads the data file and returns a bitmask of the
// world's rocks.
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
