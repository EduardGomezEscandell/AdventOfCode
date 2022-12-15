// Package day15 solves day 15 of AoC2022.
package day15

import (
	"bufio"
	"bytes"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 15
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(sensors []Sensor, beacons []Beacon, target Long) (Long, error) {
	ranges := []Range{}

	for _, s := range sensors {
		radius := manhattan(s.Point, beacons[s.Detects].Point)
		dist := fun.Abs(s.Y - target)
		if dist > radius {
			continue // Sensor is too far away
		}
		slack := radius - dist
		r := Range{
			Begin: s.X - slack,
			End:   s.X + slack + 1,
		}
		ranges = AddRange(ranges, r)
	}

	beaconOverlaps := map[Long]struct{}{}
	for _, b := range beacons {
		if b.Y != target {
			continue
		}
		_, inRange := locatePoint(ranges, b.X)
		if inRange == -1 {
			continue
		}
		beaconOverlaps[b.X] = struct{}{}
	}

	return array.MapReduce(ranges, (Range).length, fun.Add[Long], Long(-len(beaconOverlaps))), nil
}

// Part2 solves the second half of the problem.
func Part2([]Sensor, []Beacon) (int, error) {
	return 0, nil
}

// ------------- Implementation ------------------.

// Long refers to coordinates or distances between them.
type Long = int64

// Id is the identification of a beacon.
type Id uint // nolint: revive

// Point is a location in R2.
type Point struct {
	X, Y Long
}

// Beacon as in the problem statement.
type Beacon struct {
	ID Id
	Point
}

// Sensor as in the problem statement.
type Sensor struct {
	Point
	Detects Id
}

// Range is a set of contiguous integers bounded by [Begin, End).
type Range struct {
	Begin, End Long
}

func (r Range) length() Long {
	return r.End - r.Begin
}

// AddRange takes a sorted list of non-overlapping ranges and adds
// the new range, merging and streching the original ranges if
// necessary, so as to return a list of non-overlapping ranges.
func AddRange(ranges []Range, new Range) []Range {
	if new.Begin == new.End {
		return ranges
	}

	firstGap, firstRange := locatePoint(ranges, new.Begin)
	start := fun.Max(firstGap, firstRange)
	lastGap, lastRange := locatePoint(ranges[start:], new.End)
	if lastGap != -1 {
		lastGap += start
	} else {
		lastRange += start
	}

	if firstGap != -1 && lastGap != -1 && firstGap == lastGap {
		// Entirely contained in a gap
		return array.Insert(ranges, new, firstGap)
	}

	if firstGap != -1 {
		// Pulling forward beginning of the first range
		firstRange = firstGap
		ranges[firstRange].Begin = new.Begin
	}

	if lastGap != -1 {
		// Pushing back the end of the last range
		lastRange = lastGap - 1
		ranges[lastRange].End = new.End
	}

	// Stretching last range forward
	ranges[lastRange].Begin = ranges[firstRange].Begin

	// removing elements in [first, last)
	return removeRanges(ranges, firstRange, lastRange)
}

// removeRanges removes all ranges in arr[begin:end].
func removeRanges(arr []Range, begin, end int) []Range {
	if begin < 0 || begin > len(arr) {
		panic(fmt.Errorf("begin %d out of range [0, %d)", begin, len(arr)+1))
	}
	if end < 0 || end > len(arr) {
		panic(fmt.Errorf("end %d out of range [0, %d)", end, len(arr)+1))
	}
	if begin == end {
		return arr
	}
	// Copying tail
	for i := begin; end+i < len(arr); i++ {
		arr[i] = arr[end+i]
	}
	// Removing slack
	newLen := len(arr) - (end - begin)
	return arr[:newLen]
}

func locatePoint(ranges []Range, x Long) (firstGap, firstRange int) {
	idx := array.FindIf(ranges, func(rx Range) bool { return rx.End >= x })
	if idx == -1 { // Point is beyond the end
		return len(ranges), -1
	}

	// Point is within idx's range
	if ranges[idx].Begin <= x {
		return -1, idx
	}
	// Point is in the gap preceding idx's range
	return idx, -1
}

const (
	P1Target = 2_000_000 // P1Target is the Y to mesure in part 1
)

// manhattan is the manhattan distance between points p and q.
// Manhattan, taxicab and L1 distances are all the same thing.
func manhattan(p, q Point) Long {
	return fun.Abs(p.X-q.X) + fun.Abs(p.Y-q.Y)
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	sensors, beacons, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(sensors, beacons, P1Target)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(sensors, beacons)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and returns the list of
// reindeers and their calories.
func ReadData() ([]Sensor, []Beacon, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	sensors := []Sensor{}
	beaconsMap := map[Point]Id{}
	for sc.Scan() {
		s := Sensor{}
		b := Point{}
		n, err := fmt.Sscanf(sc.Text(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &s.X, &s.Y, &b.X, &b.Y)
		if err != nil {
			return nil, nil, err
		}
		if n != 4 {
			return nil, nil, fmt.Errorf("failed to read all four values from %q", sc.Text())
		}
		id, ok := beaconsMap[b]
		if !ok {
			id = Id(len(beaconsMap))
			beaconsMap[b] = id
		}
		s.Detects = id
		sensors = append(sensors, s)
	}

	if err := sc.Err(); err != nil {
		return nil, nil, err
	}

	// Turning beacons map into array
	beacons := make([]Beacon, len(beaconsMap))
	for point, id := range beaconsMap {
		beacons[id] = Beacon{id, point}
	}

	return sensors, beacons, nil
}
