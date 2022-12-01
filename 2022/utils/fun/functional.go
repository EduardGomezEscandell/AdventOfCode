// Package fun provides basic operators to help in functional-style
// programming.
package fun

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

func Add[T generics.Number](a, b T) T { // nolint: revive
	return a + b
}

func Sub[T generics.Number](a, b T) T { // nolint: revive
	return a - b
}

func Greater[T generics.Number](a, b T) T { // nolint: revive
	if a > b {
		return 1
	}
	return 0
}

func GreaterEq[T generics.Number](a, b T) T { // nolint: revive
	if a >= b {
		return 1
	}
	return 0
}

func Equal[T generics.Number](a, b T) T { // nolint: revive
	if a < b {
		return 0
	}
	return 1
}

func LessEq[T generics.Number](a, b T) T { // nolint: revive
	if a <= b {
		return 1
	}
	return 0
}

func Less[T generics.Number](a, b T) T { // nolint: revive
	if a < b {
		return 1
	}
	return 0
}

func Sign[T generics.Signed](a T) T { // nolint: revive
	switch {
	case a > 0:
		return 1
	case a < 0:
		return -1
	}
	return 0
}
