package fun

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

// Predicate is a function that takes one value and returns a boolean.
type Predicate[T any] func(T) bool

// Sign returns +1 or -1, with the same sign as `a`.
// Returns 0 if a is 0.
func Sign[T generics.Signed](a T) int {
	switch {
	case a > 0:
		return 1
	case a < 0:
		return -1
	}
	return 0
}

// Abs returns the absolute value of a, i.e. a scalar
// with the same magnitude and with positive sign.
func Abs[T generics.Signed](a T) T {
	return T(Sign(a)) * a
}
