package day20_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day20"
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

func TestPart1(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []int
		want  int64
	}{
		"example": {want: 3, input: []int{1, 2, -3, 3, -2, 0, 4}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day20.Part1(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) { // nolint: dupl
	t.Parallel()

	testCases := map[string]struct {
		input []int
		want  int64
	}{
		"example": {want: 1623178306, input: []int{1, 2, -3, 3, -2, 0, 4}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day20.Part2(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 7153
Result of part 2: 6146976244822
`
	buff := new(bytes.Buffer)

	err := day20.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
