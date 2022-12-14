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
		"single":        {data: []string{"3,5 -> 3,8"}, want: [][2]Location{{{3, 5}, {3, 8}}}},
		"two":           {data: []string{"3,5 -> 3,8", "1,3 -> 1,1"}, want: [][2]Location{{{3, 5}, {3, 8}}, {{1, 1}, {1, 3}}}},
		"double-decker": {data: []string{"2,6 -> 2,9 -> 1,9"}, want: [][2]Location{{{2, 6}, {2, 9}}, {{1, 9}, {2, 9}}}},
		"example": {data: []string{"498,4 -> 498,6 -> 496,6", "503,4 -> 502,4 -> 502,9 -> 494,9"},
			want: [][2]Location{
				{{498, 4}, {498, 6}},
				{{496, 6}, {498, 6}},
				{{502, 4}, {503, 4}},
				{{502, 4}, {502, 9}},
				{{494, 9}, {502, 9}},
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
	R = day14.Rock
	a = day14.Air
	S = day14.Sand
)

func TestAssembleWorld(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data       [][2]Location
		sourceX    int
		wantWorld  [][]Cell
		wantOffset int
	}{
		"horizontal": {data: [][2]Location{{{0, 0}, {2, 0}}}, sourceX: 0,
			wantOffset: -1, wantWorld: [][]Cell{
				{a, R, R, R, a},
				{a, a, a, a, a},
			}},
		"vertical": {data: [][2]Location{{{0, 0}, {0, 2}}}, sourceX: 0,
			wantOffset: -3, wantWorld: [][]Cell{
				{a, a, a, R, a, a, a}, // 0
				{a, a, a, R, a, a, a}, // 1
				{a, a, a, R, a, a, a}, // 2
				{a, a, a, a, a, a, a}, // 3
			}},
		"L": {data: [][2]Location{{{0, 1}, {1, 1}}, {{0, 1}, {0, 2}}}, sourceX: 0,
			wantOffset: -3, wantWorld: [][]Cell{
				{a, a, a, a, a, a, a, a}, // 0
				{a, a, a, R, R, a, a, a}, // 1
				{a, a, a, R, a, a, a, a}, // 2
				{a, a, a, a, a, a, a, a}, // 3
			}},
		"offset L": {data: [][2]Location{{{10, 3}, {11, 3}}, {{10, 3}, {10, 4}}}, sourceX: 10,
			wantOffset: 5, wantWorld: [][]Cell{
				{a, a, a, a, a, a, a, a, a, a, a, a}, // 0
				{a, a, a, a, a, a, a, a, a, a, a, a}, // 1
				{a, a, a, a, a, a, a, a, a, a, a, a}, // 2
				{a, a, a, a, a, R, R, a, a, a, a, a}, // 3
				{a, a, a, a, a, R, a, a, a, a, a, a}, // 4
				{a, a, a, a, a, a, a, a, a, a, a, a}, // 5
			}},
		"example": {data: [][2]Location{
			{{498, 4}, {498, 6}},
			{{496, 6}, {498, 6}},
			{{502, 4}, {503, 4}},
			{{502, 4}, {502, 9}},
			{{494, 9}, {502, 9}}}, sourceX: 500,
			wantOffset: 484, wantWorld: [][]Cell{
				//                           |494   496   498   500   502  |
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}, // 0
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}, // 1
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}, // 2
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}, // 3
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, R, a, a, a, R, R, a, a, a, a, a, a, a, a, a, a}, // 4
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, R, a, a, a, R, a, a, a, a, a, a, a, a, a, a, a}, // 5
				{a, a, a, a, a, a, a, a, a, a, a, a, R, R, R, a, a, a, R, a, a, a, a, a, a, a, a, a, a, a}, // 6
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, R, a, a, a, a, a, a, a, a, a, a, a}, // 7
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, R, a, a, a, a, a, a, a, a, a, a, a}, // 8
				{a, a, a, a, a, a, a, a, a, a, R, R, R, R, R, R, R, R, R, a, a, a, a, a, a, a, a, a, a, a}, // 9
				{a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}, // 10
				//                           |494   496   498   500   502  |
			}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			gotWorld, gotOffset := day14.AssembleWorld(tc.data, tc.sourceX)

			require.Equalf(t, tc.wantOffset, gotOffset, "Want:\n%s\nGot:\n%s", day14.StrWorld(tc.wantWorld), day14.StrWorld(gotWorld))
			require.Equalf(t, tc.wantWorld, gotWorld, "Want:\n%s\nGot:\n%s", day14.StrWorld(tc.wantWorld), day14.StrWorld(gotWorld))
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		input [][2]Location
		want  int
	}{
		"example": {input: [][2]Location{
			{{498, 4}, {498, 6}},
			{{496, 6}, {498, 6}},
			{{502, 4}, {503, 4}},
			{{502, 4}, {502, 9}},
			{{494, 9}, {502, 9}},
		}, want: 24},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			world, offset := day14.AssembleWorld(tc.input, day14.SourceX)
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
		offset int
		want   int
	}{
		// "empty": {world: [][]Cell{}, offset: Box{0, 0, 0, 0}, want: 0},
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
