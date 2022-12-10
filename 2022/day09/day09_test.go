package day09_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day09"
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

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data []string
		want int
	}{
		"empty":   {data: []string{}, want: 1},
		"single":  {data: []string{"R 4"}, want: 4},
		"L":       {data: []string{"D 4", "R 4"}, want: 7},
		"loop":    {data: []string{"U 1", "R 1", "D 1", "L 1"}, want: 1},
		"example": {data: []string{"R 4", "U 4", "L 3", "D 1", "R 4", "D 1", "L 5", "R 2"}, want: 13},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day09.Part1(ch)

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
		"empty":     {data: []string{}, want: 1},
		"few":       {data: []string{"R 8"}, want: 1},
		"extend":    {data: []string{"R 10"}, want: 2},
		"example":   {data: []string{"R 4", "U 4", "L 3", "D 1", "R 4", "D 1", "L 5", "R 2"}, want: 1},
		"example 2": {data: []string{"R 5", "U 8", "L 8", "D 3", "R 17", "D 10", "L 25", "U 20"}, want: 36},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day09.Part2(ch)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 6332
Result of part 2: 2511
`
	buff := new(bytes.Buffer)

	err := day09.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
