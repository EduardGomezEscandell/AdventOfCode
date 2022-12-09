// Package day09 solves day 9 of AoC2022.
package day09

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
	today    = 9
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in <-chan input.Line) (int, error) {
	var head Vec
	var tail Vec

	visits := map[Vec]struct{}{
		{0, 0}: {},
	} // Using a map as a set

	for ln := range in {
		direction, dist, e := parseInstruction(ln)
		if e != nil {
			return 0, e
		}
		for step := 0; step < dist; step++ {
			oldhead := head
			head.Add(direction)
			if head.Adjacent(tail) {
				continue
			}
			tail = oldhead
			visits[tail] = struct{}{}
		}
	}

	return len(visits), nil
}

// Part2 solves the second half of the problem.
// Part1 solves the first half of the problem.
func Part2(in <-chan input.Line) (int, error) {
	visits := map[Vec]struct{}{
		{0, 0}: {},
	} // Using a map as a set

	var rope [10]Vec

	for ln := range in {
		direction, dist, e := parseInstruction(ln)
		if e != nil {
			return 0, e
		}
		for step := 0; step < dist; step++ {
			rope[0].Add(direction)
			for i := range rope[1:] {
				lead := &rope[i]
				trail := &rope[i+1]
				if lead.Adjacent(*trail) {
					break
				}
				diff := *lead
				diff.Sub(*trail)
				trail.Add(diff.Unit())
			}
			visits[rope[len(rope)-1]] = struct{}{}
			// PrintBoard(rope[:])
		}
	}

	return len(visits), nil
}

// ------------ Debug -------------

func PrintBoard(rope []Vec) {
	size := 15
	for y := size; y >= -size; y-- {
		for x := -size; x <= size; x++ {
			if x == 0 && y == 0 {
				fmt.Print("s")
				continue
			}
			idx := -1
			for i, r := range rope {
				if r.x == x && r.y == y {
					idx = i
					break
				}
			}
			if idx != -1 {
				fmt.Printf("%d", idx)
				continue
			}

			fmt.Print(".")

		}
		fmt.Println()
	}
	fmt.Println("-------------------------------")
}

// ------------- Implementation ------------------

var (
	up    Vec = Vec{0, 1}
	down  Vec = Vec{0, -1}
	left  Vec = Vec{-1, 0}
	right Vec = Vec{1, 0}
)

type Vec struct {
	x int
	y int
}

func (v *Vec) Add(w Vec) {
	v.x += w.x
	v.y += w.y
}

func (v *Vec) Sub(w Vec) {
	v.x -= w.x
	v.y -= w.y
}

func (v *Vec) Adjacent(w Vec) bool {
	return fun.Abs(v.x-w.x) < 2 && fun.Abs(v.y-w.y) < 2
}

// NormL1 is the taxicab norm of the vector
func (v Vec) NormL1() int {
	return v.x + v.y
}

func (v Vec) Unit() Vec {
	return Vec{
		x: fun.Sign(v.x),
		y: fun.Sign(v.y),
	}
}

func parseInstruction(ln input.Line) (Vec, int, error) {
	if e := ln.Err(); e != nil {
		return Vec{}, 0, e
	}
	var direction rune
	var distance int
	_, err := fmt.Sscanf(ln.Str(), "%c %d", &direction, &distance)
	if err != nil {
		return Vec{}, 0, fmt.Errorf("could not parse %s: %v", ln.Str(), err)
	}
	switch direction {
	case 'U':
		return up, distance, nil
	case 'D':
		return down, distance, nil
	case 'R':
		return right, distance, nil
	case 'L':
		return left, distance, nil
	}
	return Vec{}, 0, fmt.Errorf("could not parse %s: unknown direction %c", ln.Str(), direction)
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
			resultCh <- problemResult{0, "", err}
			cancel()
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
