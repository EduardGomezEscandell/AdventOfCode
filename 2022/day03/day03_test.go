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
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
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
		data   []string
		buffer int
	}{
		"empty":                            {buffer: 0, data: []string(nil)},
		"empty, buffered":                  {buffer: 1, data: []string(nil)},
		"example, line 1":                  {buffer: 0, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp"}},
		"example, line 1, buffered":        {buffer: 3, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp"}},
		"example, lines 2 and 3":           {buffer: 0, data: []string{"jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg"}},
		"example, lines 2 and 3, buffered": {buffer: 4, data: []string{"jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg"}},
		"example, line 4-6":                {buffer: 0, data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"example, line 4-6, buffered":      {buffer: 2, data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example":                     {buffer: 0, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example, buffered":           {buffer: 5, data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
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

			ch, err := day03.ReadInputAsync(ctx, 0)
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

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			lines := array.Map(tc.data, day03.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := channel.FromArray(ctx, lines, 3)
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
			lines := array.Map(tc.data, day03.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := channel.FromArray(ctx, lines, 3)
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
