package day05_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day05"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
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

func TestParseInitalState(t *testing.T) {
	t.Parallel()
	example := []string{
		"    [D]    ", // 0
		"[N] [C]    ", // 1
		"[Z] [M] [P]", // 2
		" 1   2   3 ", // 3
	}
	want := [][]rune{
		{'Z', 'N'},
		{'M', 'C', 'D'},
		{'P'},
	}

	lines := array.Map(example, input.NewLine)
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	ch := charray.FromArray(ctx, lines, 0)
	gotstacks, err := day05.ParseInitalState(ch)
	require.NoError(t, err)

	internalData := array.Map(gotstacks, func(s day05.Stack) []rune { return s.Data() })
	require.Equal(t, want, internalData)
}

func TestSolve(t *testing.T) {
	fullExample := []string{
		"    [D]    ",        // 0
		"[N] [C]    ",        // 1
		"[Z] [M] [P]",        // 2
		" 1   2   3 ",        // 3
		"",                   // 4
		"move 1 from 2 to 1", // 5
		"move 3 from 1 to 3", // 6
		"move 2 from 2 to 1", // 7
		"move 1 from 1 to 2", // 8
	}

	testCases := map[string]struct {
		data  []string
		crane func([]day05.Stack, day05.Instruction)
		want  string
	}{
		// Part 1
		"crane9000 empty":   {data: fullExample[:4], want: "NDP", crane: day05.Crane9000},
		"crane9000 space":   {data: fullExample[:5], want: "NDP", crane: day05.Crane9000},
		"crane9000 one":     {data: fullExample[:6], want: "DCP", crane: day05.Crane9000},
		"crane9000 example": {data: fullExample, want: "CMZ", crane: day05.Crane9000},
		// Part 2
		"crane9001 empty":   {data: fullExample[:4], want: "NDP", crane: day05.Crane9001},
		"crane9001 example": {data: fullExample, want: "MCD", crane: day05.Crane9001},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day05.Solve(ch, tc.crane)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: TLFGBZHCN
Result of part 2: QRQFHFWCL
`
	buff := new(bytes.Buffer)

	err := day05.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
