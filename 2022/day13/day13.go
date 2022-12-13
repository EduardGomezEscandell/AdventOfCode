// Package day13 solves day 13 of AoC2022.
package day13

import (
	"context"
	"errors"
	"fmt"
	"io"
	"strings"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 13
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in <-chan input.Line) (int, error) {
	ch := parseInput(in)
	var idx int
	r := charray.Reduce(ch, func(acc int, t [2]item) int {
		idx++
		if compItems(t[0], t[1]) == gt {
			return acc
		}
		return acc + idx
	}, 0)

	return r, nil
}

// Part2 solves the second half of the problem.
func Part2(in <-chan input.Line) (int, error) {
	// arr := array.FromChannel(parseInput(in))
	// var err error
	// extraInput := []item{nil, nil}
	// if extraInput[0], err = Parse("[[2]]"); err != nil {
	// 	return 0, err
	// }
	// if extraInput[1], err = Parse("[[6]]"); err != nil {
	// 	return 0, err
	// }
	// arr = append(arr, extraInput)

	channel.Exhaust(in)
	return 0, nil
}

// ------------- Implementation ------------------
type item interface {
}

type Comp int

const (
	lt Comp = iota - 1
	eq
	gt
)

func compItems(a, b item) Comp {
	x, aInt := a.(int)
	y, bInt := b.(int)
	u, aList := a.([]item)
	v, bList := b.([]item)

	switch {
	case aInt && bInt:
		return compInts(x, y)
	case aInt && bList:
		return compSlices([]item{a}, v)
	case aList && bInt:
		return compSlices(u, []item{b})
	case aList && bList:
		return compSlices(u, v)
	}

	errMsg := "invalid arguments:"
	if !(aInt || aList) {
		errMsg += "\n- a is not valid token"
	}
	if !(bInt || bList) {
		errMsg += "\n- b is not valid token"
	}
	panic(errors.New(errMsg))
}

func compInts(a, b int) Comp {
	switch {
	case a < b:
		return lt
	case a > b:
		return gt
	default:
		return eq
	}
}

func compSlices(u, v []item) Comp {
	size := fun.Min(len(u), len(v))
	for i := 0; i < size; i++ {
		c := compItems(u[i], v[i])
		if c != eq {
			return c
		}
	}
	return compInts(len(u), len(v))
}

func parseInput(in <-chan input.Line) <-chan [2]item {
	out := make(chan [2]item, cap(in))

	go func() {
		defer close(out)
		for {
			// Reading first line in pair
			line1, ok := <-in
			if !ok {
				panic(fmt.Errorf("Failed to read first line"))
			}
			if err := line1.Err(); err != nil {
				panic(err)
			}
			t1, err := Parse(line1.Str())
			if err != nil {
				panic(fmt.Errorf("failed to parse first line: %v", err))
			}

			// Reading second line in pair
			line2, ok := <-in
			if !ok {
				panic(fmt.Errorf("Failed to read second line"))
			}
			if err := line2.Err(); err != nil {
				panic(err)
			}
			t2, err := Parse(line2.Str())
			if err != nil {
				panic(fmt.Errorf("failed to parse second line: %v", err))
			}

			// Channeling
			out <- [2]item{t1, t2}

			// Reading empty line to know if we're done
			_, ok = <-in
			if !ok {
				return
			}
		}
	}()

	return out
}

func Parse(s string) (item, error) {
	last := len(s) - 1
	if s[0] != '[' || s[last] != ']' {
		return nil, fmt.Errorf(`wrong format. Expected "[...]", got %q`, s)
	}
	var i int
	t, err := parse(s[1:], &i)
	if err != nil {
		return nil, fmt.Errorf("failed parsing string: %v\nPartial result: %s", err, PrettyPrint(t))
	}
	return t, nil
}

func parse(s string, i *int) (item, error) {
	root := []item{}
	var num *int
	for *i < len(s) {
		char := s[*i]
		*i++
		switch char {
		case '[':
			if num != nil {
				return nil, fmt.Errorf("error: string %q, character i=%d: number %d succeeded by ']'. Expected comma or another digit.", s, *i-1, *num)
			}
			x, err := parse(s, i)
			if err != nil {
				return root, err
			}
			root = append(root, x)
		case ']':
			if num != nil {
				root = append(root, *num)
			}
			return root, nil
		case ',':
			if num != nil {
				root = append(root, *num)
			}
			num = nil
		default:
			if num == nil {
				num = new(int)
			}
			d, err := charToInt(char)
			if err != nil {
				return root, err
			}
			*num = 10*(*num) + d
		}
	}
	return root, errors.New("unexpected end of string without closing ']'")
}

func nextLine(in <-chan input.Line) (string, bool, error) {
	r, ok := <-in
	if !ok {
		return "", false, nil
	}
	return r.Str(), true, r.Err()
}

func charToInt(c byte) (int, error) {
	if c > '9' || c < '0' {
		return 0, fmt.Errorf("Charcter %c (%d) is not an integer!", c, c)
	}
	return int(c - '0'), nil
}

func PrettyPrint(t item) string {
	x, aInt := t.(int)
	u, aList := t.([]item)

	switch {
	case aInt:
		return fmt.Sprintf("%d", x)
	case aList:
		return "[" + strings.Join(array.Map(u, PrettyPrint), ",") + "]"
	}
	panic(errors.New("invalid arguments: t is not valid token"))
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
