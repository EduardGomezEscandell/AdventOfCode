// Package day19 solves day 19 of AoC2022.
package day19

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 19
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1([]Blueprint) (int, error) {
	return 1, nil
}

// Part2 solves the second half of today's problem.
func Part2([]Blueprint) (int, error) {
	return 1, nil
}

// ------------ Implementation ---------------------

type Blueprint struct {
	ID                int
	OreRobotCost      [3]int
	ClayRobotCost     [3]int
	ObsidianRobotCost [3]int
	GeodeRobotCost    [3]int
}

type Currency int

const (
	Ore Currency = iota
	Clay
	Obsidian
)

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
func ReadData() ([]Blueprint, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	blueprints := []Blueprint{}

	for sc.Scan() {
		t := sc.Text()
		blueprints = append(blueprints, Blueprint{})
		b := &blueprints[len(blueprints)-1]
		fmt.Sscanf(t,
			"Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.",
			&b.ID,
			&b.OreRobotCost[Ore],
			&b.ClayRobotCost[Ore],
			&b.ObsidianRobotCost[Ore],
			&b.ObsidianRobotCost[Clay],
			&b.GeodeRobotCost[Ore],
			&b.GeodeRobotCost[Obsidian],
		)
	}

	if sc.Scan() {
		return nil, errors.New("unexpected second line in input")
	}

	return blueprints, sc.Err()
}
