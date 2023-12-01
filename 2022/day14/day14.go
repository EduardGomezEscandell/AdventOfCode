// Package day14 solves day 14 of AoC2022.
package day14

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 14
	fileName = "input.txt"
)

const (
	SourceX = 500 // SourceX is the X coordinate of the location where the source is.
)

// Part1 solves the first half of the problem.
func Part1(world [][]Cell, offset int) (int, error) {
	pouring := Location{X: 500 - offset, Y: 0}

	for count := 1; ; count++ {
		p, err := simulate(world, pouring)
		if err != nil {
			return 0, err
		}
		if p.Y == len(world)-1 {
			return count - 1, nil // -1 because we don't write this one on the map
		}
		if p.Y == 0 {
			return 0, errors.New("clogged source in part 1")
		}
	}
}

// Part2 solves the second half of the problem.
func Part2(world [][]Cell, offset, precount int) (int, error) {
	pouring := Location{X: 500 - offset, Y: 0}

	for count := precount + 1; ; count++ {
		p, err := simulate(world, pouring)
		if err != nil {
			return 0, err
		}
		if p.Y == 0 {
			return count + 1, nil
		}
	}
}

// ------------- Implementation ------------------.

// Cell are the contents of a certain location in the world.
type Cell uint8

// Types of cell.
const (
	Air Cell = iota // Air must be default value
	Rock
	Sand
)

// Location is a position in space.
type Location struct {
	X, Y int
}

func simulate(world [][]Cell, p Location) (Location, error) {
	for {
		// Fall down
		if p.Y+1 == len(world) {
			break // Touched the bottom
		}
		if p.Y+1 > len(world) {
			return Location{}, errors.New("went bellow the bottom level")
		}
		if world[p.Y+1][p.X] == Air {
			p.Y++
			continue
		}

		// Slide left
		if p.X <= 0 {
			return Location{}, errors.New("fell off left side of the world")
		}
		if world[p.Y+1][p.X-1] == Air {
			p.Y++
			p.X--
			continue
		}

		// Slide right
		if p.X+1 >= len(world[0]) {
			return Location{}, errors.New("fell off right side of the world")
		}
		if world[p.Y+1][p.X+1] == Air {
			p.Y++
			p.X++
			continue
		}
		//Nowhere to go
		break
	}
	world[p.Y][p.X] = Sand
	return p, nil
}

// StrWorld displays the world as a string.
func StrWorld(world [][]Cell) string {
	str := fmt.Sprintf("Map(height: %d, width: %d):\n", len(world), len(world[0]))
	for i, row := range world {
		str += fmt.Sprintf("%3d | ", i)
		for _, cell := range row {
			switch cell {
			case Air:
				str += "."
			case Sand:
				str += "O"
			case Rock:
				str += "#"
			}
		}
		str += "\n"
	}
	return str
}

// AssembleWorld generates an array with the relevant part of the world.
// Xoffset indicates the X-value of the first column
//
//	-> i.e. world[0][0] corresponds to (x,y)=(Xoffset, 0).
func AssembleWorld(segments [][2]Location, sourceX int) (world [][]Cell, Xoffset int) {
	if len(segments) == 0 {
		return [][]Cell{{Air, Air, Air}}, 1
	}

	type box struct {
		MinX, MaxX, MinY, MaxY int
	}

	// Finding extrema
	b := algo.Reduce(segments, func(b box, segment [2]Location) box {
		// Exploiting fact that segments go from small to big
		b.MinX = utils.Min(b.MinX, segment[0].X)
		b.MinY = utils.Min(b.MinY, segment[0].Y)
		b.MaxX = utils.Max(b.MaxX, segment[1].X)
		b.MaxY = utils.Max(b.MaxY, segment[1].Y)
		return b
	}, box{MinX: sourceX, MaxX: sourceX, MinY: 0, MaxY: 0}) // {sourceX, 0} must be contained

	b.MaxY++ // Making room to fit the bottom in part 2
	height := int64(b.MaxY) - int64(b.MinY) + 1

	// Making room to let sand fall off the sides
	b.MinX -= int(height - 1)
	b.MaxX += int(height - 1)
	width := int64(b.MaxX) - int64(b.MinX) + 1

	world = make([][]Cell, height)
	algo.Foreach(world, func(row *[]Cell) { *row = make([]Cell, width) })

	// Filling world
	offset := b.MinX
	algo.Foreach(segments, func(s *[2]Location) {
		// Exploiting fact that segments go from small to big
		s[0].X -= offset
		s[1].X -= offset
		dx := algo.Sign(s[1].X - s[0].X)
		dy := algo.Sign(s[1].Y - s[0].Y)
		l := s[0]
		for l.X <= s[1].X && l.Y <= s[1].Y {
			world[l.Y][l.X] = Rock
			l.X += dx
			l.Y += dy
		}
	})

	return world, offset
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return err
	}

	world, boundingBox := AssembleWorld(input, SourceX)

	count1, err := Part1(world, boundingBox)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", count1)

	count2, err := Part2(world, boundingBox, count1)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", count2)

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

		for _, tok := range tokens[1:] {
			var newloc Location
			_, err := fmt.Sscanf(tok, "%d,%d", &newloc.X, &newloc.Y)
			if err != nil {
				return nil, fmt.Errorf("Failed to parse point %q in line %q: %v", tokens[0], sc.Text(), err)
			}

			localSegments = append(localSegments, [2]Location{prev, newloc})
			latest := &localSegments[len(localSegments)-1]
			// Swapping: segments go from small to big
			if latest[0].X > latest[1].X || latest[0].Y > latest[1].Y {
				latest[0], latest[1] = latest[1], latest[0]
			}

			prev = newloc
		}

		segments = append(segments, localSegments...)
	}

	if err := sc.Err(); err != nil {
		return segments, err
	}

	return segments, nil
}
