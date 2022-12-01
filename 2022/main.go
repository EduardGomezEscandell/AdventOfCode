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
)

func main() {
	var day uint
	var output string

	flag.UintVar(&day, "day", 0, "Day to run")
	flag.StringVar(&output, "result", "stdout", "Output")

	flag.Parse()

	err := Run(day, output)
	if err != nil {
		log.Fatalf("%v", err)
	}
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
	switch day {
	case 0:
		return day00.Main, nil
	case 1:
		return day01.Main, nil
	}
	return nil, fmt.Errorf("Day %d is not implemented", day)
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
