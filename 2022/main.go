// package main executes the advent of code problem specified by the command line
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
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
)

func main() {
	var day uint
	var output string
	var countDays bool
	var timed bool
	var everyDay bool

	flag.UintVar(&day, "day", 0, "Run a single day")
	flag.StringVar(&output, "output", "stdout", "Output")
	flag.BoolVar(&countDays, "count-days", false, "Print the number of days available and exit.")
	flag.BoolVar(&timed, "time", false, "Run the program many times to get an average execution time, ignoring the results.")
	flag.BoolVar(&everyDay, "all", false, "Run all days.")

	flag.Parse()

	if countDays {
		log.Printf("%d\n", CountDays())
		os.Exit(0)
	}

	// Getting the writer
	w, err := getWriter(output)
	if err != nil {
		log.Fatalf("Error in writer: %v", err)
	}
	defer w.Close()

	// Choosing runner
	runner := RunDay
	if timed {
		runner = TimeDay
	}

	// Running
	if everyDay {
		err := RunAll(w, runner)
		if err != nil {
			log.Fatalf("Error running all: %v", err)
		}
		os.Exit(0)
	}

	err = runner(day, w)
	if err != nil {
		log.Fatalf("Error running single day: %v", err)
	}
}

// CountDays counts the number of days that are implemented.
func CountDays() uint {
	return uint(len(entrypoints))
}

// RunAll runs every implemented day.
func RunAll(w io.Writer, runner func(uint, io.Writer) error) error {
	fmt.Fprintf(w, "Advent of Code 2022\n--------------------\n")
	for i := uint(1); i < CountDays(); i++ {
		err := runner(i, w)
		if err != nil {
			return err
		}
	}
	return nil
}

// TimeDay runs the solution for a particular day and
// coputes an average solving time.
func TimeDay(day uint, w io.Writer) error {
	entrypoint, err := entryPoint(day)
	if err != nil {
		return fmt.Errorf("day %2d: %v", day, err)
	}

	t, err := timedRun(entrypoint)
	if err != nil {
		return fmt.Errorf("day %2d: %v", day, err)
	}
	fmt.Fprintf(w, "Day %2d: %6d µs\n", day, t/time.Microsecond)
	return nil
}

// RunDay runs the solution for a particular day.
func RunDay(day uint, w io.Writer) error {
	entrypoint, err := entryPoint(day)
	if err != nil {
		return fmt.Errorf("day %2d: %v", day, err)
	}

	fmt.Fprintf(w, "DAY %d\n", day)
	defer fmt.Fprintln(w)
	return entrypoint(w)
}

func entryPoint(day uint) (func(io.Writer) error, error) {
	if day > CountDays() {
		return nil, fmt.Errorf("day %d is not implemented", day)
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
