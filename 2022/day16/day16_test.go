package day16_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day16"
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

type Valve = day16.Valve
type ID = day16.ID
type Score = day16.Score

func TestReadData(t *testing.T) {
	tc := map[string]struct {
		input     []string
		wantWorld []Valve
		wantStart ID
	}{
		"two": {
			input: []string{
				"Valve AA has flow rate=0; tunnels lead to valves BB",
				"Valve BB has flow rate=13; tunnels lead to valves AA"},
			wantWorld: []Valve{
				{Flowrate: 13, Paths: []ID{1}},
				{Flowrate: 0, Paths: []ID{0}},
			}, wantStart: 1,
		},
		"three": {
			input: []string{
				"Valve AA has flow rate=0; tunnels lead to valves FX, CD",
				"Valve FX has flow rate=193; tunnels lead to valves AA",
				"Valve CD has flow rate=2; tunnels lead to valves FX"},
			wantWorld: []Valve{
				{Flowrate: 193, Paths: []ID{2}},
				{Flowrate: 2, Paths: []ID{0}},
				{Flowrate: 0, Paths: []ID{0, 1}},
			}, wantStart: 2,
		},
		"example": {
			input: []string{
				"Valve AA has flow rate=0; tunnels lead to valves DD, II, BB",
				"Valve BB has flow rate=13; tunnels lead to valves CC, AA",
				"Valve CC has flow rate=2; tunnels lead to valves DD, BB",
				"Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE",
				"Valve EE has flow rate=3; tunnels lead to valves FF, DD",
				"Valve FF has flow rate=0; tunnels lead to valves EE, GG",
				"Valve GG has flow rate=0; tunnels lead to valves FF, HH",
				"Valve HH has flow rate=22; tunnel leads to valve GG",
				"Valve II has flow rate=0; tunnels lead to valves AA, JJ",
				"Valve JJ has flow rate=21; tunnel leads to valve II",
			}, wantWorld: []Valve{
				{Flowrate: 22, Paths: []ID{8}},
				{Flowrate: 21, Paths: []ID{9}},
				{Flowrate: 20, Paths: []ID{4, 5, 6}},
				{Flowrate: 13, Paths: []ID{5, 6}},
				{Flowrate: 3, Paths: []ID{2, 7}},
				{Flowrate: 2, Paths: []ID{2, 3}},
				{Flowrate: 0, Paths: []ID{2, 3, 9}},
				{Flowrate: 0, Paths: []ID{4, 8}},
				{Flowrate: 0, Paths: []ID{0, 7}},
				{Flowrate: 0, Paths: []ID{1, 6}},
			}, wantStart: 6,
		},
	}

	for name, tc := range tc {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day16.ReadDataFile)()
			day16.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			gotWorld, gotStart, err := day16.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.wantWorld, gotWorld)
			require.Equal(t, tc.wantStart, gotStart)
		})
	}

}

func TestSolve(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		world []Valve
		start ID
		time  int
		want  Score
	}{
		"two": {world: []Valve{
			0: {Flowrate: 10, Paths: []ID{1}},
			1: {Flowrate: 0, Paths: []ID{0}},
		}, start: 1, time: 4, want: Score(10 * 2),
		// 4: Goto 0
		// 3: Open 0
		// 2: Do whatever, valve 0 starts
		// 1: Do whatever
		// 0: Do whatever
		},
		"choose": {
			world: []Valve{
				{Flowrate: 100, Paths: []ID{1, 2}},
				{Flowrate: 1, Paths: []ID{0, 2}},
				{Flowrate: 0, Paths: []ID{0, 1}},
			}, start: 2, time: 5, want: Score(100*3 + 1*1),
			// 5: Goto 0
			// 4: Open 0
			// 3: Goto 1, valve 0 starts
			// 2: Open 1
			// 1: Do whatever, valve 1 starts
			// 0: Do whatever
		},
		"bypass": {
			world: []Valve{
				{Flowrate: 100, Paths: []ID{1}},
				{Flowrate: 1, Paths: []ID{0, 2}},
				{Flowrate: 0, Paths: []ID{1}},
			}, start: 2, time: 6, want: Score(100*3 + 1*1),
			// 6: Goto 1
			// 5: Goto 0
			// 4: Open 0
			// 3: Goto 1, valve 0 starts
			// 2: Open 1
			// 1: Do whatever, valve 1 starts
			// 0: Do whatever
		},
		"example": {
			world: []Valve{
				{Flowrate: 22, Paths: []ID{8}},
				{Flowrate: 21, Paths: []ID{9}},
				{Flowrate: 20, Paths: []ID{4, 5, 6}},
				{Flowrate: 13, Paths: []ID{5, 6}},
				{Flowrate: 3, Paths: []ID{2, 7}},
				{Flowrate: 2, Paths: []ID{2, 3}},
				{Flowrate: 0, Paths: []ID{2, 3, 9}},
				{Flowrate: 0, Paths: []ID{4, 8}},
				{Flowrate: 0, Paths: []ID{0, 7}},
				{Flowrate: 0, Paths: []ID{1, 6}},
			}, start: 6, time: 30, want: 1651,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day16.Solve(tc.world, tc.start, tc.time)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		input []Valve
		want  int
	}{
		"empty": {input: []Valve{}, want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day16.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 2379
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day16.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
