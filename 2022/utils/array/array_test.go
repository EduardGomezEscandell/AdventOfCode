package array_test

import (
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/stretchr/testify/require"
)

func TestReduce(t *testing.T) {
	t.Parallel()
	t.Run("int", testReduce[int])
	t.Run("int32", testReduce[int32])
	t.Run("int64", testReduce[int64])
}

func testReduce[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	testCases := map[string]struct {
		input   []T
		fold    func(T, T) T
		expects T
	}{
		"empty sum":          {input: []T{}, fold: fun.Add[T], expects: 0},
		"small sum":          {input: []T{1, 2, 3}, fold: fun.Add[T], expects: 6},
		"normal sum":         {input: []T{8, 7, 5, 3, 3, -15}, fold: fun.Add[T], expects: 11},
		"empty subtraction":  {input: []T{}, fold: fun.Sub[T], expects: 0},
		"small subtraction":  {input: []T{1, 2, 3}, fold: fun.Sub[T], expects: -6},
		"normal subtraction": {input: []T{8, 7, 5, 3, 3, -15}, fold: fun.Sub[T], expects: -11},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.Reduce(tc.input, tc.fold)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testScamReduce[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	sq := func(x T) int { return int(x * x) }
	acc := func(x T, y int) T { return T(x + T(y)) }

	testCases := map[string]struct {
		input   []T
		unary   func(T) int
		fold    func(T, int) T
		expects T
	}{
		"empty sum of squares":  {input: []T{}, unary: sq, fold: acc, expects: 0},
		"small sum of squares":  {input: []T{1, 2, 3}, unary: sq, fold: acc, expects: 14},
		"normal sum of squares": {input: []T{7, 5, 3, 3}, unary: sq, fold: acc, expects: 92},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.ScanReduce(tc.input, tc.unary, tc.fold)
			require.Equal(t, tc.expects, got)
		})
	}
}

func TestAdjacentReduce(t *testing.T) {
	testCases := map[string]struct {
		input   []int
		merge   func(int, int) int
		fold    func(int, int) int
		expects int
	}{
		"empty sum of differences":  {merge: fun.Sub[int], fold: fun.Add[int], expects: 0, input: []int{}},
		"small sum of differences":  {merge: fun.Sub[int], fold: fun.Add[int], expects: -2, input: []int{1, 2, 3}},
		"normal sum of differences": {merge: fun.Sub[int], fold: fun.Add[int], expects: 23, input: []int{8, 7, 5, 3, 3, -15}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.AdjacentReduce(tc.input, tc.merge, tc.fold)
			require.Equal(t, tc.expects, got)
		})
	}
}

func TestScan(t *testing.T) {
	testCases := map[string]struct {
		input   []int
		op      func(int) int
		expects []int
	}{
		"empty sign":  {input: []int{}, op: fun.Sign[int], expects: []int{}},
		"small sign":  {input: []int{1, -2, 3}, op: fun.Sign[int], expects: []int{1, -1, 1}},
		"normal sign": {input: []int{-8, 7, 0, 3, 3, -15}, op: fun.Sign[int], expects: []int{-1, 1, 0, 1, 1, -1}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.Scan(tc.input, tc.op)
			require.Equal(t, tc.expects, got)
		})
	}
}

func TestAdjacentScan(t *testing.T) {
	testCases := map[string]struct {
		input   []int
		op      func(int, int) int
		expects []int
	}{
		"empty sum":          {input: []int{}, op: fun.Add[int], expects: []int{}},
		"small sum":          {input: []int{1, -2, 3}, op: fun.Add[int], expects: []int{-1, 1}},
		"normal sum":         {input: []int{-8, 7, 0, 3, 3, -15}, op: fun.Add[int], expects: []int{-1, 7, 3, 6, -12}},
		"empty subtraction":  {input: []int{}, op: fun.Sub[int], expects: []int{}},
		"small subtraction":  {input: []int{1, -2, 3}, op: fun.Sub[int], expects: []int{3, -5}},
		"normal subtraction": {input: []int{-8, 7, 0, 3, 3, -15}, op: fun.Sub[int], expects: []int{-15, 7, -3, 0, 18}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.AdjacentScan(tc.input, tc.op)
			require.Equal(t, tc.expects, got)
		})
	}
}
