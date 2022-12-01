package day01_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day01"
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

func TestPart1(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data [][]uint
		want uint
	}{
		"empty":   {data: [][]uint{}, want: 0},
		"one":     {data: [][]uint{{5}}, want: 5},
		"few":     {data: [][]uint{{5, 3}, {1}}, want: 8},
		"example": {data: [][]uint{{1000, 2000, 3000}, {4000}, {5000, 6000}, {7000, 8000, 9000}, {10000}}, want: 24000},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day01.Part1(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestReadData(t *testing.T) {
	testCases := map[string]struct {
		data string
		want [][]uint
	}{
		"empty":   {data: "", want: [][]uint{}},
		"one":     {data: "5", want: [][]uint{{5}}},
		"few":     {data: "5\n3\n\n1", want: [][]uint{{5, 3}, {1}}},
		"example": {data: "1000\n2000\n3000\n\n4000\n\n5000\n6000\n\n7000\n8000\n9000\n\n10000", want: [][]uint{{1000, 2000, 3000}, {4000}, {5000, 6000}, {7000, 8000, 9000}, {10000}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day01.ReadDataFile)()
			day01.ReadDataFile = func() ([]byte, error) {
				return []byte(tc.data), nil
			}

			got, err := day01.ParseInput()
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data [][]uint
		want uint
	}{
		"empty":   {data: [][]uint{}, want: 0},
		"one":     {data: [][]uint{{5}}, want: 5},
		"few":     {data: [][]uint{{5, 3}, {1, 1, 1}, {2, 7}, {13}}, want: 30},
		"example": {data: [][]uint{{1000, 2000, 3000}, {4000}, {5000, 6000}, {7000, 8000, 9000}, {10000}}, want: 45000},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day01.Part2(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got, "Wrong result. Wanted %d but got %d.", tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 68787
Result of part 2: 198041
`
	buff := new(bytes.Buffer)

	err := day01.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
