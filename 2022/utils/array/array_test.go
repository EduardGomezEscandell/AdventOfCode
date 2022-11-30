package array_test

import (
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/stretchr/testify/require"
)

func TestReduceInt(t *testing.T) {

	testCases := map[string]struct {
		input   []int
		fold    func(int, int) int
		expects int
	}{
		"empty sum":          {input: []int{}, fold: fun.Add[int], expects: 0},
		"small sum":          {input: []int{1, 2, 3}, fold: fun.Add[int], expects: 6},
		"normal sum":         {input: []int{8, 7, 5, 3, 3, -15}, fold: fun.Add[int], expects: 11},
		"empty subtraction":  {input: []int{}, fold: fun.Sub[int], expects: 0},
		"small subtraction":  {input: []int{1, 2, 3}, fold: fun.Sub[int], expects: -6},
		"normal subtraction": {input: []int{8, 7, 5, 3, 3, -15}, fold: fun.Sub[int], expects: -11},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.Reduce(tc.input, tc.fold)
			require.Equal(t, tc.expects, got)
		})
	}
}

func TestReduceFloat(t *testing.T) {

	testCases := map[string]struct {
		input   []float32
		fold    func(float32, float32) float32
		expects float32
	}{
		"empty sum":          {input: []float32{}, fold: fun.Add[float32], expects: 0},
		"small sum":          {input: []float32{1, 2, 3}, fold: fun.Add[float32], expects: 6},
		"normal sum":         {input: []float32{8, 7, 5, 3, 3, -15}, fold: fun.Add[float32], expects: 11},
		"empty subtraction":  {input: []float32{}, fold: fun.Sub[float32], expects: 0},
		"small subtraction":  {input: []float32{1, 2, 3}, fold: fun.Sub[float32], expects: -6},
		"normal subtraction": {input: []float32{8, 7, 5, 3, 3, -15}, fold: fun.Sub[float32], expects: -11},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got := array.Reduce(tc.input, tc.fold)
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
