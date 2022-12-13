// package main executes the advent of code problem specified by the command line
package main

import (
	"errors"
	"flag"
	"fmt"
	"io"
	"os"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day00"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day01"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day02"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day03"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day04"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day05"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day06"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day07"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day08"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day09"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day10"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day11"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day12"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day13"
)

func main() {
	var day uint
	var output string
	var countDays bool
	var timed bool
	var everyDay bool

	flag.UintVar(&day, "day", CountDays(), "Run a single day. If unspecified, the latest day will run.")
	flag.StringVar(&output, "output", "stdout", "Where to print the output (can be stdout, stderr or a filename).")
	flag.BoolVar(&countDays, "count-days", false, "Print the number of days available and exit.")
	flag.BoolVar(&timed, "time", false, "Run the program many times to get an average execution time, ignoring the results.")
	flag.BoolVar(&everyDay, "all", false, "Run all days. This flag overrides -day.")

	flag.Parse()

	// Getting the writer
	w, err := getWriter(output)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error in writer: %v\n", err)
		os.Exit(1)
	}
	defer w.Close()

	if countDays {
		fmt.Fprintf(w, "%d\n", CountDays())
		os.Exit(0)
	}

	// Choosing runner
	runner := RunDay
	if timed {
		runner = TimeDay
	}

	// Running
	if everyDay {
		err := RunAll(w, runner)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Error running all: %v\n", err)
			os.Exit(1)
		}
		os.Exit(0)
	}

	_, err = runner(day, w)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error running single: %v\n", err)
		os.Exit(1)
	}
}

// CountDays counts the number of days that are implemented.
func CountDays() uint {
	return uint(len(entrypoints)) - 1
}

// RunAll runs every implemented day.
func RunAll(w io.Writer, runner func(uint, io.Writer) (time.Duration, error)) error {
	fmt.Fprintf(w, "Advent of Code 2022\n--------------------\n")
	var acc time.Duration
	for i := uint(1); i <= CountDays(); i++ {
		d, err := runner(i, w)
		if err != nil {
			return err
		}
		acc += d
	}
	if acc > 0 {
		fmt.Fprintln(w, "-------------------")
		fmt.Fprintf(w, "Total   %7d µs\n", acc/time.Microsecond)

	}
	return nil
}

// TimeDay runs the solution for a particular day and
// coputes an average solving time.
func TimeDay(day uint, w io.Writer) (time.Duration, error) {
	entrypoint, err := entryPoint(day)
	if err != nil {
		return 0, fmt.Errorf("day %2d: %v", day, err)
	}

	t, err := timedRun(entrypoint)
	if err != nil {
		return t, fmt.Errorf("day %2d: %v", day, err)
	}
	fmt.Fprintf(w, "Day %2d: %7d µs\n", day, t/time.Microsecond)
	return t, nil
}

// RunDay runs the solution for a particular day.
func RunDay(day uint, w io.Writer) (time.Duration, error) {
	entrypoint, err := entryPoint(day)
	if err != nil {
		return 0, fmt.Errorf("day %2d: %v", day, err)
	}

	fmt.Fprintf(w, "DAY %d\n", day)
	defer fmt.Fprintln(w)
	return 0, entrypoint(w)
}

func entryPoint(day uint) (func(io.Writer) error, error) {
	if day > CountDays() {
		return nil, errors.New("not implemented")
	}
	return entrypoints[day], nil
}

var entrypoints = []func(io.Writer) error{
	day00.Main,
	day01.Main,
	day02.Main,
	day03.Main,
	day04.Main,
	day05.Main,
	day06.Main,
	day07.Main,
	day08.Main,
	day09.Main,
	day10.Main,
	day11.Main,
	day12.Main,
	day13.Main,
}

func getWriter(output string) (io.WriteCloser, error) {
	switch output {
	case "stdout":
		return os.Stdout, nil
	case "stderr":
		return os.Stderr, nil
	case "null":
		return os.Stderr, nil
	}

	return os.Create(output) // nolint: gosec
}

func timedRun(f func(io.Writer) error) (time.Duration, error) {
	w, err := os.OpenFile(os.DevNull, os.O_WRONLY, 0) // Throwing away stdout
	if err != nil {
		return 0, err
	}

	var acc time.Duration
	n := 100
	for i := 0; i < n; i++ {
		start := time.Now()
		err = f(w)
		acc += time.Since(start)
		if err != nil {
			return 0, err
		}
	}
	return time.Duration(int64(acc) / int64(n)), nil
}
