package day18_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day18"
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

type Cube = day18.Cube

func TestReadData(t *testing.T) {
	tc := map[string]struct {
		input []string
		want  []Cube
	}{
		"single": {input: []string{"1,2,3"}, want: []Cube{{1, 2, 3}}},
		"example": {input: []string{
			"2,2,2",
			"1,2,2",
			"3,2,2",
			"2,1,2",
			"2,3,2",
			"2,2,1",
			"2,2,3",
			"2,2,4",
			"2,2,6",
			"1,2,5",
			"3,2,5",
			"2,1,5",
			"2,3,5",
		}, want: []Cube{
			{2, 2, 2},
			{1, 2, 2},
			{3, 2, 2},
			{2, 1, 2},
			{2, 3, 2},
			{2, 2, 1},
			{2, 2, 3},
			{2, 2, 4},
			{2, 2, 6},
			{1, 2, 5},
			{3, 2, 5},
			{2, 1, 5},
			{2, 3, 5},
		}},
	}

	for name, tc := range tc {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day18.ReadDataFile)()
			day18.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day18.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Cube
		want  int
	}{
		"single": {want: 6, input: []Cube{{0, 0, 0}}},
		"two":    {want: 10, input: []Cube{{0, 0, 0}, {0, 0, 1}}},
		"example": {want: 64, input: []Cube{
			{2, 2, 2},
			{1, 2, 2},
			{3, 2, 2},
			{2, 1, 2},
			{2, 3, 2},
			{2, 2, 1},
			{2, 2, 3},
			{2, 2, 4},
			{2, 2, 6},
			{1, 2, 5},
			{3, 2, 5},
			{2, 1, 5},
			{2, 3, 5},
		}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day18.Part1(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Cube
		want  int
	}{
		"single": {want: 6, input: []Cube{{0, 0, 0}}},
		"two":    {want: 10, input: []Cube{{0, 0, 0}, {0, 0, 1}}},
		"example": {want: 58, input: []Cube{
			{2, 2, 2},
			{1, 2, 2},
			{3, 2, 2},
			{2, 1, 2},
			{2, 3, 2},
			{2, 2, 1},
			{2, 2, 3},
			{2, 2, 4},
			{2, 2, 6},
			{1, 2, 5},
			{3, 2, 5},
			{2, 1, 5},
			{2, 3, 5},
		}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day18.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 3470
Result of part 2: 1986
`
	buff := new(bytes.Buffer)

	err := day18.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
