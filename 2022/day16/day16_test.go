package day16_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day16"
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

type Valve = day16.Valve
type ID = day16.ID

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		input map[ID]Valve
		want  int
	}{
		"empty": {input: map[ID]Valve{}, want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day16.Part1(tc.input)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		input map[ID]Valve
		want  int
	}{
		"empty": {input: map[ID]Valve{}, want: 1},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day16.Part2(tc.input)

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

	err := day16.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
