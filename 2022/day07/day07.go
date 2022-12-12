// Package day07 solves day 7 of AoC2022.
package day07

import (
	"context"
	"fmt"
	"io"
	"sync"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 7
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(fs Filesystem) (int, error) {
	return int(addSmallDirectories(&fs.Root)), nil
}

// Part2 solves the second half of the problem.
func Part2(fs Filesystem) (int, error) {
	const (
		totalCapacity  = 70_000_000
		neededCapacity = 30_000_000
	)

	usedCapacity := fs.Root.Data.Size
	if usedCapacity > totalCapacity {
		return 0, fmt.Errorf("Drive is beyond full: %d/%d", usedCapacity, totalCapacity)
	}

	currentCapactiy := totalCapacity - usedCapacity
	if currentCapactiy > neededCapacity {
		return 0, nil
	}

	neededRemoval := neededCapacity - currentCapactiy
	toRemove := findFileToRemove(fs.Root, neededRemoval, fs.Root.Data)

	return int(toRemove.Size), nil
}

// / ---------- Tree exploration implementation ------------------.

// addSmallDirectories sums the sizes of all directories under
// the file being passed (the latter included). A directory is small
// if its size is bellow 100_000.
func addSmallDirectories(root *fsNode) size {
	if !root.Data.IsDir {
		return 0
	}

	ch := make(chan size)
	var wg sync.WaitGroup

	array.Foreach(root.Children, func(child **fsNode) {
		wg.Add(1)
		go func() {
			defer wg.Done()
			ch <- addSmallDirectories(*child)
		}()
	})

	go func() {
		wg.Wait()
		close(ch)
	}()
	children := charray.Reduce(ch, fun.Add[size], 0)
	if root.Data.Size > 100_000 {
		return children
	}
	return children + root.Data.Size
}

// findFileToRemove finds the smallest directory with size surpassing neededSpace.
func findFileToRemove(root fsNode, neededSpace size, currentBest *FileDescriptor) *FileDescriptor {
	// Files excluded
	if !root.Data.IsDir {
		return currentBest
	}

	// Prunning: all children will be smaller
	if root.Data.Size < neededSpace {
		return currentBest
	}

	// Updating best
	if root.Data.Size < currentBest.Size {
		currentBest = root.Data
	}

	// Exploring children asyncronously
	ch := make(chan *FileDescriptor)
	var wg sync.WaitGroup
	for _, child := range root.Children {
		child := child
		wg.Add(1)
		go func() {
			defer wg.Done()
			ch <- findFileToRemove(*child, neededSpace, currentBest)
		}()
	}

	// Synching
	go func() {
		wg.Wait()
		close(ch)
	}()

	// Collecting results
	return charray.Reduce(ch, func(a, b *FileDescriptor) *FileDescriptor {
		if a.Size < b.Size {
			return a
		}
		return b
	}, currentBest)
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

	ch, err := input.ReadDataAsync(ctx, reader, 10)
	if err != nil {
		return err
	}

	filesystem, err := ParseFilesystem(ch)
	if err != nil {
		return err
	}
	cancel()

	resultCh := make(chan problemResult)
	go func() {
		result, err := Part1(filesystem)
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), err}
	}()

	go func() {
		result, err := Part2(filesystem)
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
