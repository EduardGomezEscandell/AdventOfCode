// Package day15 solves day 15 of AoC2022.
package day15

import (
	"bufio"
	"bytes"
	"context"
	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 15
	fileName = "input.txt"
)

// Part1 solves the first half of the problem. The result is the
// sum of the size of unavailable ranges, minus the number of
// beacons in these ranges.
func Part1(sensors []Sensor, beacons []Beacon, target Long) Long {
	ranges, beaconsX := findExcludedRanges(sensors, beacons, target)
	return algo.MapReduce(ranges, (Range).length, utils.Add[Long], Long(-len(beaconsX)))
}

// Part2 solves the second half of the problem. It is mostly a wrapper
// for the parallelisation, so look at part2Worker to see the logic in
// how to solve the problem.
func Part2(sensors []Sensor, beacons []Beacon, world Range) (Long, error) {
	// Communication
	ctx, cancel := context.WithCancel(context.Background())
	defer cancel()

	chResult := make(chan Point)
	defer close(chResult)

	chSuccess := make(chan bool)
	defer close(chSuccess)

	// Partitioning the world
	nworkers := int(utils.Min(64, world.length()))
	wsize := world.length() / Long(nworkers)

	for w := 0; w < nworkers; w++ {
		w := w
		go func() {
			yRange := Range{
				Begin: world.Begin + Long(w)*wsize,
				End:   utils.Min(world.End, world.Begin+Long(w+1)*wsize),
			}
			position, found := part2Worker(ctx, sensors, beacons, world, yRange)
			chSuccess <- found
			if !found {
				return
			}
			chResult <- position
			cancel() // Cancelling all other workers: we've found a solution
		}()
	}

	// Wait for all workers to cancel, otherwise we leak them.
	found := charray.Reduce(charray.Take(chSuccess, nworkers), utils.Or, false)
	if !found {
		return 0, errors.New("failed to find distress beacon")
	}
	position := <-chResult
	return (P2RangeEnd-1)*position.X + position.Y, nil
}

// part2Worker solves part 2 of the problem in a reduced section
// of the world, with the option of being interrupted from outside.
func part2Worker(ctx context.Context, sensors []Sensor, beacons []Beacon, worldX Range, worldY Range) (Point, bool) {
	var p Point
	var i Long
	for p.Y = worldY.Begin; p.Y < worldY.End; p.Y++ {
		// scanning row of the world
		ranges, beaconsX := findExcludedRanges(sensors, beacons, p.Y)

		for _, x := range beaconsX {
			// beaconsX will be empty most of the time, so not much cost added here
			ranges = AddRange(ranges, Range{x, x + 1})
		}

		endLead, gaps, beginTail := extractGaps(ranges)

		// Free spaces to the left of the world
		if endLead > worldX.Begin {
			p.X = worldX.Begin
			return p, true
		}

		// Free spaces to the right of the world
		if beginTail < worldX.End {
			p.X = beginTail
			return p, true
		}

		// Free spaces in the middle of the world
		if len(gaps) != 0 {
			p.X = gaps[0].Begin
			return p, true
		}

		// No free spots, we poll the interruptor
		// and contiue with the next row.
		i++
		if i%100 == 0 { // (We only actually poll every 100 rows)
			select {
			default:
				continue // No interruption
			case <-ctx.Done():
				return p, false // Interrupted from outside, aborting
			}
		}
	}
	return p, false
}

// extractGaps takes a set of ranges and returns its complimentary set S.
// Note that this complimentary set is unbounded to the left and right,
// so the mathematical expression would be:
//
//	S = (-∞, endLead) U central[0] U central[1] U ... U  [beginTail, +∞)
func extractGaps(A []Range) (endLead Long, central []Range, beginTail Long) {
	if len(A) == 0 {
		return 0, []Range{}, 0
	}

	endLead = A[0].Begin
	central = algo.AdjacentMap(A, func(r1, r2 Range) Range { return Range{r1.End, r2.Begin} })
	beginTail = A[len(A)-1].End

	return
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
	start := utils.Max(firstGap, firstRange)
	lastGap, lastRange := locatePoint(ranges[start:], new.End)
	if lastGap != -1 {
		lastGap += start
	} else {
		lastRange += start
	}

	if firstGap != -1 && lastGap != -1 && firstGap == lastGap {
		// Entirely contained in a gap
		return algo.Insert(ranges, new, firstGap)
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
	return removeSubarray(ranges, firstRange, lastRange)
}

// findExcludedRanges finds all the ranges for a particular Y
// where a beacon cannot be bound.
func findExcludedRanges(sensors []Sensor, beacons []Beacon, targetY Long) ([]Range, []Long) {
	ranges := []Range{}
	for _, s := range sensors {
		radius := manhattan(s.Point, beacons[s.Detects].Point)
		dist := algo.Abs(s.Y - targetY)
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
		if b.Y != targetY {
			continue
		}
		_, inRange := locatePoint(ranges, b.X)
		if inRange == -1 {
			continue
		}
		beaconOverlaps[b.X] = struct{}{}
	}
	beaconsX := make([]Long, 0, len(beaconOverlaps))
	for x := range beaconOverlaps {
		beaconsX = append(beaconsX, x)
	}
	return ranges, beaconsX
}

// removeSubarray removes all items in arr[begin:end].
func removeSubarray(arr []Range, begin, end int) []Range {
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
	idx := algo.FindIf(ranges, func(rx Range) bool { return rx.End >= x })
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
	P1Target     Long = 2_000_000 // P1Target is the Y to measure in part 1.
	P2RangeBegin Long = 0         // P2RangeBegin is the inclusive lower bound for Y to measure in part 2.
	P2RangeEnd   Long = 4_000_001 // P2RangeEnd is the exclusive upper bound for Y to measure in part 2.
)

// manhattan is the manhattan distance between points p and q.
// Manhattan, taxicab and L1 distances are all the same thing.
func manhattan(p, q Point) Long {
	return algo.Abs(p.X-q.X) + algo.Abs(p.Y-q.Y)
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	sensors, beacons, err := ReadData()
	if err != nil {
		return err
	}

	p1 := Part1(sensors, beacons, P1Target)
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(sensors, beacons, Range{P2RangeBegin, P2RangeEnd})
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
// sensors and beacons.
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
