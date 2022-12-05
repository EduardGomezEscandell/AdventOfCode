package day03_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day03"
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
		want uint
	}{
		"empty":           {want: 0, data: []string(nil)},
		"example, line 1": {want: 16, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp"}},
		"example, line 2": {want: 38, data: []string{"jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL"}},
		"example, line 3": {want: 42, data: []string{"PmmdzqPrVvPwwTWBwg"}},
		"example, line 4": {want: 22, data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn"}},
		"example, line 5": {want: 20, data: []string{"ttgJtRGJQctTZtZT"}},
		"example, line 6": {want: 19, data: []string{"CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example":    {want: 157, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 3)
			got, err := day03.Part1(ch)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data []string
		want uint
	}{
		"empty":              {want: 0, data: []string(nil)},
		"example, line 1--3": {want: 18, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg"}},
		"example, line 4--6": {want: 52, data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example":       {want: 70, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 3)
			got, err := day03.Part2(ch)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 7826
Result of part 2: 2577
`
	buff := new(bytes.Buffer)

	err := day03.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
