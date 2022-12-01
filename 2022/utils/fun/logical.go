package fun

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

// Gt is wrapper around the > operator.
func Gt[T generics.Number](a, b T) bool {
	return a > b
}

// Ge is wrapper around the >= operator.
func Ge[T generics.Number](a, b T) bool {
	return a >= b
}

// Eq is wrapper around the == operator.
func Eq[T generics.Number](a, b T) bool {
	return a == b
}

// Le is wrapper around the <= operator.
func Le[T generics.Number](a, b T) bool {
	return a <= b
}

// Lt is wrapper around the < operator.
func Lt[T generics.Number](a, b T) bool {
	return a < b
}

// And is the logical or.
func And(a, b bool) bool {
	return a && b
}

// Or is the logical or.
func Or(a, b bool) bool {
	return a || b
}
