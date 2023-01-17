package input_test

import (
	"context"
	"os"
	"path/filepath"
	"strings"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestFilename(t *testing.T) {
	cwd, err := os.Getwd()
	require.NoError(t, err)

	testCases := map[string]struct {
		root     string
		day      uint
		filename string
		want     string
		wantErr  bool
	}{
		"one digit day": {root: "/tmp/", day: 1, filename: "data.txt", want: "/tmp/data/01/data.txt"},
		"two digit day": {root: "/tmp/dir/", day: 13, filename: "things.csv", want: "/tmp/dir/data/13/things.csv"},
		"no filename":   {root: "/tmp/dir/", day: 5, wantErr: true},
		"no env":        {day: 5, filename: "data.tar.gz", want: filepath.Join(cwd, "data/05/data.tar.gz")},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			os.Setenv(input.EnvRootDir, tc.root)
			tc.want = filepath.Clean(tc.want)

			got, err := input.DataFullPath(tc.day, tc.filename)

			if tc.wantErr {
				require.Error(t, err)
				return
			}
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestReadDataAsync(t *testing.T) {
	t.Parallel()

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

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			ctx, cancel := context.WithTimeout(context.Background(), time.Second)
			defer cancel()

			ch, err := input.ReadDataAsync(ctx, testutils.NewMockReadCloser(t, strings.Join(tc.data, "\n")), 0)
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
