package day22_test

import (
	"bytes"
	"fmt"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day22"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

type (
	Cell        = day22.Cell
	Instruction = day22.Instruction
	Direction   = day22.Turn
)

const (
	L = day22.CounterClockwise
	R = day22.Clockwise
)

func TestMain(m *testing.M) {
	err := testutils.CheckEnv()
	if err != nil {
		log.Printf("Setup: %v", err)
	}
	r := m.Run()
	os.Exit(r)
}

// t must be a filled rectangular matrix.
func transpose[T any](t [][]T) [][]T {
	if len(t) == 0 {
		return [][]T{}
	}

	u := array.Generate(len(t[0]), func() []T { return make([]T, len(t)) })

	for i := range t {
		for j := range t[i] {
			u[j][i] = t[i][j]
		}
	}

	return u
}

func TestReadInput(t *testing.T) {
	testCases := map[string]struct {
		input     []string
		wantWorld [][]Cell
		wantPath  []Instruction
	}{
		"example": {
			input: []string{
				"        ...#",
				"        .#..",
				"        #...",
				"        ....",
				"...#.......#",
				"........#...",
				"..#....#....",
				"..........#.",
				"        ...#....",
				"        .....#..",
				"        .#......",
				"        ......#.",
				"",
				"10R5L5R10L4R5L5",
			}, wantWorld: [][]Cell{
				[]Cell("        ...#    "),
				[]Cell("        .#..    "),
				[]Cell("        #...    "),
				[]Cell("        ....    "),
				[]Cell("...#.......#    "),
				[]Cell("........#...    "),
				[]Cell("..#....#....    "),
				[]Cell("..........#.    "),
				[]Cell("        ...#...."),
				[]Cell("        .....#.."),
				[]Cell("        .#......"),
				[]Cell("        ......#."),
			}, wantPath: []Instruction{{0, 10}, {R, 5}, {L, 5}, {R, 10}, {L, 4}, {R, 5}, {L, 5}},
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day22.ReadDataFile)()
			day22.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			gotWorld, gotPath, err := day22.ReadData()
			require.NoError(t, err)
			require.Equal(t, tc.wantWorld, gotWorld)
			require.Equal(t, tc.wantPath, gotPath)
		})
	}
}

func TestAdvance(t *testing.T) {
	t.Parallel()

	// Test worlds must be a single horizontal row.
	// Write test cases to test advance towards the right.
	// The tests will be transformed to also run their leftwards, upwards and downward equivalents.
	testCases := map[string]struct {
		world        [][]Cell
		inst         Instruction
		d, x, y      uint
		wantX, wantY int
	}{
		"success":             {world: [][]Cell{[]Cell(" ........")}, d: 2, x: 1, wantX: 3},
		"obstructed":          {world: [][]Cell{[]Cell(" ...#...")}, d: 10, x: 1, wantX: 3},
		"obstructed bug":      {world: [][]Cell{[]Cell(" ...#...")}, d: 3, x: 1, wantX: 3},
		"wrapped":             {world: [][]Cell{[]Cell(" ... ")}, d: 4, x: 1, wantX: 2},
		"wrapped eol":         {world: [][]Cell{[]Cell(" ...")}, d: 4, x: 1, wantX: 2},
		"wrapped twice":       {world: [][]Cell{[]Cell(" ... ")}, d: 7, x: 1, wantX: 2},
		"failed wrap":         {world: [][]Cell{[]Cell("    #...")}, d: 10, x: 5, wantX: 7},
		"failed wrap, padded": {world: [][]Cell{[]Cell("    #...     ")}, d: 10, x: 5, wantX: 7},
	}

	// -> Horizontal axis
	for name, tc := range testCases {
		tc := tc
		t.Run(fmt.Sprintf("horizontal/right/%s", name), func(t *testing.T) {
			t.Parallel()

			d := int(tc.d)
			x := int(tc.x)
			y := int(tc.y)

			err := day22.Advance(tc.world, &x, &y, day22.Right, d)
			require.NoError(t, err)

			require.Equal(t, tc.wantX, x)
			require.Equal(t, tc.wantY, y)
		})
	}

	// <- Horizontal axis
	for name, tc := range testCases {
		tc := tc
		t.Run(fmt.Sprintf("horizontal/left/%s", name), func(t *testing.T) {
			t.Parallel()

			// Mirroring world
			world := array.Map(tc.world, array.Reverse[Cell])
			d := int(tc.d)
			y := int(tc.y)
			x := len(world[y]) - int(tc.x) - 1
			wantX := len(world[y]) - tc.wantX - 1

			err := day22.Advance(world, &x, &y, day22.Left, d)
			require.NoError(t, err)

			require.Equal(t, wantX, x)
			require.Equal(t, tc.wantY, y)
		})
	}

	// v Vertical axis
	for name, tc := range testCases {
		tc := tc
		t.Run(fmt.Sprintf("vertical/down/%s", name), func(t *testing.T) {
			t.Parallel()

			// Transposing test case (back--forth becomes up--down)
			world := transpose(tc.world)
			d := int(tc.d)
			x := int(tc.y)
			y := int(tc.x)
			wantX := tc.wantY
			wantY := tc.wantX

			err := day22.Advance(world, &x, &y, day22.Down, d)
			require.NoError(t, err)

			require.Equal(t, wantX, x)
			require.Equal(t, wantY, y)
		})
	}

	// ^ Vertical axis
	for name, tc := range testCases {
		tc := tc
		t.Run(fmt.Sprintf("vertical/up/%s", name), func(t *testing.T) {
			t.Parallel()

			// Transposing and mirroring test case
			world := transpose(tc.world)
			world = array.Reverse(world)
			d := int(tc.d)
			x := int(tc.y)
			y := len(world) - int(tc.x) - 1
			wantX := tc.wantY
			wantY := len(world) - tc.wantX - 1

			err := day22.Advance(world, &x, &y, day22.Up, d)
			require.NoError(t, err)

			require.Equal(t, wantX, x)
			require.Equal(t, wantY, y)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		world [][]Cell
		path  []Instruction
		want  int
	}{
		"example": {world: [][]Cell{
			[]Cell("        ...#    "),
			[]Cell("        .#..    "),
			[]Cell("        #...    "),
			[]Cell("        ....    "),
			[]Cell("...#.......#    "),
			[]Cell("........#...    "),
			[]Cell("..#....#....    "),
			[]Cell("..........#.    "),
			[]Cell("        ...#...."),
			[]Cell("        .....#.."),
			[]Cell("        .#......"),
			[]Cell("        ......#."),
		}, path: []Instruction{{0, 10}, {R, 5}, {L, 5}, {R, 10}, {L, 4}, {R, 5}, {L, 5}},
			want: 6032,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day22.Part1(tc.world, tc.path)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input [][]Cell
		want  int64
	}{
		"empty": {want: 1, input: [][]Cell{}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day22.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 29408
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day22.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
