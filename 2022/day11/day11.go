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

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 11
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(monkeys []Monkey) (int, error) {
	for i := 0; i < 20; i++ {
		round(monkeys)
	}

	best2 := array.BestN(monkeys, 2, func(a, b Monkey) bool { return a.Inspections > b.Inspections })
	return best2[0].Inspections * best2[1].Inspections, nil
}

func round(monkeys []Monkey) {
	for i := range monkeys {
		monkeyTurn(i, monkeys)
	}
}

func monkeyTurn(id int, monkeys []Monkey) {
	m := &monkeys[id]
	sz := len(m.Inventory)
	for i := 0; i < sz; i++ {
		// Popping from front
		v := m.Inventory[0]
		m.Inventory = m.Inventory[1:]
		// Inspection
		v = m.Inspect(v)
		m.Inspections++
		// Relief
		v /= 3
		// Sending
		target := m.ThrowTrue
		if v%m.TestValue != 0 {
			target = m.ThrowFalse
		}
		monkeys[target].Inventory = append(monkeys[target].Inventory, v)
	}
}

// Part2 solves the second half of the problem.
func Part2(monkeys []Monkey) (int, error) {
	return 0, nil
}

// ------------- Implementation ------------------

type Monkey struct {
	Id          int
	Inventory   []uint64
	Inspect     func(uint64) uint64
	TestValue   uint64
	ThrowTrue   int
	ThrowFalse  int
	Inspections int
}

func NewMonkey(id int) Monkey {
	return Monkey{
		Id:          id,
		Inventory:   []uint64{},
		Inspect:     func(uint64) uint64 { panic(fmt.Sprintf("No function for monkey #%d", id)) },
		TestValue:   1,
		ThrowTrue:   id,
		ThrowFalse:  id,
		Inspections: 0,
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
		m.Inventory = append(m.Inventory, uint64(item))
	}

	// Operation
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	var op rune
	var val uint64
	if _, err := fmt.Sscanf(line, "  Operation: new = old %c %d", &op, &val); err != nil {
		if _, err := fmt.Sscanf(line, "  Operation: new = old %c old", &op); err != nil {
			return m, fmt.Errorf("unexpected line.\nWant:   Operation: new = old %%c (%%d|old)\nGot : %s", line)
		}
		switch op {
		case '*':
			m.Inspect = func(x uint64) uint64 { return x * x }
		case '+':
			m.Inspect = func(x uint64) uint64 { return x + x }
		default:
			return m, fmt.Errorf("unexpected line.\nWant: Operation: new = old [*+] ...\nGot : %s", line)
		}
	} else {
		val := val
		switch op {
		case '*':
			m.Inspect = func(x uint64) uint64 { return x * val }
		case '+':
			m.Inspect = func(x uint64) uint64 { return x + val }
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
	m.ThrowTrue = int(val)

	// If false
	if line, err = nextLine(sc); err != nil {
		return m, err
	}
	want = "    If false: throw to monkey %d"
	if _, err := fmt.Sscanf(line, want, &val); err != nil {
		return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
	}
	m.ThrowFalse = int(val)

	return m, nil
}
