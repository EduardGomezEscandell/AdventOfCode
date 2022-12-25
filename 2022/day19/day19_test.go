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
					Costs: [4][3]uint{
						Ore:      {4, 0, 0},
						Clay:     {2, 0, 0},
						Obsidian: {3, 14, 0},
						Geode:    {2, 0, 7},
					},
				}, {
					ID: 2,
					Costs: [4][3]uint{
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
		time  uint
		want  uint
	}{
		"example 1.1": {want: 9, time: 24, input: Blueprint{
			ID: 1,
			Costs: [4][3]uint{
				Ore:      {4, 0, 0},
				Clay:     {2, 0, 0},
				Obsidian: {3, 14, 0},
				Geode:    {2, 0, 7},
			},
		}},
		"example 1.2": {want: 12, time: 24, input: Blueprint{
			ID: 2,
			Costs: [4][3]uint{
				Ore:      {2, 0, 0},
				Clay:     {3, 0, 0},
				Obsidian: {3, 8, 0},
				Geode:    {3, 0, 12},
			},
		}},
		"example 2.1": {want: 56, time: 32, input: Blueprint{
			ID: 1,
			Costs: [4][3]uint{
				Ore:      {4, 0, 0},
				Clay:     {2, 0, 0},
				Obsidian: {3, 14, 0},
				Geode:    {2, 0, 7},
			},
		}},
		"example 2.2": {want: 62, time: 32, input: Blueprint{
			ID: 2,
			Costs: [4][3]uint{
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

			got := day19.SolveBlueprint(tc.input, tc.time)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Blueprint
		want  uint
	}{
		"example": {want: 33, input: []Blueprint{
			{
				ID: 1,
				Costs: [4][3]uint{
					Ore:      {4, 0, 0},
					Clay:     {2, 0, 0},
					Obsidian: {3, 14, 0},
					Geode:    {2, 0, 7},
				},
			}, {
				ID: 2,
				Costs: [4][3]uint{
					Ore:      {2, 0, 0},
					Clay:     {3, 0, 0},
					Obsidian: {3, 8, 0},
					Geode:    {3, 0, 12},
				},
			}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got := day19.Part1(tc.input)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []Blueprint
		want  uint
	}{
		"example": {want: 3472, input: []Blueprint{
			{
				ID: 1,
				Costs: [4][3]uint{
					Ore:      {4, 0, 0},
					Clay:     {2, 0, 0},
					Obsidian: {3, 14, 0},
					Geode:    {2, 0, 7},
				},
			}, {
				ID: 2,
				Costs: [4][3]uint{
					Ore:      {2, 0, 0},
					Clay:     {3, 0, 0},
					Obsidian: {3, 8, 0},
					Geode:    {3, 0, 12},
				},
			}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got := day19.Part2(tc.input)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1147
Result of part 2: 3080
`
	buff := new(bytes.Buffer)

	err := day19.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
