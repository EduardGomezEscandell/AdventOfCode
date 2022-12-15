package day15_test

import (
	"bytes"
	"log"
	"os"
	"strings"
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

type Range = day15.Range
type Long = day15.Long
type Point = day15.Point
type Sensor = day15.Sensor
type Beacon = day15.Beacon

func TestReadInput(t *testing.T) {
	testCases := map[string]struct {
		data        []string
		wantSensors []Sensor
		wantBeacons []Beacon
	}{
		"single": {
			data:        []string{"Sensor at x=3797530, y=3451192: closest beacon is at x=3316341, y=3328308"},
			wantSensors: []Sensor{{Point{3797530, 3451192}, 0}},
			wantBeacons: []Beacon{{0, Point{3316341, 3328308}}},
		},
		"duplicate beacon": {
			data:        []string{"Sensor at x=3797530, y=3451192: closest beacon is at x=3316341, y=3328308", "Sensor at x=0, y=3: closest beacon is at x=3316341, y=3328308"},
			wantSensors: []Sensor{{Point{3797530, 3451192}, 0}, {Point{0, 3}, 0}},
			wantBeacons: []Beacon{{0, Point{3316341, 3328308}}},
		},
		"example": {
			data: []string{"Sensor at x=2, y=18: closest beacon is at x=-2, y=15",
				"Sensor at x=9, y=16: closest beacon is at x=10, y=16",
				"Sensor at x=13, y=2: closest beacon is at x=15, y=3",
				"Sensor at x=12, y=14: closest beacon is at x=10, y=16",
				"Sensor at x=10, y=20: closest beacon is at x=10, y=16",
				"Sensor at x=14, y=17: closest beacon is at x=10, y=16",
				"Sensor at x=8, y=7: closest beacon is at x=2, y=10",
				"Sensor at x=2, y=0: closest beacon is at x=2, y=10",
				"Sensor at x=0, y=11: closest beacon is at x=2, y=10",
				"Sensor at x=20, y=14: closest beacon is at x=25, y=17",
				"Sensor at x=17, y=20: closest beacon is at x=21, y=22",
				"Sensor at x=16, y=7: closest beacon is at x=15, y=3",
				"Sensor at x=14, y=3: closest beacon is at x=15, y=3",
				"Sensor at x=20, y=1: closest beacon is at x=15, y=3",
			}, wantSensors: []Sensor{
				{Point{2, 18}, 0},
				{Point{9, 16}, 1},
				{Point{13, 2}, 2},
				{Point{12, 14}, 1},
				{Point{10, 20}, 1},
				{Point{14, 17}, 1},
				{Point{8, 7}, 3},
				{Point{2, 0}, 3},
				{Point{0, 11}, 3},
				{Point{20, 14}, 4},
				{Point{17, 20}, 5},
				{Point{16, 7}, 2},
				{Point{14, 3}, 2},
				{Point{20, 1}, 2},
			}, wantBeacons: []Beacon{{0, Point{-2, 15}},
				{1, Point{10, 16}},
				{2, Point{15, 3}},
				{3, Point{2, 10}},
				{4, Point{25, 17}},
				{5, Point{21, 22}},
			},
		},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			defer testutils.Backup(&day15.ReadDataFile)()
			day15.ReadDataFile = func() ([]byte, error) {
				return []byte(strings.Join(tc.data, "\n")), nil
			}

			gotSensors, gotBeacons, err := day15.ReadData()
			require.NoError(t, err)

			require.Equal(t, tc.wantSensors, gotSensors)
			require.Equal(t, tc.wantBeacons, gotBeacons)
		})
	}
}

