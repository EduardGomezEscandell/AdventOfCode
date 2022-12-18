// Package day18 solves day 18 of AoC2022.
package day18

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 18
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(world []Cube) (int, error) {
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

	countFaces := func(faces map[Point]int) (int, error) {
		var n int
		for face, count := range faces {
			switch count {
			case 0:
				return 0, fmt.Errorf("Face %v had zero hits but is in dictionary", face)
			case 1:
				n++
			case 2:
				continue
			default:
				return 0, fmt.Errorf("Face %v had more than two hits (%d)", face, count)
			}
		}
		return n, nil
	}

	var n int
	var c int
	var err error
	if c, err = countFaces(ufaces); err != nil {
		return 0, err
	}
	n += c

	if c, err = countFaces(vfaces); err != nil {
		return 0, err
	}
	n += c

	if c, err = countFaces(wfaces); err != nil {
		return 0, err
	}
	n += c

	return n, nil
}

// Part2 solves the second half of today's problem.
func Part2() (int, error) {
	return 1, nil
}

// ------------ Implementation ---------------------

// Point is a location in 3D space.
type Point struct {
	X, Y, Z int
}

// Cube contains the coordinate at the center of a cube.
type Cube Point

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

	p2, err := Part2()
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
