package day21

import (
	"fmt"
	"math"
	"strings"

	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

type operator int

// expr is an abstract syntax tree:
//
//	  Non-leaves may contain the following operators:
//		* addition
//		* subtraction
//		* multiplication
//		* division
//
//	  Leaves may contain:
//		* integer values
//		* a single unknown
type expr struct {
	op       operator
	value    int64
	children []*expr
}

// Node types.
const (
	constant operator = iota
	unknown
	add
	sub
	mul
	div
)

func (ex expr) String() string {
	operands := algo.Map(ex.children, func(ch *expr) string { return ch.String() })

	switch ex.op {
	case constant:
		return fmt.Sprintf("%d", ex.value)
	case unknown:
		return "x"
	case add:
		return fmt.Sprintf("(%s)", strings.Join(operands, "+"))
	case sub:
		return fmt.Sprintf("(%s)", strings.Join(operands, "-"))
	case mul:
		return fmt.Sprintf("(%s)", strings.Join(operands, "*"))
	case div:
		return fmt.Sprintf("(%s)", strings.Join(operands, "/"))
	default:
		panic("unreachable")
	}
}

// eval evaluates the expression:
//   - set "store: true" to replace every subtree with its equivalent constant (when possible).
//   - set "store: false" to obtain the result without modifying the syntax tree.
//   - set "replace: nil" in order to leave the unknown as it is.
//   - set "replace: some value" in order to substitute the unknown.
func eval(ex *expr, store bool, replace *int64) (v float64) {
	switch ex.op {
	case constant:
		return float64(ex.value)
	case unknown:
		if replace != nil {
			v = float64(*replace)
		} else {
			return 0
		}
	case add:
		v = algo.MapReduce(ex.children, func(ch *expr) float64 { return eval(ch, store, replace) }, utils.Add[float64], 0)
	case sub:
		x := eval(ex.children[0], store, replace)
		y := eval(ex.children[1], store, replace)
		v = x - y
	case mul:
		v = algo.MapReduce(ex.children, func(ch *expr) float64 { return eval(ch, store, replace) }, utils.Mul[float64], 1)
	case div:
		x := eval(ex.children[0], store, replace)
		y := eval(ex.children[1], store, replace)
		if math.Mod(x, y) != 0 && store {
			return 0
		}
		v = x / y
	default:
		panic("unreachable")
	}

	if store {
		ex.value = int64(v)
		ex.op = constant
		ex.children = nil
	}
	return v
}

// simplify takes the AST and simplifies the expression without modifying
// by performing modifications that reduce its size. The end result is
// equivalent and without loss of generality.
func simplify(ex *expr) {
	if len(ex.children) == 0 {
		return
	}
	algo.Foreach(ex.children, func(ch **expr) { simplify(*ch) })

	// If they all contain direct values, we operate
	tryOperate(ex)

	// If it is a subtraction, we invert it
	tryInvertSubtraction(ex)

	// Disassembling products
	tryDistributeMul(ex)
	tryDistributeDiv(ex)

	// // If children all contain commutative operations, we create a larger commutative operation
	tryCombineCommutative(ex, add)
	tryCombineCommutative(ex, mul)

	// // Elementary simplifications (multiplication by zero and similar)
	tryOptimizeAdd(ex)
	tryOptimizeSub(ex)
	tryOptimizeMult(ex)
	tryOptimizeDiv(ex)
}

func tryOptimizeMult(ex *expr) {
	if ex.op != mul {
		return
	}

	// Simplifying multiplication by zero: *(4,0,6,...) -> 0
	anyZero := algo.MapReduce(ex.children, func(ch *expr) bool { return ch.op == constant && ch.value == 0 }, utils.Or, false)
	if anyZero {
		ex.op = constant
		ex.value = 0
		ex.children = nil
		return
	}

	// Removing identity element: *(3,1,5) -> *(3,5)
	k := algo.Partition(ex.children, func(ch *expr) bool { return ch.op != constant || ch.value != 1 })
	ex.children = ex.children[:k]

	// Simplifying Unary multiplication: *(3) -> 3.
	if len(ex.children) == 1 {
		*ex = *ex.children[0]
		return
	}

	// Simplifying empty multiplication:  *() -> 1
	if len(ex.children) == 0 {
		ex.op = constant
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
	k := algo.Partition(ex.children, func(ch *expr) bool { return ch.op != constant || ch.value != 0 })
	ex.children = ex.children[:k]

	// Simplifying Unary addition: +(3) -> 3.
	if len(ex.children) == 1 {
		*ex = *ex.children[0]
		return
	}

	// Simplifying empty addition:  +() -> 0
	if len(ex.children) == 0 {
		ex.op = constant
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
	if ex.children[1].op == constant && ex.children[1].value == 0 {
		panic(fmt.Errorf("division by zero: %s", ex))
	}

	// Removing zero element: ÷(0,5) -> 0
	if ex.children[0].op == constant && ex.children[0].value == 0 {
		ex.op = constant
		ex.value = 0
		ex.children = nil
		return
	}

	// Removing identity element: ÷(3,1) -> 3
	if ex.children[1].op == constant && ex.children[1].value == 1 {
		*ex = *ex.children[0]
	}
}

func tryOptimizeSub(ex *expr) {
	if ex.op != sub {
		return
	}

	// Removing identity element: -(3,0) -> 3
	if ex.children[1].op == constant && ex.children[1].value == 0 {
		*ex = *ex.children[0]
	}
}

// tryOperate replaces a subtree with is equivalent value when
// all its children are constants.
func tryOperate(ex *expr) {
	if !algo.MapReduce(ex.children, func(ch *expr) bool { return ch.op == constant }, utils.And, true) {
		return
	}
	eval(ex, true, nil)
}

// tryInvertSubtraction replaces (a-b) with (a+(-b)) when
// the root node is a subtraction.
func tryInvertSubtraction(ex *expr) {
	if ex.op == sub && ex.children[1].op == constant {
		ex.op = add
		ex.children[1].value *= -1
	}
}

// tryCombineCommutative preforms, for a given operator ⨯:
// - (a⨯b)⨯(c⨯d) -> a⨯b⨯c⨯d
// - (a⨯b)⨯c -> a⨯b⨯c
// - a⨯(b⨯c) -> a⨯b⨯c
// - etc.
// operator ⨯ must be commutative and associative for this to make sense.
func tryCombineCommutative(ex *expr, op operator) {
	if ex.op != op || !algo.MapReduce(ex.children, func(ch *expr) bool { return ch.op == op || ch.op == constant }, utils.And, true) {
		return
	}
	newFamily := make([]*expr, 0)
	algo.Foreach(ex.children, func(ch **expr) {
		if (*ch).op == constant {
			newFamily = append(newFamily, *ch)
		} else {
			newFamily = append(newFamily, (*ch).children...)
		}
	})

	// We group all direct values together
	p := algo.Partition(newFamily, func(a *expr) bool { return a.op == constant })
	if p < 2 {
		return
	}
	val := &expr{op: op, children: newFamily[:p]}
	eval(val, true, nil)
	newFamily[p-1] = val
	newFamily = newFamily[p-1:]
	ex.children = newFamily
}

// Converts (k*(a0+a1+...)) into ((k*a0)+(k*a1)+...)
func tryDistributeMul(ex *expr) {
	if len(ex.children) != 2 {
		return
	}
	if ex.op != mul {
		return
	}

	algo.Sort(ex.children, func(a, b *expr) bool { return a.op < b.op })
	if ex.children[0].op != constant {
		return
	}
	if ex.children[1].op != add {
		return
	}

	k := ex.children[0]
	a := ex.children[1].children

	ex.op = add
	ex.children = algo.Map(a, func(ai *expr) *expr {
		e := &expr{
			op:       mul,
			children: []*expr{k, ai},
		}
		return e
	})

	simplify(ex)
}

// Converts ((a+b)/k) into (a/k + b/k).
func tryDistributeDiv(ex *expr) {
	if ex.op != div {
		return
	}

	if ex.children[0].op != add {
		return
	}
	if ex.children[1].op != constant {
		return
	}

	a := ex.children[0].children
	k := ex.children[1]

	ex.op = add
	ex.children = algo.Map(a, func(ai *expr) *expr {
		e := &expr{
			op:       div,
			children: []*expr{ai, k},
		}
		return e
	})

	simplify(ex)
}
