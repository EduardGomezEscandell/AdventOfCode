package day24_test

import (
	"bytes"
	"log"
	"os"
	"strings"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day24"
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

func TestReadInput(t *testing.T) {
	testCases := map[string]struct {
		input []string
		want  []int
	}{
		"example": {
			input: []string{""}, want: nil,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day24.ReadDataFile)()
			day24.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.input, "\n")), nil
			}

			got, err := day24.ReadData()
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		want int
	}{
		"empty": {want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day24.Part1()

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		want int
	}{
		"empty": {want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day24.Part2()

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 4218
Result of part 2: 976
`
	buff := new(bytes.Buffer)

	err := day24.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
