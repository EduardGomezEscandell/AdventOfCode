// package main executes the advent of code problem specified by the command line
package main

import (
	"flag"
	"fmt"
	"io"
	"log"
	"os"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day00"
)

var day uint
var output string

func main() {
	flag.UintVar(&day, "day", 0, "Day to run")
	flag.StringVar(&output, "result", "stdout", "Output")

	Main, err := entryPoint(day)
	if err != nil {
		log.Fatalf("%v", err)
	}

	w, err := getWriter(output)
	if err != nil {
		log.Fatalf("%v", err)
	}
	defer func() {
		err := w.Close()
		log.Printf("Failed to close outfile: %v", err)
	}()

	err = Main(w)
	if err != nil {
		log.Fatalf("%v", err)
	}
}

func entryPoint(day uint) (func(io.Writer) error, error) {
	switch day {
	case 0:
		return day00.Main, nil
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
