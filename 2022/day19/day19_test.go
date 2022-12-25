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

type (
	Blueprint = day19.Blueprint
	Material  = day19.Material
)

const (
	Ore      = day19.Ore
	Clay     = day19.Clay
	Obsidian = day19.Obsidian
	Geode    = day19.Geode
)

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
					ID: 1,
					Costs: [4][3]int{
						Ore:      {4, 0, 0},
						Clay:     {2, 0, 0},
						Obsidian: {3, 14, 0},
						Geode:    {2, 0, 7},
					},
				}, {
					ID: 2,
					Costs: [4][3]int{
						Ore:      {2, 0, 0},
						Clay:     {3, 0, 0},
						Obsidian: {3, 8, 0},
						Geode:    {3, 0, 12},
					},
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

func TestSolveBlueprint(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input Blueprint
		want  int
	}{
		"example 1": {want: 9, input: Blueprint{
			ID: 1,
			Costs: [4][3]int{
				Ore:      {4, 0, 0},
				Clay:     {2, 0, 0},
				Obsidian: {3, 14, 0},
				Geode:    {2, 0, 7},
			},
		}},
		"example 2": {want: 12, input: Blueprint{
			ID: 2,
			Costs: [4][3]int{
				Ore:      {2, 0, 0},
				Clay:     {3, 0, 0},
				Obsidian: {3, 8, 0},
				Geode:    {3, 0, 12},
			},
		}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day19.SolveBlueprint(tc.input)

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
