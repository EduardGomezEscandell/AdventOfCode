// Package day15 solves day 15 of AoC2022.
package day15

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 15
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1([]Sensor, []Beacon) (int, error) {
	return 0, nil
}

// Part2 solves the second half of the problem.
func Part2([]Sensor, []Beacon) (int, error) {
	return 0, nil
}

// ------------- Implementation ------------------.

type id uint

type Beacon struct {
	ID   id
	X, Y int
}

type Sensor struct {
	X, Y    int
	detects id
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	scanners, beacons, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(scanners, beacons)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(scanners, beacons)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and returns the list of
// reindeers and their calories.
func ReadData() ([]Sensor, []Beacon, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	sensors := []Sensor{}
	beacons := []Beacon{}
	for sc.Scan() {
		// TODO
	}

	if err := sc.Err(); err != nil {
		return sensors, beacons, err
	}

	return sensors, beacons, nil
}
