// Package day03 solves day 2 of AoC2022	.
package day03

import (
	"bufio"
	"context"
	"errors"
	"fmt"
	"io"
	"time"
	"unicode"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 3
	fileName = "input.txt"
)

type Sack [2][]rune

// Part1 solves the first half of the problem.
func Part1(dataChannel <-chan Line) (uint, error) {
	var priorities uint
	for {
		elf, read, err := ReadElf(dataChannel)
		if !read {
			break // No more elves
		}
		if err != nil {
			return 0, err
		}

		halfIdx := len(elf) / 2
		var sack Sack
		sack[0] = elf[:halfIdx]
		sack[1] = elf[halfIdx:]

		array.Sort(sack[0], fun.Gt[rune])
		array.Sort(sack[1], fun.Gt[rune])
		c := array.Common(sack[0], sack[1], fun.Gt[rune])
		c = c[:array.Unique(c, fun.Gt[rune])]
		switch len(c) {
		case 1:
			priorities += RunePriority(c[0])
		case 0:
			return 0, fmt.Errorf("line [%s|%s] has no characters in common", string(sack[0]), string(sack[1]))
		default:
			return 0, fmt.Errorf("line [%s|%s] has too many characters in common (%s)", string(sack[0]), string(sack[1]), string(c))
		}
	}

	return priorities, nil
}

// Part2 solves the second half of the problem.
func Part2(dataChannel <-chan Line) (uint, error) {
	var priorities uint
	for {
		elves := make([][]rune, 3)
		for i := 0; i < 3; i++ {
			var read bool
			var err error
			elves[i], read, err = ReadElf(dataChannel)
			if i == 0 && !read {
				return priorities, nil // No more elves!
			}
			if err != nil {
				return 0, err
			}
			array.Sort(elves[i], fun.Lt[rune])
		}

		// Finding common runes
		common := elves[0]
		for _, elf := range elves {
			common = array.Common(common, elf, fun.Lt[rune])
		}
		common = common[:array.Unique(common, fun.Lt[rune])]

		// Deallng with results
		switch len(common) {
		case 1:
			priorities += RunePriority(common[0])
		case 0:
			return 0, fmt.Errorf("No characters in common: %s", HelperElvesToString(elves))
		default:
			return 0, fmt.Errorf("Too many characters in common: %s", HelperElvesToString(elves))
		}
	}
}

// -------------- Implementation ----------------------

func RunePriority(c rune) uint {
	if unicode.IsUpper(c) {
		return 27 + uint(c) - uint('A')
	}
	return 1 + uint(c) - uint('a')
}

func ReadElf(dataChannel <-chan Line) ([]rune, bool, error) {
	line, ok := <-dataChannel
	if !ok {
		return nil, ok, errors.New("Failed to read from channel")
	}
	if line.Err() != nil {
		return nil, true, line.Err()
	}
	return []rune(line.Str()), true, nil
}

func HelperElvesToString(elves [][]rune) string {
	return array.Reduce(elves, func(acc string, elf []rune) string {
		return fmt.Sprintf("%s\n    %s", acc, string(elf))
	})
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
	ch, err := ReadInputAsync(ctx, 3) // Reading in groups of three to optimize 2nd part
	if err != nil {
		return err
	}

	channels := channel.Split(ctx, ch, 2)

	resultCh := make(chan problemResult)
	go func() {
		result, err := Part1(channels[0])
		if err != nil {
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), nil}
	}()

	go func() {
		result, err := Part2(channels[1])
		if err != nil {
			resultCh <- problemResult{1, "", err}
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

type Line struct {
	string
	error
}

func NewLine(s string) Line {
	return Line{string: s}
}

func (l Line) Str() string {
	return l.string
}

func (l Line) Err() error {
	return l.error
}

func ReadInputAsync(ctx context.Context, channelCapacity int) (<-chan Line, error) {
	reader, err := DataReader()
	if err != nil {
		return nil, err
	}

	ch := make(chan Line, channelCapacity)

	go func() {
		defer reader.Close()
		defer close(ch)

		sc := bufio.NewScanner(reader)
		for sc.Scan() {
			ln := NewLine(sc.Text())

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
