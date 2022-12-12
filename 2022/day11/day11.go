// Package day11 solves day 11 of AoC2022.
package day11

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 11
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in []Monkey) (int, error) {
	return 0, nil
}

// Part2 solves the second half of the problem.
func Part2(in []Monkey) (int, error) {
	return 0, nil
}

// ------------- Implementation ------------------

type Monkey struct {
	Id         int
	Inventory  []int
	Inspect    func(int) int
	TestValue  int
	ThrowTrue  int
	ThrowFalse int
}

func NewMonkey(id int) Monkey {
	return Monkey{
		Id:         id,
		Inventory:  []int{},
		Inspect:    func(int) int { panic(fmt.Sprintf("No function for monkey #%d", id)) },
		TestValue:  1,
		ThrowTrue:  id,
		ThrowFalse: id,
	}
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	inp, err := ParseInput()
	if err != nil {
		return err
	}

	result, err := Part1(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %v\n", result)

	result, err = Part2(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %v\n", result)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ParseInput reads the data file and returns the list of
// reindeers and their calories.
func ParseInput() ([]Monkey, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	monkeys := []Monkey{}
	for i := 0; sc.Scan(); i++ {
		m, err := parseMonkey(i, sc)
		if err != nil {
			return nil, fmt.Errorf("error parsing monkey #%d: %v", i, err)
		}
		monkeys = append(monkeys, m)
	}

	if err := sc.Err(); err != nil {
		return monkeys, err
	}

	return monkeys, nil
}

func nextLine(s *bufio.Scanner) (string, error) {
	read := s.Scan()
	if !read {
		return "", errors.New("Failed to read from scanner")
	}
	return s.Text(), nil
}

func parseMonkey(id int, sc *bufio.Scanner) (Monkey, error) {
	m := NewMonkey(id)
	var err error

	// Empty line (title line in first iteration)
	line := sc.Text()

	// Title line
	if line == "" {
		line, err = nextLine(sc)
	}
	if err != nil {
		return m, err
	}

	want := fmt.Sprintf("Monkey %d:", id)
	if line != want {
		return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
	}

	if line, err = nextLine(sc); err != nil {
		return m, err
	}

	// Filling inventory
	want = "  Starting items:"
	if !strings.HasPrefix(line, want) {
		return m, fmt.Errorf("unexpected line.\nWant: %s.*\nGot : %s", want, line)
	}
	for i, s := range strings.Split(line[len(want)+1:], ", ") {
		item, err := strconv.Atoi(s)
		if err != nil {
			return m, fmt.Errorf("unexpected non-integer in position %d: %q.\nWant: %s %%d, %%d ...\nGot : %s", i, s, want, line)
		}
		m.Inventory = append(m.Inventory, item)
	}

	// Operation
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	var op rune
	var val int
	if _, err := fmt.Sscanf(line, "  Operation: new = old %c %d", &op, &val); err != nil {
		if _, err := fmt.Sscanf(line, "  Operation: new = old %c old", &op); err != nil {
			return m, fmt.Errorf("unexpected line.\nWant:   Operation: new = old %%c (%%d|old)\nGot : %s", line)
		}
		switch op {
		case '*':
			m.Inspect = func(x int) int { return x * x }
		case '+':
			m.Inspect = func(x int) int { return x + x }
		default:
			return m, fmt.Errorf("unexpected line.\nWant: Operation: new = old [*+] ...\nGot : %s", line)
		}
	} else {
		switch op {
		case '*':
			m.Inspect = func(x int) int { return x * val }
		case '+':
			m.Inspect = func(x int) int { return x + val }
		default:
			return m, fmt.Errorf("unexpected line.\nWant: Operation: new = old [*+] ...\nGot : %s", line)
		}
	}

	// Test
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	want = "  Test: divisible by %d"
	if _, err := fmt.Sscanf(line, want, &val); err != nil {
		return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
	}
	m.TestValue = val

	// If true
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	want = "    If true: throw to monkey %d"
	if _, err := fmt.Sscanf(line, want, &val); err != nil {
		return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
	}
	m.ThrowTrue = val

	// If false
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	want = "    If false: throw to monkey %d"
	if _, err := fmt.Sscanf(line, want, &val); err != nil {
		return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
	}
	m.ThrowFalse = val

	return m, nil
}
