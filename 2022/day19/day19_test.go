package day19_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day19"
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

type Blueprint = day19.Blueprint

func TestReadData(t *testing.T) {
	tc := map[string]struct {
		input []string
		want  []Blueprint
	}{
		"example": {input: []string{
			"Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.",
			"Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian.",
		},
			want: []Blueprint{
				{
					ID:                1,
					OreRobotCost:      [3]int{4, 0, 0},
					ClayRobotCost:     [3]int{2, 0, 0},
					ObsidianRobotCost: [3]int{3, 14, 0},
					GeodeRobotCost:    [3]int{2, 0, 7},
				}, {
					ID:                2,
					OreRobotCost:      [3]int{2, 0, 0},
					ClayRobotCost:     [3]int{3, 0, 0},
					ObsidianRobotCost: [3]int{3, 8, 0},
					GeodeRobotCost:    [3]int{3, 0, 12},
				},
			},
		},
	}

	for name, tc := range tc {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day19.ReadDataFile)()
			day19.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day19.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Blueprint
		want  int
	}{
		"empty": {want: 1, input: []Blueprint{}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day19.Part1(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Blueprint
		want  int
	}{
		"empty": {want: 1, input: []Blueprint{}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day19.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day19.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
