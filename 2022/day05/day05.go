// Package day05 solves day 5 of AoC2022.
package day05

import (
	"context"
	"errors"
	"fmt"
	"io"
	"regexp"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/stack"
)

const (
	today    = 5
	fileName = "input.txt"
)

// Solve solves both parts of the problem. Part 1 should use Crane9000 and part 2 should use Crane9001.
func Solve(in <-chan input.Line, crane func(stacks []Stack, inst Instruction)) (s string, err error) {
	// Managing errors
	defer func() {
		err = recoverError(err, recover())
	}()

	// Getting stack
	stacks, err := ParseInitalState(in)
	if err != nil {
		return "", fmt.Errorf("error parsing stack: %v", err)
	}
	<-in // Discarding empty separator line

	// Operating
	instructions := charray.Map(in, parseInstruction)
	done := charray.Map(instructions, func(inst Instruction) struct{} {
		crane(stacks, inst)
		return struct{}{}
	})

	// Sync point
	channel.Exhaust(done)

	// Obtaining values
	top := array.MapReduce(stacks, (Stack).Peek, fun.Append[rune])
	return string(top), nil
}

// part1 solves the first half of the problem.
func part1(input <-chan input.Line) (string, error) {
	return Solve(input, Crane9000)
}

// part2 solves the second half of the problem.
func part2(input <-chan input.Line) (string, error) {
	return Solve(input, Crane9001)
}

/// ---------- Implementation ------------------

// Crane9000 performs instructions as specified in Part 1 of the problem statement.
func Crane9000(stacks []Stack, inst Instruction) {
	from := &stacks[inst.from]
	to := &stacks[inst.to]
	for i := 0; i < inst.qty; i++ {
		to.Push(from.Peek())
		from.Pop()
	}
}

// Crane9001 performs instructions as specified in Part 2 of the problem statement.
func Crane9001(stacks []Stack, inst Instruction) {
	from := &stacks[inst.from]
	intermediate := stack.New[rune](0, inst.qty)
	to := &stacks[inst.to]
	for i := 0; i < inst.qty; i++ {
		intermediate.Push(from.Peek())
		from.Pop()
	}
	for i := 0; i < inst.qty; i++ {
		to.Push(intermediate.Peek())
		intermediate.Pop()
	}
}

// Stack is syntax sugar for stack.Stack[rune].
type Stack = stack.Stack[rune]

// Instruction contains the stack moving instruction.
type Instruction struct {
	from, to, qty int
}

func parseInstruction(ln input.Line) Instruction {
	if err := ln.Err(); err != nil {
		panic(err)
	}
	var i Instruction
	_, err := fmt.Sscanf(ln.Str(), "move %d from %d to %d", &i.qty, &i.from, &i.to)
	if err != nil {
		panic(fmt.Errorf("line %s cause error: %v", ln.Str(), err))
	}
	// Converting to zero-indexing
	i.from--
	i.to--
	return i
}

// ParseInitalState reads the data file and returns the list of
// reindeers and their calories.
func ParseInitalState(input <-chan input.Line) (stacks []Stack, err error) {
	defer func() { array.Foreach(stacks, (*Stack).Invert) }()

	parseLine := func(line string) bool {
		data := array.Stride([]rune(line)[1:], 4)
		allLetters := regexp.MustCompile(`^[a-zA-Z ]*$`).MatchString(string(data))
		if !allLetters {
			return false // Must be end of stack
		}

		stacks = array.ZipWith(stacks, data, func(s Stack, r rune) Stack {
			if r != ' ' {
				s.Push(r)
			}
			return s
		})

		return true
	}

	fromInput := <-input
	if err := fromInput.Err(); err != nil {
		return stacks, nil
	}
	nstacks := (len(fromInput.Str()) + 1) / 4
	stacks = array.Generate(nstacks, func() Stack { return stack.New[rune]() })

	if !parseLine(fromInput.Str()) {
		return
	}

	for fromInput = range input {
		if err := fromInput.Err(); err != nil {
			return stacks, err
		}
		if !parseLine(fromInput.Str()) {
			return stacks, nil
		}
	}

	return stacks, errors.New("no end to stack")
}

/// ---------- Here be boilerplate ------------------

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
		result, err := part1(channels[0])
		channel.Exhaust(channels[0])
		if err != nil {
			resultCh <- problemResult{0, "", err}
			cancel()
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), nil}
	}()

	go func() {
		result, err := part2(channels[1])
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

func recoverError(original error, recovered any) error {
	if original != nil {
		return original
	}
	if recovered == nil {
		return nil
	}
	e, ok := recovered.(error)
	if !ok {
		return fmt.Errorf("panicked: %v", recovered)
	}
	return e
}
