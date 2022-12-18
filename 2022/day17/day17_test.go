package day17_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day17"
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

func TestReadData(t *testing.T) {
	tc := map[string]struct {
		input []string
		want  []int8
	}{
		"three": {input: []string{"<<>"}, want: []int8{-1, -1, 1}},
		"example": {input: []string{">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>"},
			want: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1},
		},
	}

	for name, tc := range tc {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day17.ReadDataFile)()
			day17.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day17.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestSolvePart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input  []int8
		nrocks day17.Long
		want   day17.Long
	}{
		"example, one":   {want: 1, nrocks: 1, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, two":   {want: 4, nrocks: 2, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, three": {want: 6, nrocks: 3, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, four":  {want: 7, nrocks: 4, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, five":  {want: 9, nrocks: 5, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, six":   {want: 10, nrocks: 6, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, seven": {want: 13, nrocks: 7, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, eight": {want: 15, nrocks: 8, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, nine":  {want: 17, nrocks: 9, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example, ten":   {want: 17, nrocks: 10, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
		"example":        {want: 3068, nrocks: 2022, input: []int8{1, 1, 1, -1, -1, 1, -1, 1, 1, -1, -1, -1, 1, 1, -1, 1, 1, 1, -1, -1, -1, 1, 1, 1, -1, -1, -1, 1, -1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, 1}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day17.Solve(tc.input, tc.nrocks)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 3071
Result of part 2: 1523615160362
`
	buff := new(bytes.Buffer)

	err := day17.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
