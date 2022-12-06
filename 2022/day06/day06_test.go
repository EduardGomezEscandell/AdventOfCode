package day06_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day06"
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
	testCases := map[string]int{
		"abcdxxxxxxxx":                      4,
		"xxxxxxxxabcd":                      11,
		"mjqjpqmgbljsphdztnvjfqwrcgsmlb":    7,
		"bvwbjplbgvbhsrlpgdmjqwftvncz":      5,
		"nppdvjthqldpwncqszvftbrmjlhg":      6,
		"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg": 10,
		"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw":  11,
		"xxxxbbxabcd":                       10,
	}

	for name, want := range testCases {
		name := name
		want := want
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day06.Part1([]byte(name))
			require.NoError(t, err)
			require.Equal(t, want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()
	testCases := map[string]int{
		"mjqjpqmgbljsphdztnvjfqwrcgsmlb":    19,
		"bvwbjplbgvbhsrlpgdmjqwftvncz":      23,
		"nppdvjthqldpwncqszvftbrmjlhg":      23,
		"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg": 29,
		"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw":  26,
	}

	for name, want := range testCases {
		name := name
		want := want
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day06.Part2([]byte(name))
			require.NoError(t, err)
			require.Equal(t, want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1855
Result of part 2: 3256
`
	buff := new(bytes.Buffer)

	err := day06.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
