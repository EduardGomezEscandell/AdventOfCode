// Package day16 solves day 16 of AoC2022.
package day16

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"regexp"
	"strconv"
	"strings"
	"sync"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 16
	fileName = "input.txt"
)

type PrunningInfo struct {
	bestRemaining []Score
	bestScore     Score
	mu            sync.Mutex
}

func (pi *PrunningInfo) CanPrune(score Score, timeLeft int) bool {
	maxAttainable := score + pi.bestRemaining[timeLeft]
	return maxAttainable < pi.BestScore()
}

func (pi *PrunningInfo) UpdateBestScore(current Score) {
	pi.mu.Lock()
	defer pi.mu.Unlock()

	if current > pi.bestScore {
		pi.bestScore = current
	}
}

func (pi *PrunningInfo) BestScore() Score {
	pi.mu.Lock()
	defer pi.mu.Unlock()
	return pi.bestScore
}

// bestRemainingScore computes the score that woud be obtained if at a certain point of time, all
// valves became connected to one another. Therefore, bestRemainingScore[15], is the maximum score
// that can be collected in 15 moves assuming you can move freely and all valves are open. Let's
// call it BRS (Best Remaining Score).
//
// This is useful because if you're X moves from finishing, and your current score + the BRF[X] is
// less that the best score you have recorded, then you don't need to continue, as it is impossible
// to set a new record. This allows for prunning bad branches in the DFS.
func bestRemainingScore(world []Valve, n int) []Score {
	brs := make([]Score, n+1)
	for i := 0; i <= n; i++ {
		var s Score
		var id int
		for tLeft := i; tLeft > 0; tLeft -= 2 {
			s += Score(tLeft-1) * world[id].Flowrate
			id++
			if id >= len(world) {
				break
			}
		}
		brs[i] = s
	}
	return brs
}

// Part1 solves the first half of the problem.
func Part1(world []Valve, startPoint ID) (Score, error) {
	return Solve(world, startPoint, 30)
}

func Solve(world []Valve, startPoint ID, time int) (Score, error) {
	if len(world) == 0 {
		return 0, errors.New("world map is empty")
	}

	pi := PrunningInfo{
		bestRemaining: bestRemainingScore(world, time),
	}

	opened := Checklist(0)

	var wg sync.WaitGroup
	for _, id := range world[startPoint].Paths {
		id := id
		wg.Add(1)
		go func() {
			defer wg.Done()
			greedyDFS(world, id, opened, time-1, 0, &pi)
		}()
	}
	wg.Wait()
	return pi.BestScore(), nil
}

func greedyDFS(world []Valve, curr ID, open Checklist, timeLeft int, score Score, pi *PrunningInfo) {
	defer func() {
		pi.UpdateBestScore(score)
	}()

	if timeLeft < 2 {
		return
	}

	if pi.CanPrune(score, timeLeft) {
		return
	}

	valve := world[curr]

	// Function that explores via DFS
	explorePath := func(where ID) func() {
		return func() {
			greedyDFS(world, where, open, timeLeft-1, score, pi)
		}
	}
	// Funcion that opens the valve at the current location
	openSelf := func() {
		score := score + Score(timeLeft-1)*valve.Flowrate
		greedyDFS(world, curr, open.WithSet(curr), timeLeft-1, score, pi)
	}

	// An action can either be to move or to open a valve
	type action struct {
		score Score
		eval  func()
	}

	// All possible actions to take are put into an array
	actions := array.Map(valve.Paths, func(id ID) action {
		var s Score        // Heuristic: the score of an action is the flowrate of the destination
		if !open.Get(id) { // If the destination valve is already open, its score is 0.
			s = world[id].Flowrate
		}
		return action{score: s, eval: explorePath(id)}
	})
	if !open.Get(curr) && valve.Flowrate != 0 {
		actions = append(actions, action{score: valve.Flowrate, eval: openSelf})
	}

	// Greedy search: we perform the actions with best score first.
	array.Sort(actions, func(a, b action) bool {
		return a.score > b.score
	})

	for _, a := range actions {
		a.eval()
	}
}

// Part2 solves the second half of the problem.
func Part2([]Valve) (int, error) {
	return 1, nil
}

// ------------- Implementation ------------------.

type ID uint8

type Score int32

type Valve struct {
	Flowrate Score
	Paths    []ID
}

type Checklist uint64

func (cl *Checklist) Set(i ID) {
	*cl = cl.WithSet(i)
}

func (cl Checklist) WithSet(i ID) Checklist {
	return cl | Checklist(1)<<i
}

func (cl Checklist) Get(i ID) bool {
	return cl&(Checklist(1)<<i) != 0
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	world, startPoint, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := Part1(world, startPoint)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(world)
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

// ReadData reads the data file and returns a map of the valves.
func ReadData() ([]Valve, ID, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, 0, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))

	type protovalve struct {
		name     string
		flowrate int
		paths    []string
	}
	pv := []protovalve{}

	regex := regexp.MustCompile(`Valve (\w+) has flow rate=(\d+); tunnels? leads? to valves? (.*)`)
	for sc.Scan() {
		line := sc.Text()
		m := regex.FindStringSubmatch(line)
		if len(m) != 4 {
			return nil, 0, fmt.Errorf("Failed to parse. Regex does not match.\nRegex: %s\nLine : %s", regex, line)
		}

		fr, err := strconv.Atoi(m[2])
		if err != nil {
			return nil, 0, fmt.Errorf("Failed to parse flowrate from line %q: %v", line, err)

		}

		pv = append(pv, protovalve{
			name:     m[1],
			flowrate: fr,
			paths:    strings.Split(m[3], ", "),
		})
	}

	if err := sc.Err(); err != nil {
		return nil, 0, err
	}

	// Sorting valves by flowrate
	array.Sort(pv, func(p, q protovalve) bool { return p.flowrate > q.flowrate })
	var startPoint int
	startPoint = array.FindIf(pv, func(pv protovalve) bool { return pv.name == "AA" })
	if startPoint == -1 {
		return nil, 0, errors.New("Coud not find valve named AA")
	}

	// Assigning a numerical ID to each valve
	ids := map[string]ID{}
	for i, p := range pv {
		ids[p.name] = ID(i)
	}
	if len(ids) > 63 {
		return nil, 0, fmt.Errorf("systems with greater than %d valves are not supported. Got: %d", 63, len(ids))
	}

	// Generating world map
	world := make([]Valve, len(pv))
	for i := range pv {
		i := ID(i)
		var err error
		world[i] = Valve{
			Flowrate: Score(pv[i].flowrate),
			Paths: array.Map(pv[i].paths, func(name string) ID {
				id, ok := ids[name]
				if !ok && err == nil {
					err = fmt.Errorf("Entry %d references inexistent valve %q", i, name)
				}
				return id
			}),
		}
		if err != nil {
			return nil, 0, err
		}
		// Sorting paths by flowrate
		array.Sort(world[i].Paths, fun.Lt[ID])
	}

	return world, ID(startPoint), nil
}
