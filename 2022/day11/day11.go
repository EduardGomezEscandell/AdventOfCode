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
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 11
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(monkeys []Monkey) (uint64, error) {
	return Solve(monkeys, 20, 3)
}

// Part2 solves the second half of the problem.
func Part2(monkeys []Monkey) (uint64, error) {
	return Solve(monkeys, 10_000, 1)
}

// Solve solves todays' problem.
func Solve(monkeys []Monkey, nrounds int, stressDivisor uint64) (uint64, error) {
	if len(monkeys) < 2 {
		return 0, errors.New("Need at least two monkeys to play")
	}
	// Computing everything mod lcd to avoid overflows
	// lcd: least common multiple of the test values of the monkeys
	tv := array.Map(monkeys, func(m Monkey) uint64 { return m.TestValue })
	lcd := LCM(tv[0], tv[1], tv[2:]...)
	// lcd := array.MapReduce(monkeys, func(m Monkey) uint64 { return m.TestValue }, fun.Mul[uint64], 1)

	for i := 0; i < nrounds; i++ {
		round(monkeys, stressDivisor, lcd)
	}

	monkeyBusiness := array.BestN(monkeys, 2, func(a, b Monkey) bool { return a.Inspections > b.Inspections })
	return monkeyBusiness[0].Inspections * monkeyBusiness[1].Inspections, nil
}

// ------------- Implementation ------------------

func round(monkeys []Monkey, stressDivisor, stressMod uint64) {
	for i := range monkeys {
		turn(monkeys, i, stressDivisor, stressMod)
	}
}

func turn(monkeys []Monkey, turn int, stressDivisor, stressMod uint64) {
	m := &monkeys[turn]
	m.Inspections += uint64(len(m.Inventory))

	queue := []uint64{}
	m.Inventory, queue = queue, m.Inventory

	for _, x := range queue {
		v := m.A*x*x + m.B*x + m.C
		if v <= x {
			fmt.Println("Overflow!!!")
		}
		v /= stressDivisor
		v %= stressMod

		t := m.ThrowTrue
		if v%m.TestValue != 0 {
			t = m.ThrowFalse
		}
		monkeys[t].Inventory = append(monkeys[t].Inventory, v)
	}
}

// GCD computes the greatest common divisor (GCD) via Euclidean algorithm.
func GCD(a, b uint64) uint64 {
	for b != 0 {
		t := b
		b = a % b
		a = t
	}
	return a
}

// LCM finds the Least Common Multiple (LCM) via GCD.
func LCM(a, b uint64, integers ...uint64) uint64 {
	result := a * b / GCD(a, b)

	for i := 0; i < len(integers); i++ {
		result = LCM(result, integers[i])
	}

	return result
}

// Monkey as in the monkeys in the problem statement.
type Monkey struct {
	ID          int      // Id of the monkey
	Inventory   []uint64 // Items held by the monkey
	A           uint64   // A inspect coefficient: y = Axx + Bx + C
	B           uint64   // B inspect coefficient: y = Axx + Bx + C
	C           uint64   // C inspect coefficient: y = Axx + Bx + C
	TestValue   uint64   // Value to test divisibility
	ThrowTrue   int      // Where to throw items when divisible
	ThrowFalse  int      // Where to throw items when non-divisible
	Inspections uint64   // Number of objects inspected
}

func (old Monkey) copy() Monkey {
	m := old
	m.Inventory = array.Map(old.Inventory, fun.Identity[uint64])
	return m
}

// NewMonkey creates a new monkey.
func NewMonkey(id int) Monkey {
	return Monkey{
		ID:        id,
		Inventory: []uint64{},
	}
}

// ---------- Here be boilerplate ------------------

type problemResult struct {
	id  int
	res string
	err error
}

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input1, err := ParseInput()
	input2 := array.Map(input1, (Monkey).copy)

	resultCh := make(chan problemResult)
	go func() {
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part1(input1)
		if err != nil {
			resultCh <- problemResult{0, "", err}
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %d", result), nil}
	}()

	go func() {
		if err != nil {
			resultCh <- problemResult{0, "", err}
			return
		}
		result, err := Part2(input2)
		if err != nil {
			resultCh <- problemResult{1, "", err}
			return
		}
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2: %d", result), nil}
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

	// Title line
	{
		var err error
		line := sc.Text()
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
	}

	// Filling inventory
	{
		line, err := nextLine(sc)
		if err != nil {
			return m, err
		}
		want := "  Starting items:"
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
	}

	// Operation
	{
		line, err := nextLine(sc)
		if err != nil {
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
				m.A = 1
			case '+':
				m.B = 2
			default:
				return m, fmt.Errorf("unexpected line.\nWant: Operation: new = old [*+] ...\nGot : %s", line)
			}
		} else {
			switch op {
			case '*':
				m.B = val
			case '+':
				m.B = 1
				m.C = val
			default:
				return m, fmt.Errorf("unexpected line.\nWant: Operation: new = old [*+] ...\nGot : %s", line)
			}
		}
	}

	// Test
	{
		line, err := nextLine(sc)
		if err != nil {
			return m, err
		}
		want := "  Test: divisible by %d"
		var val uint64
		if _, err := fmt.Sscanf(line, want, &val); err != nil {
			return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
		}
		m.TestValue = val
	}

	// If true
	{
		line, err := nextLine(sc)
		if err != nil {
			return m, err
		}
		want := "    If true: throw to monkey %d"
		var val int
		if _, err := fmt.Sscanf(line, want, &val); err != nil {
			return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
		}
		m.ThrowTrue = val
	}

	// If false
	{
		line, err := nextLine(sc)
		if err != nil {
			return m, err
		}
		var val int
		want := "    If false: throw to monkey %d"
		if _, err := fmt.Sscanf(line, want, &val); err != nil {
			return m, fmt.Errorf("unexpected line.\nWant: %s\nGot : %s", want, line)
		}
		m.ThrowFalse = val
	}

	return m, nil
}
