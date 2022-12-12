package day12_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day12"
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
		data [][]uint8
		want int
	}{
		"example": {data: strToUint8([]string{"Sabqponm", "abcryxxl", "accszExk", "acctuvwj", "abdefghi"}), want: 31},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day12.Part1(tc.data)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func strToUint8(s []string) [][]uint8 {
	data := make([][]uint8, 0, len(s))
	for _, line := range s {
		data = append(data, []uint8(line))
	}
	return data
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		data [][]uint8
		want int
	}{
		"example": {data: strToUint8([]string{"Sabqponm", "abcryxxl", "accszExk", "acctuvwj", "abdefghi"}), want: 29},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day12.Part2(tc.data)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 370
Result of part 2: 363
`
	buff := new(bytes.Buffer)

	err := day12.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
