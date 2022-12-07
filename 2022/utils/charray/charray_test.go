package charray_test

import (
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/generics"
	"github.com/stretchr/testify/require"
)

func TestMap(t *testing.T) {
	t.Parallel()
	t.Run("int", testMap[int])
	t.Run("int8", testMap[int8])
	t.Run("int32", testMap[int32])
	t.Run("int64", testMap[int64])
}

func TestReduce(t *testing.T) {
	t.Parallel()
	t.Run("int", testReduce[int])
	t.Run("int8", testReduce[int8])
	t.Run("int32", testReduce[int32])
	t.Run("int64", testReduce[int64])
}

func TestMapReduce(t *testing.T) {
	t.Parallel()
	t.Run("int", testMapReduce[int])
	t.Run("int8", testMapReduce[int8])
	t.Run("int32", testMapReduce[int32])
	t.Run("int64", testMapReduce[int64])
}

func TestAdjacentMap(t *testing.T) {
	t.Parallel()
	t.Run("int", testAdjacentMap[int])
	t.Run("int8", testAdjacentMap[int8])
	t.Run("int32", testAdjacentMap[int32])
	t.Run("int64", testAdjacentMap[int64])
}

func TestAdjacentReduce(t *testing.T) {
	t.Parallel()
	t.Run("int", testAdjacentReduce[int])
	t.Run("int8", testAdjacentReduce[int8])
	t.Run("int32", testAdjacentReduce[int32])
	t.Run("int64", testAdjacentReduce[int64])
}

func TestZipWith(t *testing.T) {
	t.Parallel()
	t.Run("int", testZipWith[int])
	t.Run("int8", testZipWith[int8])
	t.Run("int32", testZipWith[int32])
	t.Run("int64", testZipWith[int64])
}

func TestZipReduce(t *testing.T) {
	t.Parallel()
	t.Run("int", testZipReduce[int])
	t.Run("int8", testZipReduce[int8])
	t.Run("int32", testZipReduce[int32])
	t.Run("int64", testZipReduce[int64])
}

func TestTopN(t *testing.T) {
	t.Parallel()
	t.Run("int", testTopN[int])
	t.Run("int8", testTopN[int8])
	t.Run("int32", testTopN[int32])
	t.Run("int64", testTopN[int64])
}

func TestCommon(t *testing.T) {
	t.Parallel()
	t.Run("int", testCommon[int])
	t.Run("int8", testCommon[int8])
	t.Run("int32", testCommon[int32])
	t.Run("int64", testCommon[int64])
}

func TestUnique(t *testing.T) {
	t.Parallel()
	t.Run("int", testUnique[int])
	t.Run("int8", testUnique[int8])
	t.Run("int32", testUnique[int32])
	t.Run("int64", testUnique[int64])
}

func TestMultiplex(t *testing.T) {
	t.Parallel()
	t.Run("int", testMultiplex[int])
	t.Run("int8", testMultiplex[int8])
	t.Run("int32", testMultiplex[int32])
	t.Run("int64", testMultiplex[int64])
}

func testMap[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	testCases := map[string]struct {
		input   []T
		op      func(T) int
		expects []int
	}{
		"empty sign":  {input: []T{}, op: fun.Sign[T], expects: []int{}},
		"small sign":  {input: []T{1, -2, 3}, op: fun.Sign[T], expects: []int{1, -1, 1}},
		"normal sign": {input: []T{-8, 7, 0, 3, 3, -15}, op: fun.Sign[T], expects: []int{-1, 1, 0, 1, 1, -1}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			gotch := charray.Map(ch, tc.op)

			got := array.FromChannel(gotch)
			require.Equal(t, tc.expects, got)
		})
	}
}

func inputToChannel[T any](in []T, cap int, timeout time.Duration) (<-chan T, context.CancelFunc) {
	ctx, cancel := context.WithTimeout(context.Background(), timeout)
	outch := charray.FromArray(ctx, in, cap)
	return outch, cancel
}

