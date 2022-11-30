package day00_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day00"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestMain(m *testing.M) {
	err := testutils.CheckEnv()
	if err != nil {
		log.Fatalf("Setup: %v", err)
	}
	r := m.Run()
	os.Exit(r)
}

func TestPart1(t *testing.T) {
	testCases := map[string]struct {
		data []uint
		want uint
	}{
		"empty":     {want: 0},
		"one":       {data: []uint{1}, want: 0},
		"few":       {data: []uint{1, 2}, want: 1},
		"same":      {data: []uint{1, 1, 1, 1}, want: 0},
		"staircase": {data: []uint{1, 2, 3, 4}, want: 3},
		"descent":   {data: []uint{5, 3, 1, 0}, want: 0},
		"example":   {data: []uint{199, 200, 208, 210, 200, 207, 240, 269, 260, 263}, want: 7},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day00.Part1(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	testCases := map[string]struct {
		data []uint
		want uint
	}{
		"empty":     {want: 0},
		"one":       {data: []uint{1}, want: 0},
		"two":       {data: []uint{1, 16}, want: 0},
		"few":       {data: []uint{1, 2, 3}, want: 0},
		"same":      {data: []uint{1, 1, 1, 1}, want: 0},
		"staircase": {data: []uint{1, 2, 3, 4}, want: 1},
		"descent":   {data: []uint{5, 3, 1, 0}, want: 0},
		"example":   {data: []uint{199, 200, 208, 210, 200, 207, 240, 269, 260, 263}, want: 5},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day00.Part2(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1715
Result of part 2: 1739
`
	buff := new(bytes.Buffer)

	err := day00.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
