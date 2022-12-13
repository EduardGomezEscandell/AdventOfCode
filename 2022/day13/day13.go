// Package day13 solves day 13 of AoC2022.
package day13

import (
	"context"
	"fmt"
	"io"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 13
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in <-chan input.Line) (int, error) {
	channel.Exhaust(in)
	return 0, nil
}

// Part2 solves the second half of the problem.
func Part2(in <-chan input.Line) (int, error) {
	channel.Exhaust(in)
	return 0, nil
}

// ------------- Implementation ------------------

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

	ch, err := input.ReadDataAsync(ctx, reader, 10)
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
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %d", result), err}
	}()

	go func() {
		result, err := Part2(channels[1])
		if err != nil {
			cancel()
		}
		channel.Exhaust(channels[1])
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2: %d", result), err}
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
