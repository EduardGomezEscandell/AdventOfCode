package testutils_test

import (
	"bufio"
	"io"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day03"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/testutils"
	"github.com/stretchr/testify/require"
)

func TestStringSliceReader(t *testing.T) {
	testCases := map[string]struct {
		data []string
	}{
		"empty":                       {data: []string(nil)},
		"day 3 example line 1":        {data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp"}},
		"day 3 example lines 2 and 3": {data: []string{"jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg"}},
		"day 3 example lines 4-6":     {data: []string{"wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
		"full example":                {data: []string{"vJrwpWtwJgWrhcsFMMfFFhFp", "jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL", "PmmdzqPrVvPwwTWBwg", "wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn", "ttgJtRGJQctTZtZT", "CrZsJsPPZsGzwwsLwLmpwMDw"}},
	}

	defer testutils.Backup(&day03.DataReader)()

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			var r io.ReadCloser
			r = testutils.NewMockReadCloser(t, strings.Join(tc.data, "\n"))

			var output []string
			scanner := bufio.NewScanner(r)
			for i := 0; scanner.Scan(); i++ {
				output = append(output, scanner.Text())
			}

			require.NoError(t, scanner.Err())
			require.Equal(t, tc.data, output)

			r.Close()
		})
	}
}
