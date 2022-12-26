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
	expr, err := assembleExpression(monkeys, "root")
	if err != nil {
		return 0, err
	}

	simplify(expr)
	if expr.op != constant {
		return 0, fmt.Errorf("failed to simplify expression:\n%v", expr)
	}

	return expr.value, err
}

// Part2 solves the first half of today's problem.
func Part2(monkeys map[string]monkey) (int64, error) {
	root := monkeys["root"]
	root.expr.op = sub
	monkeys["root"] = root

	human := monkeys["humn"]
	human.expr = expr{
		op: unknown,
	}
	monkeys["humn"] = human

	expr, err := assembleExpression(monkeys, "root")
	if err != nil {
		return 0, err
	}
	simplify(expr)

	return findRoot(expr, 0), nil
}

// ------------ Implementation ---------------------

// assembleExpression takes a monkey dict and creates an Abstract Syntax Tree
// out of its jobs.
func assembleExpression(jungle map[string]monkey, name string) (*expr, error) {
	monkey, found := jungle[name]
	if !found {
		return nil, fmt.Errorf("monkey not found: %s", name)
	}
	if monkey.visited {
		return &monkey.expr, nil
	}

	if monkey.expr.op == constant {
		return &monkey.expr, nil
	}

	if monkey.expr.op == unknown {
		return &monkey.expr, nil
	}

	monkey.expr.children = make([]*expr, len(monkey.children))

	var err error
	for idx := range monkey.children {
		monkey.expr.children[idx], err = assembleExpression(jungle, monkey.children[idx])
		if err != nil {
			return nil, fmt.Errorf("called from %s:\n%v", name, err)
		}
	}

	monkey.visited = true
	return &monkey.expr, nil
}

func findRoot(ex *expr, x int64) int64 {
	// Computing residual
	r := eval(ex, false, &x)
	if r == 0 {
		return x
	}

	// Computing gradient via perturbation
	delta := int64(1)
	x1 := x + delta
	r1 := eval(ex, false, &x1)
	g := (r1 - r) / float64(delta)

	if g == 0 { // Integer-float conversion error may screw us if we overshoot too far.
		panic(fmt.Errorf("Failed to converge. Solution must be close to %d (residual: %g)", x, r))
	}

	// Newton-Raphson
	next := float64(x) - r/g

	return findRoot(ex, int64(next))
}

type monkey struct {
	children [2]string
	expr     expr
	visited  bool
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input1, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}
	input2 := map[string]monkey{}
	for k, v := range input1 {
		input2[k] = v
	}

	p1, err := Part1(input1)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(input2)
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

var operators = map[rune]operator{
	'+': add,
	'-': sub,
	'*': mul,
	'/': div,
}

// ReadData reads the data file and a list of all cubes.
func ReadData() (monkeys map[string]monkey, err error) { // nolint: revive
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

			monkeys[name[:len(name)-1]] = monkey{
				expr: expr{
					op:    constant,
					value: num,
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
			operator, ok := operators[op]
			if !ok {
				return fmt.Errorf("Failed to parse line %q: invalid operator", line)
			}
			monkeys[name] = monkey{
				children: [2]string{parentL, parentR},
				expr: expr{
					op: operator,
				},
			}
			return nil
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
