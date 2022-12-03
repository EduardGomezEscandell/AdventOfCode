// package main executes the advent of code problem specified by the command line
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day00"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day01"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day02"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/day03"
)

func main() {
	var day uint
	var output string
	var countDays bool

	flag.UintVar(&day, "day", 0, "Day to run")
	flag.StringVar(&output, "result", "stdout", "Output")
	flag.BoolVar(&countDays, "count-days", false, "print the number of days available")

	flag.Parse()

	if countDays {
		fmt.Printf("%d\n", CountDays())
		return
	}

	err := Run(day, output)
	if err != nil {
		log.Fatalf("%v", err)
	}
}

// CountDays counts the number of days that are implemented.
func CountDays() uint {
	return uint(len(entrypoints))
}

// Run runs the main program. Extacted away from main() so
// that it can be tested.
func Run(day uint, output string) error {
	entrypoint, err := entryPoint(day)
	if err != nil {
		return err
	}

	w, err := getWriter(output)
	if err != nil {
		return err
	}
	defer func() {
		err := w.Close()
		if err != nil {
			log.Printf("Failed to close outfile: %v", err)
		}
	}()

	fmt.Fprintf(w, "DAY %d\n", day)
	return entrypoint(w)
}

func entryPoint(day uint) (func(io.Writer) error, error) {
	if day > CountDays() {
		return nil, fmt.Errorf("Day %d is not implemented", day)
	}
	return entrypoints[day], nil
}

var entrypoints = []func(io.Writer) error{
	day00.Main,
	day01.Main,
	day02.Main,
	day03.Main,
}

func getWriter(output string) (io.WriteCloser, error) {
	switch output {
	case "stdout":
		return os.Stdout, nil
	case "stderr":
		return os.Stderr, nil
	}

	return os.Create(output) // nolint: gosec
}
