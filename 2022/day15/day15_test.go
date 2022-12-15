package day15_test

import (
	"bytes"
	"log"
	"os"
	"testing"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/day15"
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

type Sensor = day15.Sensor
type Beacon = day15.Beacon

func TestPart1(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		sensors []Sensor
		beacons []Beacon
		want    int
	}{
		"empty": {sensors: []Sensor{}, beacons: []Beacon{}, want: 0},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day15.Part1(tc.sensors, tc.beacons)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		sensors []Sensor
		beacons []Beacon
		want    int
	}{
		"empty": {sensors: []Sensor{}, beacons: []Beacon{}, want: 0},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got, err := day15.Part2(tc.sensors, tc.beacons)

			require.NoError(t, err)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 0
Result of part 2: 0
`
	buff := new(bytes.Buffer)

	err := day15.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
