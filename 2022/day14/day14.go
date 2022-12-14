// Package day14 solves day 14 of AoC2022.
package day14

import (
	"bufio"
	"bytes"
	"fmt"
	"io"
	"math"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 14
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(world [][]Cell, offset Location) (uint, error) {

	var count uint
	for done := false; !done; count++ {
		pouring := Location{X: 500 - offset.X, Y: len(world)}
		done = simulate(&world, pouring)
	}

	return count - 1, nil
}

// Part2 solves the second half of the problem.
func Part2(world [][]Cell, offset Location) (uint, error) {
	return 0, nil
}

// ------------- Implementation ------------------

func simulate(world *[][]Cell, p Location) (infinity bool) {
	for {
		// Attemp to fall
		for {
			// Fall off the world
			if p.Y-1 < 0 {
				return true
			}
			// Cannot fall any longer
			if (*world)[p.Y-1][p.X] != Air {
				break
			}
			// Continue falling
			p.Y--
		}
		// Slide left
		if (*world)[p.Y-1][p.X-1] == Air {
			p.Y--
			p.X--
			continue
		}
		// Slide right
		if (*world)[p.Y-1][p.X+1] == Air {
			p.Y--
			p.X++
			continue
		}
		//Nowhere to go
		break
	}
	// Stack of sand piled up to the source!
	if p.Y >= cap(*world) {
		panic("Stack of sand piled up to the source!")
	}
	// Stack of sand piled up to the top, making more room.
	if p.Y >= len(*world) {
		*world = append(*world, make([]Cell, len((*world)[0])))
	}
	(*world)[p.Y][p.X] = Sand

	return false
}

type Cell int

const (
	Air Cell = iota // Air must be default value
	Rock
	Sand
)

type Location struct {
	X, Y int
}

func AssembleWorld(segments [][2]Location) (world [][]Cell, offset Location) {
	if len(segments) == 0 {
		return [][]Cell{{Air, Air, Air}}, Location{-1, 0}
	}

	// Finding extrema
	type extrema struct {
		minX, minY, maxX, maxY int
	}
	ex := array.Reduce(segments, func(ex extrema, s [2]Location) extrema {
		// Exploiting fact that segments go from small to big
		ex.minX = fun.Min(ex.minX, s[0].X)
		ex.minY = fun.Min(ex.minY, s[0].Y)
		ex.maxX = fun.Max(ex.maxX, s[1].X)
		ex.maxY = fun.Max(ex.maxY, s[1].Y)
		return ex
	}, extrema{math.MaxInt, math.MaxInt, math.MinInt, math.MinInt})

	// Making room to let sand fall off the sides
	ex.minX--
	ex.maxX++

	// Generating world
	height := int64(ex.maxY) - int64(ex.minY) + 1
	width := int64(ex.maxX) - int64(ex.minX) + 1

	world = make([][]Cell, height, 1-ex.minY) // (World is upside down)
	array.Foreach(world, func(row *[]Cell) { *row = make([]Cell, width) })

	offset = Location{ex.minX, ex.minY}

	// Filling world
	array.Foreach(segments, func(s *[2]Location) {
		// Exploiting fact that segments go from small to big
		s[0].X -= offset.X
		s[0].Y -= offset.Y
		s[1].X -= offset.X
		s[1].Y -= offset.Y
		dx := fun.Sign(s[1].X - s[0].X)
		dy := fun.Sign(s[1].Y - s[0].Y)
		l := s[0]
		for l.X <= s[1].X && l.Y <= s[1].Y {
			world[l.Y][l.X] = Rock
			l.X += dx
			l.Y += dy
		}
	})

	return world, offset
}

func copyWorld(w [][]Cell) [][]Cell {
	return array.Map(w, func(r []Cell) []Cell { return array.Map(r, fun.Identity[Cell]) })
}

// ---------- Here be boilerplate ------------------

type problemResult struct {
	id  int
	res string
	err error
}

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	world1, offset := AssembleWorld(input)
	world2 := copyWorld(world1)

	resultCh := make(chan problemResult)
	go func() {
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part1(world1, offset)
		if err != nil {
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %d", result), nil}
	}()

	go func() {
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part2(world2, offset)
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

// ReadData reads the data file and returns the list of
// reindeers and their calories.
func ReadData() ([][2]Location, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	segments := [][2]Location{}
	for sc.Scan() {
		tokens := strings.Split(sc.Text(), " -> ")
		if len(tokens) < 2 {
			return nil, fmt.Errorf("Not enough points in line %q", sc.Text())
		}
		localSegments := make([][2]Location, 0, len(tokens)-1)

		var prev Location
		_, err := fmt.Sscanf(tokens[0], "%d,%d", &prev.X, &prev.Y)
		if err != nil {
			return nil, fmt.Errorf("Failed to parse point %q in line %q: %v", tokens[0], sc.Text(), err)
		}
		prev.Y *= -1 // We store the world upside down

		for _, tok := range tokens[1:] {
			var new Location
			_, err := fmt.Sscanf(tok, "%d,%d", &new.X, &new.Y)
			if err != nil {
				return nil, fmt.Errorf("Failed to parse point %q in line %q: %v", tokens[0], sc.Text(), err)
			}
			new.Y *= -1 // We store the world upside down

			localSegments = append(localSegments, [2]Location{prev, new})
			latest := &localSegments[len(localSegments)-1]
			// Swapping: segments go from small to big
			if latest[0].X > latest[1].X || latest[0].Y > latest[1].Y {
				latest[0], latest[1] = latest[1], latest[0]
			}

			prev = new
		}

		segments = append(segments, localSegments...)
	}

	if err := sc.Err(); err != nil {
		return segments, err
	}

	return segments, nil
}
