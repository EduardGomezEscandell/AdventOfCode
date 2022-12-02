package day02_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day02"
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

type Round = day02.Round

const (
	ROCK     = day02.Rock
	PAPER    = day02.Paper
	SCISSORS = day02.Scissors
)

const (
	Lose = day02.Lose
	Tie  = day02.Draw
	Win  = day02.Win
)

func TestReadData(t *testing.T) {
	testCases := map[string]struct {
		data string
		want []Round
	}{
		"empty":   {data: "", want: []Round{}},
		"one":     {data: "A X", want: []Round{{ROCK, ROCK}}},
		"example": {data: "A Y\nB X\nC Z", want: []Round{{ROCK, PAPER}, {PAPER, ROCK}, {SCISSORS, SCISSORS}}},
		"sample":  {data: "C X\nC X\nA Z\nC X", want: []Round{{SCISSORS, Lose}, {SCISSORS, Lose}, {ROCK, Win}, {SCISSORS, Lose}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day02.ReadDataFile)()
			day02.ReadDataFile = func() ([]byte, error) {
				return []byte(tc.data), nil
			}

			got, err := day02.ParseInput()
			t.Log(err)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data []Round
		want uint
	}{
		"empty":   {want: 0, data: []Round{}},
		"one":     {want: 4, data: []Round{{ROCK, ROCK}}},
		"example": {want: 15, data: []Round{{ROCK, PAPER}, {PAPER, ROCK}, {SCISSORS, SCISSORS}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day02.Part1(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		data []Round
		want uint
	}{
		"empty":   {want: 0, data: []Round{}},
		"one":     {want: 8, data: []Round{{ROCK, Win}}},
		"first":   {want: 4, data: []Round{{ROCK, Tie}}},
		"second":  {want: 1, data: []Round{{PAPER, Lose}}},
		"third":   {want: 7, data: []Round{{SCISSORS, Win}}},
		"example": {want: 12, data: []Round{{ROCK, Tie}, {PAPER, Lose}, {SCISSORS, Win}}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()
			got, err := day02.Part2(tc.data)
			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 14375
Result of part 2: 10274
`
	buff := new(bytes.Buffer)

	err := day02.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
