package day04_test

import (
	"bytes"
	"context"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day04"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/stretchr/testify/require"
)

// func TestMain(m *testing.M) {
// 	err := testutils.CheckEnv()
// 	if err != nil {
// 		log.Printf("Setup: %v", err)
// 	}
// 	r := m.Run()
// 	os.Exit(r)
// }

func TestPart1(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data []string
		want uint
	}{
		"empty":                  {want: 0, data: []string(nil)},
		"example, line 1":        {want: 0, data: []string{"2-4,6-8"}},
		"example, line 2":        {want: 0, data: []string{"2-3,4-5"}},
		"example, line 3":        {want: 0, data: []string{"5-7,7-9"}},
		"example, line 4":        {want: 1, data: []string{"2-8,3-7"}},
		"example, line 5":        {want: 1, data: []string{"6-6,4-6"}},
		"example, line 6":        {want: 0, data: []string{"2-6,4-8"}},
		"example, lines 5 and 6": {want: 1, data: []string{"6-6,4-6", "2-6,4-8"}},
		"match":                  {want: 1, data: []string{"2-3,2-3"}},
		"multidigit, no overlap": {want: 0, data: []string{"12-15,19-33"}},
		"multidigit, overlap":    {want: 1, data: []string{"12-15,13-14"}},
		"full example":           {want: 2, data: []string{"2-4,6-8", "2-3,4-5", "5-7,7-9", "2-8,3-7", "6-6,4-6", "2-6,4-8"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := channel.FromArray(ctx, lines, 0)
			got, err := day04.Part1(ch)

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
		"empty":                  {want: 0, data: []string(nil)},
		"example, line 1":        {want: 0, data: []string{"2-4,6-8"}},
		"example, line 2":        {want: 0, data: []string{"2-3,4-5"}},
		"example, line 3":        {want: 1, data: []string{"5-7,7-9"}},
		"example, line 4":        {want: 1, data: []string{"2-8,3-7"}},
		"example, line 5":        {want: 1, data: []string{"6-6,4-6"}},
		"example, line 6":        {want: 1, data: []string{"2-6,4-8"}},
		"example, lines 5 and 6": {want: 2, data: []string{"6-6,4-6", "2-6,4-8"}},
		"single section overlap": {want: 1, data: []string{"2-3,3-5"}},
		"match":                  {want: 1, data: []string{"2-3,2-3"}},
		"multidigit, no overlap": {want: 0, data: []string{"12-15,19-33"}},
		"multidigit, overlap":    {want: 1, data: []string{"12-15,13-14"}},
		"full example":           {want: 4, data: []string{"2-4,6-8", "2-3,4-5", "5-7,7-9", "2-8,3-7", "6-6,4-6", "2-6,4-8"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
			defer cancel()

			ch := channel.FromArray(ctx, lines, 0)
			got, err := day04.Part2(ch)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 509
Result of part 2: 0
`
	buff := new(bytes.Buffer)

	err := day04.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
