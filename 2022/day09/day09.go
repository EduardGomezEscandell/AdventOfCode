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
	var head vec
	var tail vec

	visits := map[vec]struct{}{
		{0, 0}: {},
	} // Using a map as a set

	for ln := range in {
		direction, dist, e := parseInstruction(ln)
		if e != nil {
			return 0, e
		}
		for step := 0; step < dist; step++ {
			oldhead := head
			head.add(direction)
			if head.adjacent(tail) {
				continue
			}
			tail = oldhead
			visits[tail] = struct{}{}
		}
	}

	return len(visits), nil
}

// Part2 solves the second half of the problem.
func Part2(in <-chan input.Line) (int, error) {
	visits := map[vec]struct{}{
		{0, 0}: {},
	} // Using a map as a set

	var rope [10]vec

	for ln := range in {
		direction, dist, e := parseInstruction(ln)
		if e != nil {
			return 0, e
		}
		for step := 0; step < dist; step++ {
			rope[0].add(direction)
			for i := range rope[1:] {
				lead := &rope[i]
				trail := &rope[i+1]
				if lead.adjacent(*trail) {
					break
				}
				diff := *lead
				diff.sub(*trail)
				trail.add(diff.unit())
			}
			visits[rope[len(rope)-1]] = struct{}{}
		}
	}

	return len(visits), nil
}

// ------------- Implementation ------------------

var (
	up    = vec{0, 1}
	down  = vec{0, -1}
	left  = vec{-1, 0}
	right = vec{1, 0}
)

type vec struct {
	x int
	y int
}

func (v *vec) add(w vec) {
	v.x += w.x
	v.y += w.y
}

func (v *vec) sub(w vec) {
	v.x -= w.x
	v.y -= w.y
}

// adjacent indicates if two point vectors are at a taxicab or
// diagonal distance under 2.
func (v *vec) adjacent(w vec) bool {
	return fun.Abs(v.x-w.x) < 2 && fun.Abs(v.y-w.y) < 2
}

// unit gives a vector pointing in the same direction as the original,
// with x,y contained in {-1,0,1}.
func (v vec) unit() vec {
	return vec{
		x: fun.Sign(v.x),
		y: fun.Sign(v.y),
	}
}

func parseInstruction(ln input.Line) (vec, int, error) {
	if e := ln.Err(); e != nil {
		return vec{}, 0, e
	}
	var direction rune
	var distance int
	_, err := fmt.Sscanf(ln.Str(), "%c %d", &direction, &distance)
	if err != nil {
		return vec{}, 0, fmt.Errorf("could not parse %s: %v", ln.Str(), err)
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
	return vec{}, 0, fmt.Errorf("could not parse %s: unknown direction %c", ln.Str(), direction)
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
