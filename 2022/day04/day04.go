// Package day04 solves day 4 of AoC2022.
package day04

import (
	"context"
	"fmt"
	"io"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 4
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(dataChannel <-chan input.Line) (uint, error) {
	var count uint
	for line := range dataChannel {
		fullOverlap, err := ParseLine(line)
		if err != nil {
			return 0, err
		}
		if fullOverlap {
			count++
		}
	}
	return count, nil
}

// Part2 solves the second half of the problem.
func Part2(dataChannel <-chan input.Line) (uint, error) {
	for range dataChannel {
		// Emptying channel
	}
	return 0, nil
}

// -------------- Implementation ----------------------

func ParseLine(line input.Line) (fullOverlap bool, err error) {
	if err := line.Err(); err != nil {
		return false, err
	}

	elf1 := [2]int{}
	elf2 := [2]int{}
	_, err = fmt.Sscanf(line.Str(), "%d-%d,%d-%d", &elf1[0], &elf1[1], &elf2[0], &elf2[1])
	if err != nil {
		return false, err
	}

	start := fun.Min(elf1[0], elf2[0])
	finish := fun.Max(elf1[1], elf2[1])

	if start == elf1[0] && finish == elf1[1] {
		return true, nil
	}
	if start == elf2[0] && finish == elf2[1] {
		return true, nil
	}
	return false, nil
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
		if err != nil {
			cancel()
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), nil}
	}()

	go func() {
		result, err := Part2(channels[1])
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
