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

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 16
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(world []Valve, startPoint VID) (Score, error) {
	return Solve(world, startPoint, 30, false)
}

// Part2 solves the second half of the problem.
func Part2(world []Valve, startPoint VID) (Score, error) {
	return Solve(world, startPoint, 26, true)
}

// ------------- Implementation ------------------.

func Solve(world []Valve, startPoint VID, time int, elephantsAllowed bool) (Score, error) {
	if len(world) == 0 {
		return 0, errors.New("world map is empty")
	}

	pi := NewPrunningInfo(world, time, elephantsAllowed)

	ws := worldState{
		location: [2]VID{startPoint, startPoint},
		opened:   Checklist(0),
		timeLeft: time,
	}
	greedyDFS(world, ws, 0, elephantsAllowed, pi)
	return pi.bestScore, nil
}

// An action can either be to move or to open a valve
type action struct {
	priority Score
	newState worldState
	eval     func() Score
}

func greedyDFS(world []Valve, ws worldState, score Score, elephantsAllowed bool, pi *PrunningInfo) (addedScore Score) {
	if ws.timeLeft < 2 {
		return 0
	}
	if pi.CanPrune(score, ws.timeLeft) {
		return 0
	}

	defer func() { pi.bestScore = fun.Max(pi.bestScore, score+addedScore) }()

	// Checking if we have the solution in cache
	if s, ok := pi.lru.Check(ws); ok {
		return s
	}
	defer func() { pi.lru.Update(ws, addedScore) }() // Will update the cache on function exit

	// Checking if we have the simetrical solution in cache
	if elephantsAllowed {
		ws := ws
		ws.location[player], ws.location[elephant] = ws.location[elephant], ws.location[player]
		if s, ok := pi.lru.Check(ws); ok {
			return s
		}
		defer func() { pi.lru.Update(ws, addedScore) }() // Will update the cache on function exit
	}

	/*
	 * Compiling all possible continuations
	 */
	playerAt := world[ws.location[player]]
	elephantAt := world[ws.location[elephant]]

	// Movements
	playerMoves := array.Map(playerAt.Paths, fun.Identity[VID])
	elephantMoves := array.Map(elephantAt.Paths, fun.Identity[VID])

	// Adding valve openings
	playerMoves = append(playerMoves, ws.location[player])
	if !elephantsAllowed {
		elephantMoves = []VID{0}
	} else {
		elephantMoves = append(elephantMoves, ws.location[elephant])
	}

	actions := make([]action, 0, len(elephantMoves)*len(playerMoves))
	for _, pm := range playerMoves {
		for _, em := range elephantMoves {
			a := NewAction(world, ws, score, elephantsAllowed, pi, pm, em)
			if a == nil {
				continue
			}
			actions = append(actions, *a)
		}
	}
	// The player and the elephant are equivalent, we can prune
	if elephantsAllowed && ws.location[player] == ws.location[elephant] {
		actions = PurgeDuplicateActions(actions)
	}
	/*
	 * Greedy search: we perform the actions with best score first.
	 */
	array.Sort(actions, func(a, b action) bool { return a.priority > b.priority })
	return array.MapReduce(actions, func(a action) Score { return a.eval() }, fun.Max[Score], 0)
}

func PurgeDuplicateActions(actions []action) []action {
	array.Sort(actions, func(a, b action) bool {
		ka0 := uint16(a.newState.location[0])
		ka1 := uint16(a.newState.location[1])
		ka := fun.Min(ka0, ka1)<<8 + fun.Max(ka0, ka1)

		kb0 := uint16(b.newState.location[0])
		kb1 := uint16(b.newState.location[1])
		kb := fun.Min(kb0, kb1)<<8 + fun.Max(kb0, kb1)
		return ka < kb
	})
	return actions[:array.Unique(actions, func(a, b action) bool {
		if a.newState.location == b.newState.location {
			return true
		}
		a.newState.location[0], a.newState.location[1] = a.newState.location[1], a.newState.location[0]
		return a.newState.location == b.newState.location
	})]
}

type VID uint8 // Valve ID
type PID uint8 // Player ID

type Score int32

type Valve struct {
	Flowrate Score
	Paths    []VID
}

type Checklist uint64

