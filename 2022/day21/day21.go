// Package day21 solves day 21 of AoC2022.
package day21

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 21
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(monkeys map[string]monkey) (int64, error) {
	for _, monkey := range monkeys {
		go monkey.eval()
	}

	return <-monkeys["root"].ch, nil
}

// Part2 solves the second half of today's problem.
func Part2(monkeys map[string]monkey) (int, error) {
	return 1, nil
}

// ------------ Implementation ---------------------

type monkey struct {
	eval func()
	ch   chan int64
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := Part1(input)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(input)
	if err != nil {
		return fmt.Errorf("error in part 2: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and a list of all cubes.
func ReadData() (monkeys map[string]monkey, err error) { // nolint: revive
	defer func() {
		if err == nil {
			return
		}
		for _, m := range monkeys {
			close(m.ch)
		}
	}()

	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	monkeys = map[string]monkey{}

	for sc.Scan() {
		line := sc.Text()

		err1 := func() error {
			var name string
			var num int64
			_, err := fmt.Sscanf(line, "%s %d", &name, &num)
			if err != nil {
				return err
			}
			if name[len(name)-1] != ':' {
				return errors.New("lead name is not followed by colon")
			}

			ch := make(chan int64)
			monkeys[name[:len(name)-1]] = monkey{
				ch: ch,
				eval: func() {
					ch <- num
					close(ch)
				},
			}
			return nil
		}()
		if err1 == nil {
			continue
		}

		err2 := func() error {
			var namePre, parentL, parentR string
			var op rune
			_, err := fmt.Sscanf(line, `%s %s %c %s`, &namePre, &parentL, &op, &parentR)
			if err != nil {
				return err
			}
			if namePre[len(namePre)-1] != ':' {
				return errors.New("lead name is not followed by colon")
			}
			name := namePre[:len(namePre)-1]

			ch := make(chan int64)
			m := monkey{}
			m.ch = ch
			switch op {
			case '+':
				m.eval = func() {
					v := <-monkeys[parentL].ch + <-monkeys[parentR].ch
					ch <- v
					close(ch)
				}
				monkeys[name] = m
				return nil
			case '-':
				m.eval = func() {
					v := <-monkeys[parentL].ch - <-monkeys[parentR].ch
					ch <- v
					close(ch)
				}
				monkeys[name] = m
				return nil
			case '*':
				m.eval = func() {
					v := <-monkeys[parentL].ch * <-monkeys[parentR].ch
					ch <- v
					close(ch)
				}
				monkeys[name] = m
				return nil
			case '/':
				m.eval = func() {
					v := <-monkeys[parentL].ch / <-monkeys[parentR].ch
					ch <- v
					close(ch)
				}
				monkeys[name] = m
				return nil
			default:
				close(ch)
				return fmt.Errorf("Failed to parse line %q: invalid operator", line)
			}
		}()
		if err2 == nil {
			continue
		}

		return nil, fmt.Errorf(`Failed to parse line %q:
 > failed to get direct value: %v
 > failed to get operation: %v`, line, err1, err2)
	}

	return monkeys, sc.Err()
}
