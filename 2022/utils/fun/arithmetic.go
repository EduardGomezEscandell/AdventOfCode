// Package fun implements wrappers and helpers to help in functional programming.
package fun

import (
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"golang.org/x/exp/constraints"
)

// Add is a wrapper around the plus operator.
func Add[T generics.Number](a, b T) T {
	return a + b
}

// Sub is a wrapper around the minus operator.
func Sub[T generics.Number](a, b T) T {
	return a - b
}

// Mul is a wrapper around the product operator.
func Mul[T generics.Number](a, b T) T {
	return a * b
}

// Div is a wrapper around the division operator.
// Unsafe from division by zero.
func Div[T generics.Number](a, b T) T {
	return a / b
}

// Count adds one to `a` iff `b` is true.
func Count[T generics.Number](a T, b bool) T {
	if b {
		return a + 1
	}
	return a
}

// Min returns the minimum between two numbers.
func Min[T generics.Number](a, b T) T {
	if a <= b {
		return a
	}
	return b
}

// Max returns the maximum between two numbers.
func Max[T generics.Number](a, b T) T {
	if a >= b {
		return a
	}
	return b
}

// Clamp returns value x if it is inside the range [lo, hi], otherwise
// returning the range boundary it is closest to.
func Clamp[T generics.Number](lo, x, hi T) T {
	return Max(lo, Min(x, hi))
}

// GCD computes the greatest common divisor (GCD) via Euclidean algorithm.
func GCD[T constraints.Integer](a, b T) T {
	for b != 0 {
		t := b
		b = a % b
		a = t
	}
	return a
}

// LCM finds the Least Common Multiple (LCM) via GCD.
func LCM[T constraints.Integer](a, b T, integers ...T) T {
	result := a * b / GCD(a, b)

	for i := 0; i < len(integers); i++ {
		result = LCM(result, integers[i])
	}

	return result
}