func TestAddRange(t *testing.T) {
	t.Parallel()
	testCases := map[string]struct {
		ranges []Range
		new    Range
		want   []Range
	}{
		"left":          {ranges: []Range{{0, 5}}, new: Range{-5, -1}, want: []Range{{-5, -1}, {0, 5}}},
		"right":         {ranges: []Range{{0, 5}}, new: Range{8, 13}, want: []Range{{0, 5}, {8, 13}}},
		"same":          {ranges: []Range{{0, 5}}, new: Range{0, 5}, want: []Range{{0, 5}}},
		"inside":        {ranges: []Range{{0, 5}}, new: Range{1, 4}, want: []Range{{0, 5}}},
		"merge left":    {ranges: []Range{{0, 5}}, new: Range{-1, 3}, want: []Range{{-1, 5}}},
		"merge right":   {ranges: []Range{{0, 5}}, new: Range{4, 16}, want: []Range{{0, 16}}},
		"merge outside": {ranges: []Range{{0, 5}}, new: Range{-4, 19}, want: []Range{{-4, 19}}},
		"join inside":   {ranges: []Range{{0, 5}, {8, 13}}, new: Range{1, 11}, want: []Range{{0, 13}}},
		"join left":     {ranges: []Range{{0, 5}, {8, 13}}, new: Range{-6, 11}, want: []Range{{-6, 13}}},
		"join right":    {ranges: []Range{{0, 5}, {8, 13}}, new: Range{2, 19}, want: []Range{{0, 19}}},
		"join outside":  {ranges: []Range{{0, 5}, {8, 13}}, new: Range{-16, 82}, want: []Range{{-16, 82}}},
		"join exact":    {ranges: []Range{{0, 5}, {8, 13}}, new: Range{0, 13}, want: []Range{{0, 13}}},
	}
	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got := day15.AddRange(tc.ranges, tc.new)

			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart1(t *testing.T) {
	t.Parallel()
	/*
		                  111111111
		  6543210123456789012345678
		6 ·················#·······
		5 ······B·········XXX······ <- "double"
		4 ·····###·······#####·····
		3 ····XXXXX·B···#######···· <- "single"
		2 ···#######···#########···
		1 ··#########·###########··
		0 ·#####0####B#####1######·
	*/
	testCases := map[string]struct {
		target  Long
		sensors []Sensor
		beacons []Beacon
		want    Long
	}{
		"single": {target: 3, sensors: []Sensor{{Point{0, 0}, 0}}, beacons: []Beacon{{0, Point{0, 5}}, {1, Point{5, 0}}, {2, Point{4, 3}}}, want: 5},
		"double": {target: 5, sensors: []Sensor{{Point{0, 0}, 0}, {Point{11, 0}, 1}}, beacons: []Beacon{{0, Point{0, 5}}, {0, Point{5, 0}}, {2, Point{4, 3}}}, want: 3},
		"example": {target: 10, want: 26, sensors: []Sensor{
			{Point{2, 18}, 0},
			{Point{9, 16}, 1},
			{Point{13, 2}, 2},
			{Point{12, 14}, 1},
			{Point{10, 20}, 1},
			{Point{14, 17}, 1},
			{Point{8, 7}, 3},
			{Point{2, 0}, 3},
			{Point{0, 11}, 3},
			{Point{20, 14}, 4},
			{Point{17, 20}, 5},
			{Point{16, 7}, 2},
			{Point{14, 3}, 2},
			{Point{20, 1}, 2}}, beacons: []Beacon{
			{0, Point{-2, 15}},
			{1, Point{10, 16}},
			{2, Point{15, 3}},
			{3, Point{2, 10}},
			{4, Point{25, 17}},
			{5, Point{21, 22}},
		}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got := day15.Part1(tc.sensors, tc.beacons, tc.target)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestPart2(t *testing.T) {
	t.Parallel()

	testCases := map[string]struct {
		sensors []Sensor
		beacons []Beacon
		world   Range
		want    Long
	}{
		"example": {world: Range{0, 21}, want: 56000011, sensors: []Sensor{
			{Point{2, 18}, 0},
			{Point{9, 16}, 1},
			{Point{13, 2}, 2},
			{Point{12, 14}, 1},
			{Point{10, 20}, 1},
			{Point{14, 17}, 1},
			{Point{8, 7}, 3},
			{Point{2, 0}, 3},
			{Point{0, 11}, 3},
			{Point{20, 14}, 4},
			{Point{17, 20}, 5},
			{Point{16, 7}, 2},
			{Point{14, 3}, 2},
			{Point{20, 1}, 2},
		}, beacons: []Beacon{
			{0, Point{-2, 15}},
			{1, Point{10, 16}},
			{2, Point{15, 3}},
			{3, Point{2, 10}},
			{4, Point{25, 17}},
			{5, Point{21, 22}},
		}},
	}

	for name, tc := range testCases {
		tc := tc
		t.Run(name, func(t *testing.T) {
			t.Parallel()

			got := day15.Part2(tc.sensors, tc.beacons, tc.world)
			require.Equal(t, tc.want, got)
		})
	}
}

func TestRealData(t *testing.T) {
	expected := `Result of part 1: 5511201
Result of part 2: 11318723411840
`
	buff := new(bytes.Buffer)

	err := day15.Main(buff)

	require.NoError(t, err)
	require.Equal(t, expected, buff.String())
}
