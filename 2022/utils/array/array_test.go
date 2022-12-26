package array_test

import (
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
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

func TestFind(t *testing.T) {
	t.Parallel()
	t.Run("int", testFind[int])
	t.Run("int8", testFind[int8])
	t.Run("int32", testFind[int32])
	t.Run("int64", testFind[int64])
}

func TestPartition(t *testing.T) {
	t.Parallel()
	t.Run("int", testPartition[int])
	t.Run("int8", testPartition[int8])
	t.Run("int32", testPartition[int32])
	t.Run("int64", testPartition[int64])
}

func TestInsert(t *testing.T) {
	t.Parallel()
	t.Run("int", testInsert[int])
	t.Run("int8", testInsert[int8])
	t.Run("int32", testInsert[int32])
	t.Run("int64", testInsert[int64])
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
			got := array.Map(tc.input, tc.op)
			require.Equal(t, tc.expects, got)
		})
	}
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
			got := array.Reduce(tc.input, tc.fold, 0)
			require.Equal(t, tc.expects, got)
		})
	}
}

func testMapReduce[T generics.Signed](t *testing.T) { // nolint: thelper
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
			got := array.MapReduce(tc.input, tc.unary, tc.fold, 0)
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
			got := array.AdjacentReduce(tc.input, tc.merge, tc.fold)
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
			got := array.ZipWith(tc.input1, tc.input2, tc.zip)
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
			got := array.ZipReduce(tc.input1, tc.input2, fun.Mul[T], fun.Add[T])
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
			got := array.AdjacentMap(tc.input, tc.op)
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
			got := array.BestN(tc.input, tc.n, tc.comp)
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
			got := array.Common(tc.input1, tc.input2, tc.sort)
			require.Equal(t, tc.want, got)
		})
	}
}

func testUnique[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		input     []T
		sort      func(T, T) bool
		wantArr   []T
		wantCount int
	}{
		"empty":                           {sort: fun.Lt[T]},
		"one":                             {sort: fun.Lt[T], input: []T{5}, wantArr: []T{5}, wantCount: 1},
		"few, greater than, no repeats":   {sort: fun.Gt[T], input: []T{3, 1, 5}, wantArr: []T{5, 3, 1}, wantCount: 3},
		"few, less than, no repeats":      {sort: fun.Lt[T], input: []T{3, 1, 5}, wantArr: []T{1, 3, 5}, wantCount: 3},
		"few, greater than, some repeats": {sort: fun.Gt[T], input: []T{3, 1, 3, 5}, wantArr: []T{5, 3, 1, 3}, wantCount: 3},
		"few, less than, some repeats":    {sort: fun.Lt[T], input: []T{3, 1, 3, 5}, wantArr: []T{1, 3, 5, 3}, wantCount: 3},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			array.Sort(tc.input, tc.sort)
			got := array.Unique(tc.input, fun.SortToEqual(tc.sort))
			require.Equal(t, tc.wantArr, tc.input)
			require.Equal(t, tc.wantCount, got)
		})
	}
}

func testFind[T generics.Number](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		data   []T
		search T
		want   int
	}{
		"empty":          {data: []T{}, want: -1},
		"single, found":  {data: []T{5}, search: 5, want: 0},
		"single, missed": {data: []T{3}, search: 9, want: -1},
		"missed":         {data: []T{3, 13, 84, 6, 3}, search: 9, want: -1},
		"first":          {data: []T{3, 13, 84, 6, 11}, search: 3, want: 0},
		"last":           {data: []T{3, 13, 84, 6, 11}, search: 11, want: 4},
		"repeated":       {data: []T{3, 13, 84, 6, 3}, search: 3, want: 0},
		"standard":       {data: []T{3, 13, 84, 6, 3}, search: 84, want: 2},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			input := make([]T, len(tc.data))
			copy(input, tc.data)

			got := array.Find(input, tc.search, fun.Eq[T])
			require.Equal(t, tc.want, got)

			require.Equal(t, input, tc.data, "array.Find modified the input array")
		})
	}
}

func testPartition[T generics.Number](t *testing.T) { // nolint: thelper
	t.Parallel()
	testCases := map[string]struct {
		data   []T
		search T
		want   int
	}{
		"empty":          {data: []T{}, want: 0},
		"single, before": {data: []T{5}, search: 3, want: 0},
		"single, after":  {data: []T{3}, search: 9, want: 1},
		"standard":       {data: []T{3, 13, 84, 6, 3}, search: 9, want: 3},
		"bug":            {data: []T{1, 2, 15}, search: 9, want: 2},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			input := make([]T, len(tc.data))
			copy(input, tc.data)

			got := array.Partition(input, func(x T) bool { return x < tc.search })
			require.Equal(t, tc.want, got)

			for i, v := range input[:got] {
				require.LessOrEqual(t, v, tc.search, "Item %d is greater than the partition: %d > %d. Array: %v", i, v, tc.search, input)
			}
			for i, v := range input[got:] {
				require.GreaterOrEqual(t, v, tc.search, "Item %d is smaller than the partition: %d < %d. Array: %v", i, v, tc.search, input)
			}
		})
	}
}

func testInsert[T generics.Signed](t *testing.T) { // nolint: thelper
	t.Parallel()

	testCases := map[string]struct {
		input []T
		pos   int
		val   T
		want  []T
	}{
		"empty":  {input: []T{}, val: 5, pos: 0, want: []T{5}},
		"lead":   {input: []T{12, 8, 6}, val: 11, pos: 0, want: []T{11, 12, 8, 6}},
		"tail":   {input: []T{12, 8, 6}, val: 11, pos: 3, want: []T{12, 8, 6, 11}},
		"middle": {input: []T{12, 8, 6}, val: 11, pos: 1, want: []T{12, 11, 8, 6}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			in := make([]T, len(tc.input))
			copy(in, tc.input)
			got := array.Insert(in, tc.val, tc.pos)
			require.Equal(t, tc.want, got)
		})
	}
}
