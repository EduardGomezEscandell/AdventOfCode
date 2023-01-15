// Package day24 solves day 24 of AoC2022.
package day24

import (
	"bufio"
	"bytes"
	"container/heap"
	"errors"
	"fmt"
	"io"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/myheap"
)

const (
	today    = 24
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(world [][]rune) (int, error) {
	wi, err := newWorldInfo(world)
	if err != nil {
		return 0, err
	}
	return aStarSearch(wi, 0)
}

// Part2 solves the second half of today's problem.
func Part2(world [][]rune) (int, error) {
	wi, err := newWorldInfo(world)
	if err != nil {
		return 0, err
	}

	var time int
	for i := 0; i < 3; i++ {
		time, err = aStarSearch(wi, time)
		if err != nil {
			return 0, fmt.Errorf("error in leg #%d of the trip: %v", i, err)
		}
		wi.entrance, wi.exit = wi.exit, wi.entrance
		wi.bestEpoch = map[state]int{} // Resetting prunning data
	}

	return time, nil
}

// ------------ Implementation ---------------------

// cell is every tile in the today's problem's world.
type cell = int8

const (
	obstacle cell = 1 // wall or storm
	free     cell = 0 // free to walk to
)

// snapshot represents the world's walls and storm locations
// at a particular point in time.
type snapshot = [][]cell

// location packs the row, col values to refer to a position
// in a 2D array.
type location struct {
	row, col int
}

// state contains the location and point in time. This is sufficient
// information to fully define a node int the search graph.
type state struct {
	location
	time int
}

// worldInfo contains the information sufficient to:
// - navigate the world (using snapshots, width, height and period)
// - search paths (from entrance to exit)
// - prune path transpositions (bestTimes).
type worldInfo struct {
	// Geometry info
	snapshots     []snapshot // storm configuration for every turn in the first (and hence every) period
	width, height int        // syntax sugar, so as not to check the size of the first snapshot every time

	// Due to the rectangular shape of the world, every certain number of
	// turns the storm configuration will repeat. This number of turns we
	// call the 'period'. After n periods have passed, we say you are in
	// 'epoch' n.
	period int

	// Reaching the same state but a later epoch is identical but worse.
	// Storing the lowest epoch that we reached a state in allows us to
	// prune large sections of the search tree.
	bestEpoch map[state]int

	// Routing info
	entrance, exit location // where we start and end the route
}

// newWorldInfo generates the world info ready to be used in part one,
// i.e. with the entrance and exit at the top and bottom, respectively.
func newWorldInfo(world [][]rune) (*worldInfo, error) {
	wi := &worldInfo{}

	wi.height = len(world)
	if wi.height < 1 {
		return wi, errors.New("world is empty")
	}

	wi.width = len(world[0])
	if wi.width < 1 {
		return wi, errors.New("world is empty")
	}

	wi.entrance = location{
		row: 0,
		col: array.Find(world[0], '.', fun.Eq[rune]),
	}

	wi.exit = location{
		row: len(world) - 1,
		col: array.Find(world[len(world)-1], '.', fun.Eq[rune]),
	}

	wi.bestEpoch = map[state]int{}

	wi.period = fun.LCM(wi.width-2, wi.height-2) // -2: The walls are outside the cycle
	wi.snapshots = make([]snapshot, wi.period)

	for idx := range wi.snapshots {
		var err error
		wi.snapshots[idx], err = getSnapshot(world, idx)
		if err != nil {
			return wi, err
		}
	}

	return wi, nil
}

// getSnapshot computes the storm configuration after a certain number of turns.
// Note that if the world period is n, then {turn, turn+n, turn+2n, ...} will all
// have the same snapshot.
func getSnapshot(world [][]rune, turn int) (snapshot, error) {
	snapshot := array.Generate2D(len(world), len(world[0]), func() cell { return free })

	height := uint(len(world))
	width := uint(len(world[0]))

	for row := range world {
		for col := range world[row] {
			r, c := row, col
			switch world[row][col] {
			case '.':
				continue
			case '#':
				snapshot[r][c] = obstacle
				continue
			case '<':
				c = col - turn
			case '>':
				c = col + turn
			case '^':
				r = row - turn
			case 'v':
				r = row + turn
			default:
				return nil, fmt.Errorf("unexpected character '%c' found in row %d column %d", world[row][col], row, col)
			}

			// -1 and +1 because storms cannot be on the walls
			r = positiveMod(r-1, height-2) + 1
			c = positiveMod(c-1, width-2) + 1

			snapshot[r][c] = obstacle
		}
	}

	return snapshot, nil
}

// aStarSearch uses A* to search for the best path from wi.entrance to wi.exit
// startTime is the time when the route starts (necessary to know the storm
// configuration). Returns the exitTime, equal to startTime + traversalTime.
func aStarSearch(wi *worldInfo, startTime int) (int, error) {
	start := state{
		location: wi.entrance,
		time:     startTime,
	}

	queue := myheap.New(func(a, b state) bool { return aStarHeuristic(wi, a, b) })
	heap.Push(queue, start)

	for queue.Len() > 0 {
		curr := heap.Pop(queue).(state) //nolint:forcetypeassert // We only push state objects to the heap
		finished, cont := exploreNode(wi, curr)
		if finished {
			return curr.time, nil
		}
		for _, c := range cont {
			heap.Push(queue, c)
		}
	}

	return 0, errors.New("could not find a path to the exit")
}

// aStarHeuristic is the rule used to identify the best candidate continuation.
// Note that for the success condition it degenerates to Dijkstra, meaning that
// the score is just the time to reach that point. This means that we can exit
// once we find the exit for the first time.
func aStarHeuristic(wi *worldInfo, a, b state) bool {
	bestPossibleScore := func(s state) int {
		return s.time + manhattan(s.location, wi.exit)
	}
	return bestPossibleScore(a) < bestPossibleScore(b)
}

// exploreNode finds all possible, relevant (i.e. not prunned) continuations
// to a state; both moving and staying in place.
func exploreNode(wi *worldInfo, s state) (finished bool, continuations []state) {
	// Exit found!
	if s.location == wi.exit {
		return true, nil
	}

	// No prunning, filling candidate moves
	continuations = []state{}
	var c state // candidate continuation

	// Step right
	c = state{
		time: s.time + 1,
		location: location{
			row: s.row,
			col: s.col + 1,
		},
	}
	if stateIsValidAndNovel(wi, c) {
		continuations = append(continuations, c)
	}

	// Step left
	c = state{
		time: s.time + 1,
		location: location{
			row: s.row,
			col: s.col - 1,
		},
	}
	if stateIsValidAndNovel(wi, c) {
		continuations = append(continuations, c)
	}

	// Step up
	c = state{
		time: s.time + 1,
		location: location{
			row: s.row - 1,
			col: s.col,
		},
	}
	if stateIsValidAndNovel(wi, c) {
		continuations = append(continuations, c)
	}

	// Step down
	c = state{
		time: s.time + 1,
		location: location{
			row: s.row + 1,
			col: s.col,
		},
	}
	if stateIsValidAndNovel(wi, c) {
		continuations = append(continuations, c)
	}

	// Stay put until one of these happens:
	// - You wait for a whole cycle
	// - The current cell is no longer available
	c = s
	for i := 1; i < wi.period; i++ {
		c.time = s.time + i
		if !stateIsValidAndNovel(wi, c) {
			break
		}
		continuations = append(continuations, c)
	}

	return false, continuations
}

// stateIsValidAndNovel ensures that a given state s is valide (i.e. the player
// is inside the world and not overlapping with an obstacle) and novel, i.e. we
// have not been in this state at a previous or equal epoch.
func stateIsValidAndNovel(wi *worldInfo, s state) bool {
	if s.row < 0 || s.row >= wi.height {
		return false
	}
	if s.col < 0 || s.col >= wi.width {
		return false
	}
	epoch := s.time / wi.period
	turn := s.time % wi.period
	if wi.snapshots[turn][s.row][s.col] != free {
		return false
	}

	// Prunning:
	// Consider if we've been here in a previous cycle
	epochlessState := s
	epochlessState.time = turn
	prev, ok := wi.bestEpoch[epochlessState]
	if ok && epoch >= prev {
		// Prunning: we've been in this position with the same storm
		// configuration at an earlier or equal time.
		return false
	}
	wi.bestEpoch[epochlessState] = epoch
	return true
}

// manhattan distance, taxicab distance, L1 norm, whatever you want to call it.
func manhattan(from, to location) int {
	return fun.Abs(from.row-to.row) + fun.Abs(from.col-to.col)
}

func positiveMod(x int, n uint) int {
	m := x % int(n)
	if m < 0 {
		m += int(n)
	}
	return m
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem's solution.
func Main(stdout io.Writer) error {
	world, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(world)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(world)
	if err != nil {
		return fmt.Errorf("error in part 2: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 2: %d\n", p2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and returns the raw world data.
func ReadData() (data [][]rune, err error) { // nolint: revive
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	world := [][]rune{}
	for row := 0; sc.Scan(); row++ {
		world = append(world, []rune(sc.Text()))
	}

	return world, sc.Err()
}
