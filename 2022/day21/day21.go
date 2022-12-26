// Package day21 solves day 21 of AoC2022.
package day21

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
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
	if expr.op != eq {
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
	simplify(expr.children[0])
	simplify(expr.children[1])
	fmt.Println(expr)

	return 1, nil
}

// ------------ Implementation ---------------------

func (ex expr) String() string {
	operands := strings.Join(array.Map(ex.children, func(ch *expr) string { return ch.String() }), ", ")

	switch ex.op {
	case eq:
		return fmt.Sprintf("%d", ex.value)
	case unknown:
		return fmt.Sprintf("UNKNOWN")
	case add:
		return fmt.Sprintf("+(%s)", operands)
	case sub:
		return fmt.Sprintf("-(%s)", operands)
	case mul:
		return fmt.Sprintf("*(%s)", operands)
	case div:
		return fmt.Sprintf("÷(%s)", operands)
	default:
		panic("unreachable")
	}
}

func assembleExpression(jungle map[string]monkey, name string) (*expr, error) {
	monkey, found := jungle[name]
	if !found {
		return nil, fmt.Errorf("monkey not found: %s", name)
	}
	if monkey.visited {
		return &monkey.expr, nil
	}

	if monkey.expr.op == eq {
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

func simplify(ex *expr) {
	if len(ex.children) == 0 {
		return
	}
	array.Foreach(ex.children, func(ch **expr) { simplify(*ch) })

	// If they all contain direct values, we operate
	tryOperate(ex)

	// If it is a subtraction, we invert it
	tryInvertSubtraction(ex)

	// Disassembling products
	tryDistribute(ex)

	// If children all contain commutative operations, we create a larger commutative operation
	tryCombineCommutative(ex, add)
	tryCombineCommutative(ex, mul)

	// Elementary simplifications (multiplication by zero and similar)
	tryOptimizeAdd(ex)
	tryOptimizeSub(ex)
	// tryOptimizeMult(ex)
	tryOptimizeDiv(ex)
}

func tryOptimizeMult(ex *expr) {
	if ex.op != mul {
		return
	}

	// Simplifying multiplication by zero: *(4,0,6,...) -> 0
	anyZero := array.MapReduce(ex.children, func(ch *expr) bool { return ch.op == eq && ch.value == 0 }, fun.Or, false)
	if anyZero {
		ex.op = eq
		ex.value = 0
		ex.children = nil
		return
	}

	// Removing identity element: *(3,1,5) -> *(3,5)
	k := array.Partition(ex.children, func(ch *expr) bool { return ch.op == eq && ch.value != 1 })
	ex.children = ex.children[:k]

	// Simplifying Unary multiplication: *(3) -> 3.
	if len(ex.children) == 1 {
		*ex = *ex.children[0]
		return
	}

	// Simplifying empty multiplication:  *() -> 1
	if len(ex.children) == 0 {
		ex.op = eq
		ex.value = 1
		ex.children = nil
		return
	}
}

func tryOptimizeAdd(ex *expr) {
	if ex.op != add {
		return
	}

	// Removing identity element: +(3,0,5) -> +(3,5)
	k := array.Partition(ex.children, func(ch *expr) bool { return ch.op == eq && ch.value != 0 })
	ex.children = ex.children[:k]

	// Simplifying Unary addition: +(3) -> 3.
	if len(ex.children) == 1 {
		*ex = *ex.children[0]
		return
	}

	// Simplifying empty addition:  +() -> 0
	if len(ex.children) == 0 {
		ex.op = eq
		ex.value = 0
		ex.children = nil
		return
	}
}

func tryOptimizeDiv(ex *expr) {
	if ex.op != div {
		return
	}

	// Illegal division: ÷(3,0) -> Undef
	if ex.children[1].op == eq && ex.children[1].value == 0 {
		panic(fmt.Errorf("division by zero: %s", ex))
	}

	// Removing zero element: ÷(0,5) -> 0
	if ex.children[0].op == eq && ex.children[0].value == 0 {
		ex.op = eq
		ex.value = 0
		ex.children = nil
		return
	}

	// Removing identity element: ÷(3,1) -> 3
	if ex.children[1].op == eq && ex.children[1].value == 1 {
		*ex = *ex.children[0]
	}
}

func tryOptimizeSub(ex *expr) {
	if ex.op != sub {
		return
	}

	// Removing identity element: -(3,0) -> 3
	if ex.children[1].op == eq && ex.children[1].value == 0 {
		*ex = *ex.children[0]
	}
}

func tryOperate(ex *expr) {
	if !array.MapReduce(ex.children, func(ch *expr) bool { return ch.op == eq }, fun.And, true) {
		return
	}
	operate(ex)
}

func tryInvertSubtraction(ex *expr) {
	if ex.op == sub && ex.children[1].op == eq {
		ex.op = add
		ex.children[1].value *= -1
	}
}

func tryCombineCommutative(ex *expr, op operator) {
	if ex.op != op || !array.MapReduce(ex.children, func(ch *expr) bool { return ch.op == op || ch.op == eq }, fun.And, true) {
		return
	}
	newFamily := make([]*expr, 0)
	array.Foreach(ex.children, func(ch **expr) {
		if (*ch).op == eq {
			newFamily = append(newFamily, *ch)
		} else {
			newFamily = append(newFamily, (*ch).children...)
		}
	})

	// We group all direct values together
	p := array.Partition(newFamily, func(a *expr) bool { return a.op == eq })
	if p < 2 {
		return
	}
	val := &expr{op: op, children: newFamily[:p]}
	operate(val)
	newFamily[p-1] = val
	newFamily = newFamily[p-1:]
	ex.children = newFamily
	return
}

// Converts (k*(a0+a1+...)) into ((k*a0)+(k*a1)+...)
func tryDistribute(ex *expr) {
	if len(ex.children) != 2 {
		return
	}

	array.Sort(ex.children, func(a, b *expr) bool { return a.op < b.op })
	if ex.children[0].op != eq {
		return
	}
	if ex.children[1].op != add {
		return
	}

	k := ex.children[0]
	a := ex.children[1].children

	ex.op = add
	ex.children = array.Map(a, func(ai *expr) *expr {
		e := &expr{
			op:       mul,
			children: []*expr{k, ai},
		}
		return e
	})

	simplify(ex)
}

func operate(ex *expr) {
	switch ex.op {
	case eq:
	case unknown:
		return
	case add:
		ex.value = array.MapReduce(ex.children, func(ch *expr) int64 { return ch.value }, fun.Add[int64], 0)
	case sub:
		ex.value = ex.children[0].value - ex.children[1].value
	case mul:
		ex.value = array.MapReduce(ex.children, func(ch *expr) int64 { return ch.value }, fun.Mul[int64], 1)
	case div:
		ex.value = ex.children[0].value / ex.children[1].value
	default:
		panic("unreachable")
	}
	ex.op = eq
	ex.children = nil
}

type operator int

const (
	eq operator = iota
	add
	sub
	mul
	div
	unknown
)

type monkey struct {
	children [2]string
	expr     expr
	visited  bool
}

type expr struct {
	op       operator
	value    int64
	children []*expr
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
					op:    eq,
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
