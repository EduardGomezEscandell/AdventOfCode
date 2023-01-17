// Package day13 solves day 13 of AoC2022.
package day13

import (
	"context"
	"errors"
	"fmt"
	"io"
	"strings"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 13
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in <-chan node) (int, error) {
	var acc int
	for idx := 1; ; idx++ {
		t1, ok := <-in
		if !ok {
			break
		}
		t2, ok := <-in
		if !ok {
			return 0, fmt.Errorf("Failed to read second line of pair #%d", idx)
		}
		if compNodes(t1, t2) == gt {
			continue
		}
		acc += idx
	}

	return acc, nil
}

// Part2 solves the second half of the problem.
func Part2(in <-chan node) (int, error) {
	arr := charray.Deserialize(in)

	// Append extra entries
	var err error
	extraInput := []node{nil, nil}
	if extraInput[0], err = Parse("[[2]]"); err != nil {
		return 0, err
	}
	if extraInput[1], err = Parse("[[6]]"); err != nil {
		return 0, err
	}

	// Sorting extra values (yes I could hard code it, whatever)
	algo.Sort(extraInput[:], func(a, b node) bool { return compNodes(a, b) == lt })

	// The index of each value is the count of entries that are beneath it in order, plus one.
	// This solution is O(N), whereas sorting would be O(N log N)
	countSmaller := algo.Reduce(arr, func(acc [2]int, x node) [2]int {
		if compNodes(x, extraInput[0]) != gt {
			return [2]int{acc[0] + 1, acc[1] + 1}
		}
		if compNodes(x, extraInput[1]) != gt {
			return [2]int{acc[0], acc[1] + 1}
		}
		return acc
	}, [2]int{0, 1}) // Initial values are {0,1} because the second extra value has the first one before it

	return (countSmaller[0] + 1) * (countSmaller[1] + 1), nil
}

// ------------- Implementation ------------------

// A node of the abstract syntax tree. Can be either
// a list of nodes, or an integer (a leaf).
type node interface{}

// comp is the result of a strict comparison.
type comp int

const (
	lt comp = iota - 1 // less than
	eq                 // equal
	gt                 // greater than
)

// compNodes compares nodes according to the
// criteria specified by the problem statement.
func compNodes(a, b node) comp {
	x, aInt := a.(int)
	y, bInt := b.(int)
	u, aList := a.([]node)
	v, bList := b.([]node)

	switch {
	case aInt && bInt:
		return compInts(x, y)
	case aInt && bList:
		return compSlices([]node{a}, v)
	case aList && bInt:
		return compSlices(u, []node{b})
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

// compSlices compares integer-nodes according to the
// criterion specified by the problem statement.
func compInts(a, b int) comp {
	switch {
	case a < b:
		return lt
	case a > b:
		return gt
	default:
		return eq
	}
}

// compSlices compares list-nodes according to the
// criteria specified by the problem statement.
func compSlices(u, v []node) comp {
	size := utils.Min(len(u), len(v))
	for i := 0; i < size; i++ {
		c := compNodes(u[i], v[i])
		if c != eq {
			return c
		}
	}
	return compInts(len(u), len(v))
}

// Parse takes a string containing the representation of the syntax tree
// in the format
//   - An node is either an integer or a list of other nodes.
//   - Lists are represented as a comma-separated lists of nodes, enclosed in [square brackets].
//   - Integers are represented in base 10
//   - The abstract syntax tree contains a root node which is always a list.
func Parse(s string) (node, error) {
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

// ParseInput takes the input channel and returns a channel
// with abstract syntax trees. Empty lines are discarded.
func ParseInput(in <-chan input.Line) <-chan node {
	out := make(chan node, cap(in))

	go func() {
		defer close(out)
		for ln := range in {
			if err := ln.Err(); err != nil {
				panic(err)
			}
			if ln.Str() == "" {
				continue
			}
			t1, err := Parse(ln.Str())
			if err != nil {
				panic(fmt.Errorf("failed to parse first line: %v", err))
			}
			out <- t1
		}
	}()

	return out
}

// parse takes a string and an index and parses the subtree from s[i] until
// the next closing bracket ']'.
func parse(s string, i *int) (node, error) {
	root := []node{}
	var num *int
	for *i < len(s) {
		char := s[*i]
		*i++
		switch char {
		case '[':
			if num != nil {
				return nil, fmt.Errorf("error: string %q, character i=%d: number %d succeeded by ']'. Expected comma or another digit", s, *i-1, *num)
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

func charToInt(c byte) (int, error) {
	if c > '9' || c < '0' {
		return 0, fmt.Errorf("character %c (%d) is not an integer", c, c)
	}
	return int(c - '0'), nil
}

// PrettyPrint converts the AST into a string in the same format as the problem statement.
func PrettyPrint(t node) string {
	x, aInt := t.(int)
	u, aList := t.([]node)

	switch {
	case aInt:
		return fmt.Sprintf("%d", x)
	case aList:
		return "[" + strings.Join(algo.Map(u, PrettyPrint), ",") + "]"
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

	channels := channel.Split(ctx, ParseInput(ch), 2)

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
