// Package day03 solves day 2 of AoC2022	.
package day03

import (
	"bufio"
	"context"
	"fmt"
	"io"
	"time"
	"unicode"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 3
	fileName = "input.txt"
)

type Sack [2][]rune

// Part1 solves the first half of the problem.
func Part1() (uint, error) {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	ch, err := ReadInputAsync(ctx)
	if err != nil {
		return 0, err
	}

	var priorities []uint
	for line := range ch {
		if line.Err() != nil {
			return 0, line.Err()
		}

		halfIdx := len(line.Str()) / 2
		var sack Sack
		sack[0] = []rune(line.Str()[:halfIdx])
		sack[1] = []rune(line.Str()[halfIdx:])

		array.Sort(sack[0], fun.Gt[rune])
		array.Sort(sack[1], fun.Gt[rune])
		i, _ := array.Common(sack[0], sack[1], fun.Gt[rune])
		if i == -1 {
			return 0, fmt.Errorf("line [%s|%s] has no characters in common", string(sack[0]), string(sack[1]))
		}

		priorities = append(priorities, RunePriority(sack[0][i]))
	}

	return array.Reduce(priorities, fun.Add[uint]), nil

}

// // Part2 solves the second half of the problem.
// func Part2(input []Round) (uint, error) {
// 	return array.MapReduce(input, (Round).ScoreP2, fun.Add[uint]), nil
// }

// -------------- Implementation ----------------------

func RunePriority(c rune) uint {
	if unicode.IsUpper(c) {
		return 27 + uint(c) - uint('A')
	}
	return 1 + uint(c) - uint('a')
}

// ------------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {

	result, err := Part1()
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %v\n", result)

	// result, err = Part2(inp)
	// if err != nil {
	// 	return err
	// }
	// fmt.Fprintf(stdout, "Result of part 2: %v\n", result)

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

type Line struct {
	string
	error
}

func (l Line) Str() string {
	return l.string
}

func (l Line) Err() error {
	return l.error
}

func ReadInputAsync(ctx context.Context) (<-chan Line, error) {
	reader, err := DataReader()
	if err != nil {
		return nil, err
	}

	ch := make(chan Line)

	go func() {
		defer reader.Close()
		defer close(ch)

		sc := bufio.NewScanner(reader)
		for sc.Scan() {
			ln := Line{string: sc.Text()}

			select {
			case <-ctx.Done():
				ch <- Line{error: ctx.Err()}
				return
			case ch <- ln:
			}
		}

		if sc.Err() != nil {
			ch <- Line{error: ctx.Err()}
		}
	}()

	return ch, nil
}
