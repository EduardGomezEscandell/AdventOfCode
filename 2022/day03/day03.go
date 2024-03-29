// Package day03 solves day 2 of AoC2022	.
package day03

import (
	"context"
	"errors"
	"fmt"
	"io"
	"time"
	"unicode"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 3
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(dataChannel <-chan input.Line) (uint, error) {
	var priorities uint
	for {
		elf, read, err := readElf(dataChannel)
		if !read {
			break // No more elves
		}
		if err != nil {
			return 0, err
		}

		halfIdx := len(elf) / 2
		var sack [2][]rune
		sack[0] = elf[:halfIdx]
		sack[1] = elf[halfIdx:]

		algo.Sort(sack[0], utils.Gt[rune])
		algo.Sort(sack[1], utils.Gt[rune])
		c := algo.Intersect(sack[0], sack[1], utils.Gt[rune])
		c = c[:algo.Unique(c, utils.Eq[rune])]
		switch len(c) {
		case 1:
			priorities += runePriority(c[0])
		case 0:
			return 0, fmt.Errorf("line [%s|%s] has no characters in common", string(sack[0]), string(sack[1]))
		default:
			return 0, fmt.Errorf("line [%s|%s] has too many characters in common (%s)", string(sack[0]), string(sack[1]), string(c))
		}
	}

	return priorities, nil
}

// Part2 solves the second half of the problem.
func Part2(dataChannel <-chan input.Line) (uint, error) {
	var priorities uint
	for {
		elves := make([][]rune, 3)
		for i := 0; i < 3; i++ {
			var read bool
			var err error
			elves[i], read, err = readElf(dataChannel)
			if i == 0 && !read {
				return priorities, nil // No more elves!
			}
			if err != nil {
				return 0, err
			}
			algo.Sort(elves[i], utils.Lt[rune])
		}

		// Finding common runes
		common := elves[0]
		for _, elf := range elves[1:] {
			common = algo.Intersect(common, elf, utils.Lt[rune])
		}
		common = common[:algo.Unique(common, utils.Eq[rune])]

		// Deallng with results
		switch len(common) {
		case 1:
			priorities += runePriority(common[0])
		case 0:
			return 0, fmt.Errorf("No characters in common: %s", helperElvesToString(elves))
		default:
			return 0, fmt.Errorf("Too many characters in common: %s", helperElvesToString(elves))
		}
	}
}

// -------------- Implementation ----------------------

func runePriority(c rune) uint {
	if unicode.IsUpper(c) {
		return 27 + uint(c) - uint('A')
	}
	return 1 + uint(c) - uint('a')
}

func readElf(dataChannel <-chan input.Line) ([]rune, bool, error) {
	line, ok := <-dataChannel
	if !ok {
		return nil, ok, errors.New("Failed to read from channel")
	}
	if line.Err() != nil {
		return nil, true, line.Err()
	}
	return []rune(line.Str()), true, nil
}

func helperElvesToString(elves [][]rune) string {
	return algo.Reduce(elves, func(acc string, elf []rune) string {
		return fmt.Sprintf("%s\n    %s", acc, string(elf))
	}, "")
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
		}
		channel.Exhaust(channels[0])
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), err}
	}()

	go func() {
		result, err := Part2(channels[1])
		if err != nil {
			cancel()
		}
		channel.Exhaust(channels[1])
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2: %v", result), err}
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
