package day08_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day08"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestMain(m *testing.M) {
	err := testutils.CheckEnv()
	if err != nil {
		log.Printf("Setup: %v", err)
	}
	r := m.Run()
	os.Exit(r)
}

type Height = day08.Height

func TestReadData(t *testing.T) {
	testCases := map[string]struct {
		data string
		want [][]Height
	}{
		"single": {data: "1", want: [][]Height{{1}}},
		"normal": {data: "123\n456\n789", want: [][]Height{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day08.ReadDataFile)()
			day08.ReadDataFile = func() ([]byte, error) {
				return []byte(tc.data), nil
			}

			got, err := day08.ParseInput()
			t.Log(err)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestComputeMinimumHeights(t *testing.T) {
	t.Parallel()
	n := Height(-1) // To keep alignment

	testCases := map[string]struct {
		data [][]Height
		want [][]Height
	}{
		"empty": {data: [][]Height{}, want: [][]Height{}},
		"one":   {data: [][]Height{{1}}, want: [][]Height{{n}}},
		"two":   {data: [][]Height{{1, 2}}, want: [][]Height{{n, n}}},
		"four":  {data: [][]Height{{1, 2}, {3, 4}}, want: [][]Height{{n, n}, {n, n}}},
		"nine":  {data: [][]Height{{1, 2, 1}, {3, 5, 3}, {3, 8, 3}}, want: [][]Height{{n, n, n}, {n, 2, n}, {n, n, n}}},
		"example": {data: [][]Height{
			{3, 0, 3, 7, 3},
			{2, 5, 5, 1, 2},
			{6, 5, 3, 3, 2},
			{3, 3, 5, 4, 9},
			{3, 5, 3, 9, 0},
		}, want: [][]Height{
			{n, n, n, n, n},
			{n, 0, 2, 2, n},
			{n, 3, 3, 2, n},
			{n, 3, 3, 5, n},
			{n, n, n, n, n}},
		},
		"real": {data: [][]Height{
			{1, 3, 1, 1, 0, 2},
			{2, 1, 0, 0, 3, 0},
			{2, 0, 1, 3, 0, 2},
			{2, 1, 0, 3, 0, 3},
			{0, 2, 1, 3, 1, 0},
		}, want: [][]Height{
			{n, n, n, n, n, n},
			{n, 2, 1, 1, 0, n},
			{n, 2, 1, 1, 1, n},
			{n, 2, 1, 2, 1, n},
			{n, n, n, n, n, n},
		},
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got := day08.ComputeMinimumHeights(tc.data)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestComputeScenery(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data [][]Height
		row  int
		col  int
		want uint
	}{
		"one":        {data: [][]Height{{1}}, want: 0},
		"two, right": {data: [][]Height{{1, 2}}, want: 0},
		"four":       {data: [][]Height{{1, 2}, {3, 4}}, want: 0},
		"nine": {data: [][]Height{
			{1, 2, 1},
			{3, 5, 3},
			{3, 8, 3}}, row: 1, col: 1, want: 1},
		"example": {data: [][]Height{
			{3, 0, 3, 7, 3},
			{2, 5, 5, 1, 2},
			{6, 5, 3, 3, 2},
			{3, 3, 5, 4, 9},
			{3, 5, 3, 9, 0},
		}, row: 3, col: 2, want: 8},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got := day08.ComputeScenery(tc.data, tc.row, tc.col)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data [][]Height
		want uint
	}{
		"empty":                {data: [][]Height{}, want: 0},
		"four":                 {data: [][]Height{{1, 2}, {3, 4}}, want: 4},
		"nine, center visible": {data: [][]Height{{1, 2, 1}, {3, 5, 3}, {3, 8, 3}}, want: 9},
		"nine, center hidden":  {data: [][]Height{{1, 2, 1}, {3, 1, 3}, {3, 8, 3}}, want: 8},
		"example": {data: [][]Height{
			{3, 0, 3, 7, 3},
			{2, 5, 5, 1, 2},
			{6, 5, 3, 3, 2},
			{3, 3, 5, 4, 9},
			{3, 5, 3, 9, 0}}, want: 21},
		"real": {data: [][]Height{
			{1, 3, 1, 1, 0, 2},
			{2, 1, 0, 0, 3, 0},
			{2, 0, 1, 3, 0, 2},
			{2, 1, 0, 3, 0, 3},
			{0, 2, 1, 3, 1, 0}}, want: 21,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day08.Part1(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data [][]Height
		want uint
	}{
		"empty": {data: [][]Height{}, want: 0},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day08.Part2(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1719
Result of part 2: 0
`
	buff := new(bytes.Buffer)

	err := day08.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