func testReduce[T generics.Signed](t *testing.T) { // nolint: thelper // nolint: thelper
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
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			got := charray.Reduce(ch, tc.fold, 0)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testMapReduce[T generics.Signed](t *testing.T) { // nolint: thelper // nolint: thelper
	t.Parallel()

	sq := func(x T) int { return int(x * x) }
	acc := func(x T, y int) T { return x + T(y) }

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
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			got := charray.Reduce(charray.Map(ch, tc.unary), tc.fold, 0)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testAdjacentReduce[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input   []T
		merge   func(T, T) T
		fold    func(T, T) T
		expects T
	}{
		"empty sum of differences":  {merge: fun.Sub[T], fold: fun.Add[T], expects: 0, input: []T{}},
		"small sum of differences":  {merge: fun.Sub[T], fold: fun.Add[T], expects: -2, input: []T{1, 2, 3}},
		"normal sum of differences": {merge: fun.Sub[T], fold: fun.Add[T], expects: 23, input: []T{8, 7, 5, 3, 3, -15}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			got := charray.Reduce(charray.AdjacentMap(ch, tc.merge), tc.fold, 0)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testZipWith[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input1 []T
		input2 []T
		zip    func(T, T) T
		want   []T
	}{
		"empty":      {zip: fun.Sub[T], want: []T{}},
		"half empty": {zip: fun.Sub[T], input1: []T{1}, want: []T{}},
		"single":     {zip: fun.Sub[T], input1: []T{1}, input2: []T{2}, want: []T{-1}},
		"normal":     {zip: fun.Sub[T], input1: []T{1, 3, 9}, input2: []T{2, -1, 6}, want: []T{-1, 4, 3}},
		"normal add": {zip: fun.Add[T], input1: []T{1, 3, 9}, input2: []T{2, -1, 6}, want: []T{3, 2, 15}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch1 := charray.FromArray(ctx, tc.input1, 0)
			ch2 := charray.FromArray(ctx, tc.input2, 0)

			gotch := charray.ZipWith(ch1, ch2, tc.zip)

			got := array.FromChannel(gotch)
			require.Equal(t, tc.want, got)
		})
	}
}

func testZipReduce[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	testCases := map[string]struct {
		input1 []T
		input2 []T
		want   T
	}{
		"empty":      {want: 0},
		"half empty": {input1: []T{1}, want: 0},
		"single":     {input1: []T{1}, input2: []T{2}, want: 2},
		"normal":     {input1: []T{1, 3, 9}, input2: []T{2, -1, 6}, want: 53},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			// Testing by computing inner product.
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch1 := charray.FromArray(ctx, tc.input1, 0)
			ch2 := charray.FromArray(ctx, tc.input2, 0)

			got := charray.Reduce(charray.ZipWith(ch1, ch2, fun.Mul[T]), fun.Add[T], 0)
			require.Equal(t, tc.want, got)
		})
	}
}

func testAdjacentMap[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input   []T
		op      func(T, T) T
		expects []T
	}{
		"empty sum":          {input: []T{}, op: fun.Add[T], expects: []T{}},
		"small sum":          {input: []T{1, -2, 3}, op: fun.Add[T], expects: []T{-1, 1}},
		"normal sum":         {input: []T{-8, 7, 0, 3, 3, -15}, op: fun.Add[T], expects: []T{-1, 7, 3, 6, -12}},
		"empty subtraction":  {input: []T{}, op: fun.Sub[T], expects: []T{}},
		"small subtraction":  {input: []T{1, -2, 3}, op: fun.Sub[T], expects: []T{3, -5}},
		"normal subtraction": {input: []T{-8, 7, 0, 3, 3, -15}, op: fun.Sub[T], expects: []T{-15, 7, -3, 0, 18}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			gotch := charray.AdjacentMap(ch, tc.op)

			got := array.FromChannel(gotch)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testTopN[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input []T
		n     uint
		comp  fun.Comparator[T]
		want  []T
	}{
		"empty":           {n: 2, input: []T{}, comp: fun.Lt[T], want: []T{}},
		"too short":       {n: 5, input: []T{1, 2, 3}, comp: fun.Lt[T], want: []T{1, 2, 3}},
		"just the amount": {n: 5, input: []T{1, 4, 2, 3}, comp: fun.Lt[T], want: []T{1, 2, 3, 4}},
		"bottom 3":        {n: 3, input: []T{8, 7, 5, 3, 3, 15}, comp: fun.Lt[T], want: []T{3, 3, 5}},
		"top 3":           {n: 3, input: []T{8, 7, 5, 3, 3, 15}, comp: fun.Gt[T], want: []T{15, 8, 7}},
		"bottom 4":        {n: 4, input: []T{-1, 84, 5, 101, 12, 9, 15, 1}, comp: fun.Lt[T], want: []T{-1, 1, 5, 9}},
		"top 4":           {n: 4, input: []T{-1, 84, 5, 101, 12, 9, 15, 1}, comp: fun.Gt[T], want: []T{101, 84, 15, 12}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			got := charray.BestN(ch, tc.n, tc.comp)
			require.Equal(t, tc.want, got)
		})
	}
}

