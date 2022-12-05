package stack_test

import (
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/stack"
	"github.com/stretchr/testify/require"
)

func TestStack(t *testing.T) {
	t.Parallel()
	t.Run("int", testStack[int])
	t.Run("int8", testStack[int8])
	t.Run("int32", testStack[int32])
	t.Run("int64", testStack[int64])
}

func testStack[T generics.Signed](t *testing.T) {

	testCases := map[string]struct {
		input []T
	}{
		"empty": {input: []T{}},
		"one":   {input: []T{3}},
		"two":   {input: []T{1, 53}},
		"many":  {input: []T{1, 3, 15, 25, -16, 44}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			s := stack.New[T]()
			for _, p := range tc.input {
				s.Push(p)
			}
			require.Equal(t, tc.input, s.Data())
			require.Equal(t, len(tc.input), s.Size())

			for range tc.input {
				s.Pop()
			}

			require.Equal(t, 0, s.Size())
			require.True(t, s.IsEmpty())

			require.Panics(t, s.Pop, "Unexpected success popping empty stack")
			require.Panics(t, s.Pop, "Unexpected success popping empty stack twice")
		})
	}
}
