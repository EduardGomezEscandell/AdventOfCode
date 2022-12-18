// Package day18 solves day 18 of AoC2022.
package day18

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"math"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 18
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(world []Cube) (int, error) {
	faces := countFaceAppearances(world)
	var acc int
	for axis := range faces {
		for face, count := range faces[axis] {
			switch count {
			case 0:
				// Inaccessible code: malformed face data.
				return 0, fmt.Errorf("face %v had zero hits but is in dictionary", face)
			case 1:
				// Only one appearance: it connects one cube to the outside.
				acc++
			case 2:
				// Two appearances: it connects two cubes.
				continue
			default:
				// Impossible in 3D: probably overlapping cubes.
				return 0, fmt.Errorf("Face %v had more than two hits (%d)", face, count)
			}
		}
	}

	return acc, nil
}

// Part2 solves the second half of today's problem.
func Part2(cubeList []Cube) (int, error) {
	// Compute bounding box
	box := boundingBox(cubeList)
	xspan := box.max.X - box.min.X + 1
	yspan := box.max.Y - box.min.Y + 1
	zspan := box.max.Z - box.min.Z + 1

	// Generate domain: bounding box +1 unit in all directions.
	origin := box.min
	world := array.Generate3D(xspan, yspan, zspan, func() colour { return unpainted })

	// Two jobs:
	// - Change of coordinates so domain starts at (0,0,0)
	// - Paint cubes inside the domain with "inside" colour
	array.Foreach(cubeList, func(c *Cube) {
		c.X -= origin.X
		c.Y -= origin.Y
		c.Z -= origin.Z
		world[c.X][c.Y][c.Z] = inside
	})

	// Paint all cubes that connect to the outside with "outside" colour.
	// We start with {0,0,0} because we know it is outside the bounding box,
	// since we included that +1 unit of slack.
	paintDFS(world, Point{})

	// Bubble cubes are painted as "unpainted".
	// Inside cubes are painted as "inside".
	// Outside cubes are painted as "outside".
	var count int
	for _, cube := range cubeList {
		if world[cube.X+1][cube.Y][cube.Z] == outside {
			count++
		}
		if world[cube.X-1][cube.Y][cube.Z] == outside {
			count++
		}
		if world[cube.X][cube.Y+1][cube.Z] == outside {
			count++
		}
		if world[cube.X][cube.Y-1][cube.Z] == outside {
			count++
		}
		if world[cube.X][cube.Y][cube.Z+1] == outside {
			count++
		}
		if world[cube.X][cube.Y][cube.Z-1] == outside {
			count++
		}
	}

	return count, nil
}

// ------------ Implementation ---------------------

// Point is a location in 3D space.
type Point struct {
	X, Y, Z int
}

// Cube contains the coordinate at the center of a cube.
type Cube Point

// colour is used to paint every cube according to their location.
type colour int8

const (
	unpainted colour = iota
	inside
	outside
)

// countFaceAppearances counts how many times each face
// appears. It returns three dicts:
//
//   - Each dict contains all the faces in a particular
//     axis (normal pointing to X, Y, and Z respectively).
//
//   - The key is the location of that face. Note that the
//     coordinates for faces are shifted +{.5,.5,.5} from
//     those of the cubes. This avoids decimals.
//
//   - The value is the count of faces in that location.
func countFaceAppearances(world []Cube) [3]map[Point]int {
	ufaces := map[Point]int{}
	vfaces := map[Point]int{}
	wfaces := map[Point]int{}

	for _, c := range world {
		ufaces[Point(c)]++                 // Front
		vfaces[Point(c)]++                 // Right
		wfaces[Point(c)]++                 // Top
		ufaces[Point{c.X - 1, c.Y, c.Z}]++ // Behind
		vfaces[Point{c.X, c.Y - 1, c.Z}]++ // Left
		wfaces[Point{c.X, c.Y, c.Z - 1}]++ // Bottom
	}
	return [3]map[Point]int{ufaces, vfaces, wfaces}
}

// paintDFS explores the world painting all accessible cubes with the
// "outside" colour. It will only visit "unpainted" cubes. It will only
// paint visited cubes.
func paintDFS(world [][][]colour, root Point) {
	world[root.X][root.Y][root.Z] = outside
	xspan := len(world)
	yspan := len(world[0])
	zspan := len(world[0][0])

	if root.X > 0 && world[root.X-1][root.Y][root.Z] == unpainted {
		paintDFS(world, Point{root.X - 1, root.Y, root.Z})
	}
	if root.X+1 < xspan && world[root.X+1][root.Y][root.Z] == unpainted {
		paintDFS(world, Point{root.X + 1, root.Y, root.Z})
	}

	if root.Y > 0 && world[root.X][root.Y-1][root.Z] == unpainted {
		paintDFS(world, Point{root.X, root.Y - 1, root.Z})
	}
	if root.Y+1 < yspan && world[root.X][root.Y+1][root.Z] == unpainted {
		paintDFS(world, Point{root.X, root.Y + 1, root.Z})
	}

	if root.Z > 0 && world[root.X][root.Y][root.Z-1] == unpainted {
		paintDFS(world, Point{root.X, root.Y, root.Z - 1})
	}
	if root.Z+1 < zspan && world[root.X][root.Y][root.Z+1] == unpainted {
		paintDFS(world, Point{root.X, root.Y, root.Z + 1})
	}
}

// extrema expresses a 3D cube determined by its maximum and
// minimum coordinates.
type extrema struct {
	min Point
	max Point
}

// boundingBox returns a bounding box with 1 tile of slack
// in every direction.
func boundingBox(c []Cube) extrema {
	return array.Reduce(c, func(e extrema, c Cube) extrema {
		e.min.X = fun.Min(e.min.X, c.X-1)
		e.max.X = fun.Max(e.max.X, c.X+1)

		e.min.Y = fun.Min(e.min.Y, c.Y-1)
		e.max.Y = fun.Max(e.max.Y, c.Y+1)

		e.min.Z = fun.Min(e.min.Z, c.Z-1)
		e.max.Z = fun.Max(e.max.Z, c.Z+1)

		return e
	}, extrema{
		min: Point{math.MaxInt, math.MaxInt, math.MaxInt},
		max: Point{math.MinInt, math.MinInt, math.MinInt},
	})
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := Part1(input)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(input)
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

// ReadData reads the data file and a list of all cubes.
func ReadData() ([]Cube, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	cubes := []Cube{}

	for sc.Scan() {
		c := Cube{}
		fmt.Sscanf(sc.Text(), "%d,%d,%d", &c.X, &c.Y, &c.Z)
		cubes = append(cubes, c)
	}

	if sc.Scan() {
		return nil, errors.New("unexpected second line in input")
	}

	return cubes, sc.Err()
}