const (
	player   PID = 0
	elephant PID = 1
)

func (cl *Checklist) Set(i VID) {
	*cl = cl.WithSet(i)
}

func (cl Checklist) WithSet(i VID) Checklist {
	return cl | Checklist(1)<<i
}

func (cl Checklist) Get(i VID) bool {
	return cl&(Checklist(1)<<i) != 0
}

type PrunningInfo struct {
	bestRemaining []Score
	bestScore     Score
	lru           *lruCache[worldState, Score]
}

type worldState struct {
	location [2]VID
	opened   Checklist
	timeLeft int
}

func NewPrunningInfo(world []Valve, time int, elephantsAllowed bool) *PrunningInfo {
	return &PrunningInfo{
		bestRemaining: bestRemainingScore(world, time, elephantsAllowed),
		lru:           NewLRUCache[worldState, Score](10_000_000),
	}
}

func (pi *PrunningInfo) CanPrune(score Score, timeLeft int) bool {
	maxAttainable := score + pi.bestRemaining[timeLeft]
	return maxAttainable < pi.bestScore
}

// bestRemainingScore computes the score that woud be obtained if at a certain point of time, all
// valves became connected to one another. Therefore, bestRemainingScore[15], is the maximum score
// that can be collected in 15 moves assuming you can move freely and all valves are open. Let's
// call it BRS (Best Remaining Score).
//
// This is useful because if you're X moves from finishing, and your current score + the BRF[X] is
// less that the best score you have recorded, then you don't need to continue, as it is impossible
// to set a new record. This allows for prunning bad branches in the DFS.
func bestRemainingScore(world []Valve, n int, elephantsAllowed bool) []Score {
	if !elephantsAllowed {
		return bestRemainingScoreSinglePlayer(world, n)
	}
	return bestRemainingScoreCoop(world, n)
}

func bestRemainingScoreSinglePlayer(world []Valve, n int) []Score {
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

func bestRemainingScoreCoop(world []Valve, n int) []Score {
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

func NewAction(world []Valve, ws worldState, score Score, elephantsAllowed bool, pi *PrunningInfo, playerT, elephantT VID) *action {
	var priority Score
	var newScore Score

	timeLeft := ws.timeLeft - 1
	opened := ws.opened

	if ws.location[player] == playerT {
		// Player opens a valve
		if opened.Get(playerT) || world[playerT].Flowrate == 0 {
			return nil // already opened
		}
		opened.Set(playerT)
		newScore += world[playerT].Flowrate * Score(timeLeft)
		priority += newScore
	} else if !ws.opened.Get(playerT) {
		priority += world[playerT].Flowrate * Score(timeLeft-1)
	}

	if elephantsAllowed {
		if ws.location[elephant] == elephantT {
			// Elephant opens a valve
			if opened.Get(elephantT) || world[elephantT].Flowrate == 0 {
				return nil // already opened
			}
			opened.Set(elephantT)
			newScore += world[elephantT].Flowrate * Score(timeLeft)
			priority += newScore
		} else if !opened.Get(elephantT) && elephantT != playerT {
			priority += world[elephantT].Flowrate * Score(timeLeft-1)
		}
	}

	new := ws
	new.location[player] = playerT
	new.location[elephant] = elephantT
	new.timeLeft = timeLeft
	new.opened = opened

	return &action{
		priority: priority,
		newState: new,
		eval:     func() Score { return newScore + greedyDFS(world, new, score+newScore, elephantsAllowed, pi) },
	}
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

	p2, err := Part2(world, startPoint)
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
func ReadData() ([]Valve, VID, error) {
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
	ids := map[string]VID{}
	for i, p := range pv {
		ids[p.name] = VID(i)
	}
	if len(ids) > 63 {
		return nil, 0, fmt.Errorf("systems with greater than %d valves are not supported. Got: %d", 63, len(ids))
	}

	// Generating world map
	world := make([]Valve, len(pv))
	for i := range pv {
		i := VID(i)
		var err error
		world[i] = Valve{
			Flowrate: Score(pv[i].flowrate),
			Paths: array.Map(pv[i].paths, func(name string) VID {
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
		array.Sort(world[i].Paths, fun.Lt[VID])
	}

	return world, VID(startPoint), nil
}
