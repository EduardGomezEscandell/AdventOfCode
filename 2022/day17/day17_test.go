package day17_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day17"
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

func TestReadData(t *testing.T) {
	tc := map[string]struct {
		input []string
		want  int
	}{
		"empty": {input: []string{}, want: 1},
	}

	for name, tc := range tc {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day17.ReadDataFile)()
			day17.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day17.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		want int
	}{
		"empty": {want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day17.Part1()

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		want int
	}{
		"empty": {want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day17.Part2()

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 1
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day17.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
