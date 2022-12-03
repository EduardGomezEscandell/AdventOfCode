package day03_test

import (
	"bytes"
	"context"
	"io"
	"log"
	"os"
	"strings"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day03"
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

func TestReadInputAsync(t *testing.T) {
	testCases := map[string]struct {
		data []string
	}{
		"empty":                  {data: []string(nil)},
		"example, line 1":        {data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp"}},
		"example, lines 2 and 3": {data: []string{"jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg"}},
		"example, line 4-6":      {data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example":           {data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
	}

	defer testutils.Backup(&day03.DataReader)()

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			day03.DataReader = func() (io.ReadCloser, error) {
				fileContents := strings.Join(tc.data, "\n")
				return testutils.NewMockReadCloser(t, fileContents), nil
			}

			ctx, cancel := context.WithTimeout(context.Background(), time.Minute)
			defer cancel()

			ch, err := day03.ReadInputAsync(ctx)
			require.NoError(t, err)

			var output []string
			for v := range ch {
				require.NoError(t, v.Err())
				output = append(output, v.Str())
			}

			require.Equal(t, tc.data, output)
		})
	}
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
	defer testutils.Backup(&day03.DataReader)()

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			day03.DataReader = func() (io.ReadCloser, error) {
				fileContents := strings.Join(tc.data, "\n")
				return testutils.NewMockReadCloser(t, fileContents), nil
			}

			got, err := day03.Part1()
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

// func TestPart2(t *testing.T) {
// 	t.Parallel()
// 	testCases := map[string]struct {
// 		data []Round
// 		want uint
// 	}{
// 		"empty":   {want: 0, data: []Round{}},
// 		"one":     {want: 8, data: []Round{{ROCK, Win}}},
// 		"first":   {want: 4, data: []Round{{ROCK, Tie}}},
// 		"second":  {want: 1, data: []Round{{PAPER, Lose}}},
// 		"third":   {want: 7, data: []Round{{SCISSORS, Win}}},
// 		"example": {want: 12, data: []Round{{ROCK, Tie}, {PAPER, Lose}, {SCISSORS, Win}}},
// 	}

// 	for name, tc := range testCases {
// 		tc := tc
// 		t.Run(name, func(t *testing.T) {
// 			t.Parallel()
// 			got, err := day03.Part2(tc.data)
// 			require.NoError(t, err)
// 			require.Equal(t, tc.want, got)
// 		})
// 	}
// }

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 7826
`
	buff := new(bytes.Buffer)

	err := day03.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
