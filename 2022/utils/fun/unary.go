package fun

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"

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

func Abs[T generics.Signed](a T) T {
	return T(Sign(a)) * a
}
