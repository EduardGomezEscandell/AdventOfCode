package day14_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day14"
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

type Location = day14.Location
type Cell = day14.Cell

func TestReadInput(t *testing.T) {
	testCases := map[string]struct {
		data []string
		want [][2]Location
	}{
		"single":        {data: []string{"3,5 -> 3,8"}, want: [][2]Location{{{3, -8}, {3, -5}}}},
		"two":           {data: []string{"3,5 -> 3,8", "1,3 -> 1,1"}, want: [][2]Location{{{3, -8}, {3, -5}}, {{1, -3}, {1, -1}}}},
		"double-decker": {data: []string{"2,6 -> 2,9 -> 1,9"}, want: [][2]Location{{{2, -9}, {2, -6}}, {{1, -9}, {2, -9}}}},
		"example": {data: []string{"498,4 -> 498,6 -> 496,6", "503,4 -> 502,4 -> 502,9 -> 494,9"},
			want: [][2]Location{
				{{498, -6}, {498, -4}},
				{{496, -6}, {498, -6}},
				{{502, -4}, {503, -4}},
				{{502, -9}, {502, -4}},
				{{494, -9}, {502, -9}},
			},
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day14.ReadDataFile)()
			day14.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.data, "\n")), nil
			}

			got, err := day14.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.want, got)

		})
	}
}

const (
	Rock = day14.Rock
	Air  = day14.Air
	Sand = day14.Sand
)

func TestAssembleWorld(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data       [][2]Location
		wantWorld  [][]Cell
		wantOffset Location
	}{
		"horizontal": {data: [][2]Location{{{0, 0}, {2, 0}}}, wantOffset: Location{-1, 0}, wantWorld: [][]Cell{{Air, Rock, Rock, Rock, Air}}},
		"vertical":   {data: [][2]Location{{{0, -2}, {0, 0}}}, wantOffset: Location{-1, -2}, wantWorld: [][]Cell{{Air, Rock, Air}, {Air, Rock, Air}, {Air, Rock, Air}}},
		"L":          {data: [][2]Location{{{0, 0}, {1, 0}}, {{0, -1}, {0, 0}}}, wantOffset: Location{-1, -1}, wantWorld: [][]Cell{{Air, Rock, Air, Air}, {Air, Rock, Rock, Air}}},
		"offset L":   {data: [][2]Location{{{10, -20}, {11, -20}}, {{10, -21}, {10, -20}}}, wantOffset: Location{9, -21}, wantWorld: [][]Cell{{Air, Rock, Air, Air}, {Air, Rock, Rock, Air}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			gotWorld, gotOffset := day14.AssembleWorld(tc.data)

			require.Equal(t, tc.wantOffset, gotOffset)
			require.Equal(t, tc.wantWorld, gotWorld)
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		input [][2]Location
		want  uint
	}{
		"example": {input: [][2]Location{
			{{498, -6}, {498, -4}},
			{{496, -6}, {498, -6}},
			{{502, -4}, {503, -4}},
			{{502, -9}, {502, -4}},
			{{494, -9}, {502, -9}},
		}, want: 24},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			world, offset := day14.AssembleWorld(tc.input)
			got, err := day14.Part1(world, offset)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		world  [][]Cell
		offset Location
		want   uint
	}{
		"empty": {world: [][]Cell{}, offset: Location{0, 0}, want: 0},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day14.Part2(tc.world, tc.offset)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 618
Result of part 2: 0
`
	buff := new(bytes.Buffer)

	err := day14.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
