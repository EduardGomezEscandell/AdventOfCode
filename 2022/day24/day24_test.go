package day24_test

import (
	"bytes"
	"log"
	"os"
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

func TestPart1Part2(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		data   [][]rune
		wantP1 int
		wantP2 int
	}{
		"example": {
			data: [][]rune{
				[]rune("#.######"),
				[]rune("#>>.<^<#"),
				[]rune("#.<..<<#"),
				[]rune("#>v.><>#"),
				[]rune("#<^v^^>#"),
				[]rune("######.#"),
			},
			wantP1: 18, wantP2: 54,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day24.Part1(tc.data)
			require.NoError(t, err, "unexpected error in part 1")
			require.Equal(t, tc.wantP1, got, "unexpected result in part 1")

			got, err = day24.Part2(tc.data)
			require.NoError(t, err, "unexpected error in part 2")
			require.Equal(t, tc.wantP2, got, "unexpected result in part 2")
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 283
Result of part 2: 883
`
	buff := new(bytes.Buffer)

	err := day24.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
