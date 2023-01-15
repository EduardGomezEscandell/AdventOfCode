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

func TestPart1(t *testing.T) { // nolint: dupl
	testCases := map[string]struct {
		data [][]rune
		want int
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
			want: 18,
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			got, err := day24.Part1(tc.data)

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
	expected := `Result of part 1: 283
Result of part 2: 1
`
	buff := new(bytes.Buffer)

	err := day24.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
