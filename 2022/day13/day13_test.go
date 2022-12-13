package day13_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day13"
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

func TestParse(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data string
	}{
		"empty":         {data: "[]"},
		"single":        {data: "[1]"},
		"couple":        {data: "[1,3]"},
		"nesting":       {data: "[1,[3,5]]"},
		"double digits": {data: "[1,[35,7]]"},
		"example first": {data: "[1,1,3,1,1]"},
		"example last":  {data: "[1,[2,[3,[4,[5,6,0]]]],8,9]"},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			input := tc.data
			tk, err := day13.Parse(input)
			require.NoError(t, err)
			got := day13.PrettyPrint(tk)
			require.Equal(t, tc.data, got)
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data []string
		want int
	}{
		"ok":        {data: []string{"[1,2,3,4]", "[1,2,5]"}, want: 1},
		"not":       {data: []string{"[1,2,3,4]", "[1,2,3]"}, want: 0},
		"example 1": {data: []string{"[1,1,3,1,1]", "[1,1,5,1,1]"}, want: 1},
		"example 2": {data: []string{"[1,1,3,1,1]", "[1,1,5,1,1]", "", "[[1],[2,3,4]]", "[[1],4]"}, want: 3},
		"example 3": {data: []string{"[1,1,3,1,1]", "[1,1,5,1,1]", "", "[[1],[2,3,4]]", "[[1],4]", "", "[9]", "[[8,7,6]]"}, want: 3},
		"example": {data: []string{
			"[1,1,3,1,1]",
			"[1,1,5,1,1]",
			"",
			"[[1],[2,3,4]]",
			"[[1],4]",
			"",
			"[9]",
			"[[8,7,6]]",
			"",
			"[[4,4],4,4]",
			"[[4,4],4,4,4]",
			"",
			"[7,7,7,7]",
			"[7,7,7]",
			"",
			"[]",
			"[3]",
			"",
			"[[[]]]",
			"[[]]",
			"",
			"[1,[2,[3,[4,[5,6,7]]]],8,9]",
			"[1,[2,[3,[4,[5,6,0]]]],8,9]",
		}, want: 13},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day13.Part1(day13.ParseInput((ch)))

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data []string
		want int
	}{
		"example": {data: []string{
			"[1,1,3,1,1]",
			"[1,1,5,1,1]",
			"",
			"[[1],[2,3,4]]",
			"[[1],4]",
			"",
			"[9]",
			"[[8,7,6]]",
			"",
			"[[4,4],4,4]",
			"[[4,4],4,4,4]",
			"",
			"[7,7,7,7]",
			"[7,7,7]",
			"",
			"[]",
			"[3]",
			"",
			"[[[]]]",
			"[[]]",
			"",
			"[1,[2,[3,[4,[5,6,7]]]],8,9]",
			"[1,[2,[3,[4,[5,6,0]]]],8,9]",
		}, want: 140},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day13.Part2(day13.ParseInput((ch)))

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 5882
Result of part 2: 24948
`
	buff := new(bytes.Buffer)

	err := day13.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
