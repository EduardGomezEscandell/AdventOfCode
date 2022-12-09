package fun_test

import (
	"fmt"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/stretchr/testify/require"
)

func TestArithmetic(t *testing.T) {
	t.Parallel()
	t.Run("int", testArithmetic[int])
	t.Run("int8", testArithmetic[int8])
	t.Run("int32", testArithmetic[int32])
	t.Run("int64", testArithmetic[int64])
}

func TestAbs(t *testing.T) {
	t.Parallel()
	t.Run("int", testAbs[int])
	t.Run("int8", testAbs[int8])
	t.Run("int32", testAbs[int32])
	t.Run("int64", testAbs[int64])
}

func testArithmetic[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input1   T
		input2   T
		fun      func(T, T) T
		expected any
	}{
		"add": {fun: fun.Add[T], input1: 1, input2: 5, expected: T(6)},
		"sub": {fun: fun.Sub[T], input1: 1, input2: 5, expected: T(-4)},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := tc.fun(tc.input1, tc.input2)
			require.Equal(t, tc.expected, got)
		})
	}
}

func testAbs[T generics.Signed](t *testing.T) {
	t.Parallel()
	testCases := map[T]T{
		0:   0,
		13:  13,
		-15: 15,
	}

	for input, want := range testCases {
		input, want := input, want
		t.Run(fmt.Sprintf("%v", input), func(t *testing.T) {
			require.Equal(t, want, fun.Abs(input))
		})
	}
}
