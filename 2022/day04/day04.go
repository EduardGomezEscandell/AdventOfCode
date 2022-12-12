// Package day04 solves day 4 of AoC2022.
package day04

import (
	"context"
	"fmt"
	"io"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 4
	fileName = "input.txt"
)

// solve solves day 4's problem. It reads line by line and evaluates
// parseLine on each line. If it returns true, the count is increased
// by one.
func solve(dataChannel <-chan input.Line, parseLine func(input.Line) bool) (r uint, err error) {
	// Managing errors
	defer func() {
		rec := recover()
		if rec != nil {
			err = recoverError(rec)
		}
	}()

	// Parsing input
	overlaps := charray.Map(dataChannel, parseLine)
	return charray.Reduce(overlaps, fun.Count[uint], 0), nil
}

// Part1 solves the first half of the problem.
func Part1(dataChannel <-chan input.Line) (uint, error) {
	return solve(dataChannel, lineContainsFullOverlap)
}

// Part2 solves the second half of the problem.
func Part2(dataChannel <-chan input.Line) (uint, error) {
	return solve(dataChannel, lineContainsOverlap)
}

// -------------- Implementation ----------------------

func lineContainsFullOverlap(line input.Line) bool {
	if err := line.Err(); err != nil {
		panic(err)
	}

	elf1 := [2]int{}
	elf2 := [2]int{}
	_, err := fmt.Sscanf(line.Str(), "%d-%d,%d-%d", &elf1[0], &elf1[1], &elf2[0], &elf2[1])
	if err != nil {
		panic(err)
	}

	start := fun.Min(elf1[0], elf2[0])
	finish := fun.Max(elf1[1], elf2[1])

	if start == elf1[0] && finish == elf1[1] {
		return true
	}
	if start == elf2[0] && finish == elf2[1] {
		return true
	}
	return false
}

func lineContainsOverlap(line input.Line) bool {
	if err := line.Err(); err != nil {
		panic(err)
	}

	elf1 := [2]int{}
	elf2 := [2]int{}
	_, err := fmt.Sscanf(line.Str(), "%d-%d,%d-%d", &elf1[0], &elf1[1], &elf2[0], &elf2[1])
	if err != nil {
		panic(err)
	}

	if elf1[0] <= elf2[0] && elf2[0] <= elf1[1] {
		return true // Elf2 starts halfway through elf1's work
	}

	if elf2[0] <= elf1[0] && elf1[0] <= elf2[1] {
		return true // Elf1 starts halfway through elf2's work
	}

	return false
}

func recoverError(recovered any) error {
	if recovered == nil {
		return nil
	}
	e, ok := recovered.(error)
	if !ok {
		return fmt.Errorf("panicked: %v", recovered)
	}
	return e
}

// ------------- Here be boilerplate ------------------

type problemResult struct {
	id  int
	res string
	err error
}

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	reader, err := DataReader()
	if err != nil {
		return err
	}

	ch, err := input.ReadDataAsync(ctx, reader, 3) // Reading in groups of three to optimize 2nd part
	if err != nil {
		return err
	}

	channels := channel.Split(ctx, ch, 2)

	resultCh := make(chan problemResult)
	go func() {
		result, err := Part1(channels[0])
		channel.Exhaust(channels[0])
		if err != nil {
			cancel()
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), nil}
	}()

	go func() {
		result, err := Part2(channels[1])
		channel.Exhaust(channels[1])
		if err != nil {
			resultCh <- problemResult{1, "", err}
			cancel()
		}
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2: %v", result), nil}
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

// DataReader is a wrapper around input.DataFile made to be
// easily mocked.
var DataReader = func() (r io.ReadCloser, e error) {
	f, e := input.DataFile(today, fileName)
	if e != nil {
		return nil, e
	}
	return f, nil
}
