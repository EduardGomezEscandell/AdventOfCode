package day23_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day23"
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

type (
	Location = day23.Location
	Elf      = day23.Elf
)

func TestReadInput(t *testing.T) {
	testCases := map[string]struct {
		input []string
		want  map[Location]Elf
	}{
		"example": {
			input: []string{
				//123456
				"....#..", // 0
				"..###.#", // 1
				"#...#.#", // 2
				".#...##", // 3
				"#.###..", // 4
				"##.#.##", // 5
				".#..#..", // 6
			}, want: map[Location]day23.Elf{
				{0, 4}: {},
				{1, 2}: {}, {1, 3}: {}, {1, 4}: {}, {1, 6}: {},
				{2, 0}: {}, {2, 4}: {}, {2, 6}: {},
				{3, 1}: {}, {3, 5}: {}, {3, 6}: {},
				{4, 0}: {}, {4, 2}: {}, {4, 3}: {}, {4, 4}: {},
				{5, 0}: {}, {5, 1}: {}, {5, 3}: {}, {5, 5}: {}, {5, 6}: {},
				{6, 1}: {}, {6, 4}: {},
			},
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day23.ReadDataFile)()
			day23.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day23.ReadData()
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		input map[Location]Elf
		want  int
	}{
		"example": {
			input: map[Location]day23.Elf{
				{0, 4}: {},
				{1, 2}: {}, {1, 3}: {}, {1, 4}: {}, {1, 6}: {},
				{2, 0}: {}, {2, 4}: {}, {2, 6}: {},
				{3, 1}: {}, {3, 5}: {}, {3, 6}: {},
				{4, 0}: {}, {4, 2}: {}, {4, 3}: {}, {4, 4}: {},
				{5, 0}: {}, {5, 1}: {}, {5, 3}: {}, {5, 5}: {}, {5, 6}: {},
				{6, 1}: {}, {6, 4}: {},
			},
			want: 110,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day23.Part1(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		input map[Location]Elf
		want  int
	}{
		"example": {
			input: map[Location]day23.Elf{
				{0, 4}: {},
				{1, 2}: {}, {1, 3}: {}, {1, 4}: {}, {1, 6}: {},
				{2, 0}: {}, {2, 4}: {}, {2, 6}: {},
				{3, 1}: {}, {3, 5}: {}, {3, 6}: {},
				{4, 0}: {}, {4, 2}: {}, {4, 3}: {}, {4, 4}: {},
				{5, 0}: {}, {5, 1}: {}, {5, 3}: {}, {5, 5}: {}, {5, 6}: {},
				{6, 1}: {}, {6, 4}: {},
			},
			want: 20,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day23.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 4218
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day23.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
