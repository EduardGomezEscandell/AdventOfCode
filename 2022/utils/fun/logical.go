package fun

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

// Comparator is a function that takes two values and returs a boolean.
// Useful for wrappers around <=, ==, >=, etc.
type Comparator[T any] func(T, T) bool

// SortToEqual takes a function used to sort (e.g <, >) and prodces a
// function to check equality under that ordering.
func SortToEqual[T any](comp Comparator[T]) Comparator[T] {
	return func(a, b T) bool {
		return !comp(a, b) && !comp(b, a)
	}
}

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