func testCommon[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input1 []T
		input2 []T
		sort   func(T, T) bool
		want   []T
	}{
		"less than, empty":                               {sort: fun.Lt[T], want: []T{}},
		"greater than, empty":                            {sort: fun.Gt[T], want: []T{}},
		"less than, half empty":                          {sort: fun.Lt[T], input1: []T{1, 6, 8}, want: []T{}},
		"greater than, half empty":                       {sort: fun.Gt[T], input1: []T{1, 6, 8}, want: []T{}},
		"less than, single, no shared":                   {sort: fun.Lt[T], input1: []T{1}, input2: []T{2}, want: []T{}},
		"greater than, single, no shared":                {sort: fun.Gt[T], input1: []T{1}, input2: []T{2}, want: []T{}},
		"less than, single, shared":                      {sort: fun.Lt[T], input1: []T{1}, input2: []T{1}, want: []T{1}},
		"greater than, single, shared":                   {sort: fun.Gt[T], input1: []T{1}, input2: []T{1}, want: []T{1}},
		"less than, normal, shared":                      {sort: fun.Lt[T], input1: []T{1, 3, 9}, input2: []T{1, 5, 9}, want: []T{1, 9}},
		"greater than, normal, shared":                   {sort: fun.Gt[T], input1: []T{1, 3, 9}, input2: []T{1, 5, 9}, want: []T{9, 1}},
		"less than, normal, no shared":                   {sort: fun.Lt[T], input1: []T{1, 3, 9}, input2: []T{2, 4, 6}, want: []T{}},
		"greater than, normal, no shared":                {sort: fun.Gt[T], input1: []T{1, 3, 9}, input2: []T{2, 4, 6}, want: []T{}},
		"less than, repeats, no shared":                  {sort: fun.Lt[T], input1: []T{15, 0, 15, 9}, input2: []T{0, 15}, want: []T{0, 15}},
		"greater than, repeats, no shared":               {sort: fun.Gt[T], input1: []T{15, 0, 15, 9}, input2: []T{0, 15}, want: []T{15, 0}},
		"less than, double repeats, no shared":           {sort: fun.Lt[T], input1: []T{15, 0, 15, 9}, input2: []T{0, 15, 15}, want: []T{0, 15, 15}},
		"greater than, double repeats, no shared":        {sort: fun.Gt[T], input1: []T{15, 0, 15, 9}, input2: []T{0, 15, 15}, want: []T{15, 15, 0}},
		"greater than, triple double repeats, no shared": {sort: fun.Gt[T], input1: []T{15, 15, 15, 9}, input2: []T{0, 15, 15}, want: []T{15, 15}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			array.Sort(tc.input1, tc.sort)
			array.Sort(tc.input2, tc.sort)

			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch1 := charray.FromArray(ctx, tc.input1, 0)
			ch2 := charray.FromArray(ctx, tc.input2, 0)

			gotch := charray.Common(ch1, ch2, tc.sort)
			got := array.FromChannel(gotch)
			require.Equal(t, tc.want, got)
		})
	}
}

func testUnique[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input []T
		sort  func(T, T) bool
		want  []T
	}{
		"empty":                           {sort: fun.Lt[T], want: []T{}},
		"one":                             {sort: fun.Lt[T], input: []T{5}, want: []T{5}},
		"few, greater than, no repeats":   {sort: fun.Gt[T], input: []T{3, 1, 5}, want: []T{5, 3, 1}},
		"few, less than, no repeats":      {sort: fun.Lt[T], input: []T{3, 1, 5}, want: []T{1, 3, 5}},
		"few, greater than, some repeats": {sort: fun.Gt[T], input: []T{3, 1, 3, 5}, want: []T{5, 3, 1}},
		"few, less than, some repeats":    {sort: fun.Lt[T], input: []T{3, 1, 3, 5}, want: []T{1, 3, 5}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			array.Sort(tc.input, tc.sort)

			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			gotch := charray.Unique(ch, fun.SortToEqual(tc.sort))

			got := array.FromChannel(gotch)
			require.Equal(t, tc.want, got)
		})
	}
}

func testMultiplex[T generics.SignedInt](t *testing.T) { // nolint: thelper
	t.Parallel()

	testCases := map[string]struct {
		input []T
		want  [][]T
	}{
		"empty":      {want: [][]T{{}, {}}},
		"half empty": {input: []T{1}, want: [][]T{{}, {1}}},
		"samesies":   {input: []T{1, 3}, want: [][]T{{}, {1, 3}}},
		"split":      {input: []T{1, 3, 8, 29}, want: [][]T{{8}, {1, 3, 29}}},
		"normal":     {input: []T{-5, 8, 1, 32, 90, -101}, want: [][]T{{8, 32, 90}, {-5, 1, -101}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			// Testing by splitting between even and odd.
			ch, cancel := inputToChannel(tc.input, 0, 10*time.Second)
			defer cancel()

			gotchans := charray.Multiplex(ch, 2, func(x T) int {
				if x%2 == 0 {
					return 0
				}
				return 1
			})

			// Converting multiplexed streams into arrays for easier assertion
			got := make([][]T, len(gotchans))
			done := make(chan struct{})
			for i, ch := range gotchans {
				i, ch := i, ch
				go func() {
					got[i] = array.FromChannel(ch)
					done <- struct{}{}
				}()
			}

			for range gotchans {
				<-done
			}
			close(done)

			require.Equal(t, tc.want, got)
		})
	}
}
