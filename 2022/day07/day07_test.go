package day07_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"testing"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day07"
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
		// "single file": {want: 9, data: []string{"$ cd /", "$ ls", "9 a.txt"}},
		// "two  files":  {want: 5, data: []string{"$ cd /", "$ ls", "2 a.txt", "3 b.txt"}},
		// "nested":      {want: 410, data: []string{"$ cd /", "$ cd foo", "$ ls", "105 a.txt", "305 b.txt"}},
		// "repeated":    {want: 1, data: []string{"$ cd /", "$ ls", "1 a.txt", "$ ls", "1 a.txt"}},
		"example": {want: 95437, data: []string{"$ cd /", "$ ls", "dir a", "14848514 b.txt", "8504156 c.dat", "dir d", "$ cd a", "$ ls", "dir e", "29116 f", "2557 g", "62596 h.lst", "$ cd e", "$ ls", "584 i", "$ cd ..", "$ cd ..", "$ cd d", "$ ls", "4060174 j", "8033020 d.log", "5626152 d.ext", "7214296 k"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day07.Part1(ch)

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
		"empty": {data: []string{""}, want: 0},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			got, err := day07.Part2(ch)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 0
Result of part 2: 0
`
	buff := new(bytes.Buffer)

	err := day07.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
