package day07_test

import (
	"bytes"
	"context"
	"log"
	"os"
	"strings"
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
		"single file": {want: 9, data: []string{"$ cd /", "$ ls", "9 a.txt"}},
		"two  files":  {want: 5, data: []string{"$ cd /", "$ ls", "2 a.txt", "3 b.txt"}},
		"nested":      {want: 820, data: []string{"$ cd /", "$ ls", "dir foo", "$ cd foo", "$ ls", "105 a.txt", "305 b.txt"}},
		"repeated":    {want: 1, data: []string{"$ cd /", "$ ls", "1 a.txt", "$ ls", "1 a.txt"}},
		"example":     {want: 95437, data: []string{"$ cd /", "$ ls", "dir a", "14848514 b.txt", "8504156 c.dat", "dir d", "$ cd a", "$ ls", "dir e", "29116 f", "2557 g", "62596 h.lst", "$ cd e", "$ ls", "584 i", "$ cd ..", "$ cd ..", "$ cd d", "$ ls", "4060174 j", "8033020 d.log", "5626152 d.ext", "7214296 k"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			fs, err := day07.AssembleFilesystem(ch)
			t.Logf(day07.String(fs))
			require.NoError(t, err, "Failed to read filesystem")

			got, err := day07.Part1(fs)

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
		"small":   {want: 69000000, data: []string{"$ cd /", "$ ls", "69000000 b.txt"}},
		"two":     {want: 30000000, data: []string{"$ cd /", "$ ls", "dir a", "dir b", "$ cd a", "$ ls", "40000000 a.txt", "$ cd ..", "$ cd b", "$ ls", "30000000 b.txt"}},
		"example": {want: 24933642, data: []string{"$ cd /", "$ ls", "dir a", "14848514 b.txt", "8504156 c.dat", "dir d", "$ cd a", "$ ls", "dir e", "29116 f", "2557 g", "62596 h.lst", "$ cd e", "$ ls", "584 i", "$ cd ..", "$ cd ..", "$ cd d", "$ ls", "4060174 j", "8033020 d.log", "5626152 d.ext", "7214296 k"}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			lines := array.Map(tc.data, input.NewLine)
			ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
			defer cancel()

			ch := charray.FromArray(ctx, lines, 0)
			fs, err := day07.AssembleFilesystem(ch)
			t.Log(day07.String(fs))
			require.NoError(t, err, "Failed to read filesystem")

			got, err := day07.Part2(fs)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

// TestAssembleFilesystem tests that the example filesystem is properly
// parsed.
func TestAssembleFilesystem(t *testing.T) {
	data := `$ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k`

	want := `- / (dir)
  - a (dir)
    - e (dir)
      - i (file, size=584)
    - f (file, size=29116)
    - g (file, size=2557)
    - h.lst (file, size=62596)
  - b.txt (file, size=14848514)
  - c.dat (file, size=8504156)
  - d (dir)
    - j (file, size=4060174)
    - d.log (file, size=8033020)
    - d.ext (file, size=5626152)
    - k (file, size=7214296)`

	// Reading through channel
	lines := array.Map(strings.Split(data, "\n"), input.NewLine)
	ctx, cancel := context.WithTimeout(context.Background(), 100000*time.Second)
	defer cancel()
	ch := charray.FromArray(ctx, lines, 0)

	// Partsing
	fs, err := day07.AssembleFilesystem(ch)
	got := day07.String(fs)
	t.Log(got)
	require.NoError(t, err)
	require.Equal(t, want, got)
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1642503
Result of part 2: 6999588
`
	buff := new(bytes.Buffer)

	err := day07.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
