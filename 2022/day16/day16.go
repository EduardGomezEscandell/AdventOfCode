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

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/utils"
)

const (
	today    = 16
	fileName = "input.txt"
)

// part1 solves the first half of the problem.
func part1(world []Valve, startPoint VID) (Score, error) {
	return Solve(world, startPoint, 30, false)
}

// part2 solves the second half of the problem.
func part2(world []Valve, startPoint VID) (Score, error) {
	return Solve(world, startPoint, 26, true)
}

// ------------- Implementation ------------------.

// Solve solves both first and second half of the problem.
func Solve(world []Valve, startPoint VID, time int, elephantsAllowed bool) (Score, error) {
	if len(world) == 0 {
		return 0, errors.New("world map is empty")
	}

	pi := newPrunningTool(world, time, elephantsAllowed)

	ws := worldState{
		location: [2]VID{startPoint, startPoint},
		opened:   checklist(0),
		timeLeft: time,
	}
	greedyDFS(world, ws, 0, elephantsAllowed, pi)
	return pi.bestScore, nil
}

func greedyDFS(world []Valve, ws worldState, score Score, elephantsAllowed bool, pi *prunningTool) (addedScore Score) {
	if ws.timeLeft < 2 {
		return 0
	}
	if pi.canPrune(score, ws.timeLeft) {
		return 0
	}

	defer func() { pi.bestScore = utils.Max(pi.bestScore, score+addedScore) }()

	// Checking if we have the solution in cache
	if s, ok := pi.cache.Get(ws); ok {
		return s
	}
	defer func() { pi.cache.Set(ws, addedScore) }() // Will update the cache on function exit

	// Checking if we have the simetrical solution in cache
	if elephantsAllowed {
		ws := ws
		ws.location[player], ws.location[elephant] = ws.location[elephant], ws.location[player]
		if s, ok := pi.cache.Get(ws); ok {
			return s
		}
		defer func() { pi.cache.Set(ws, addedScore) }() // Will update the cache on function exit
	}

	/*
	 * Compiling all possible continuations
	 */
	playerAt := world[ws.location[player]]
	elephantAt := world[ws.location[elephant]]

	// Movements
	playerMoves := algo.Map(playerAt.Paths, utils.Identity[VID])
	elephantMoves := algo.Map(elephantAt.Paths, utils.Identity[VID])

	// Adding valve openings
	playerMoves = append(playerMoves, ws.location[player])
	if !elephantsAllowed {
		elephantMoves = []VID{0}
	} else {
		elephantMoves = append(elephantMoves, ws.location[elephant])
	}

	// Converting movements to actions
	actions := make([]action, 0, len(elephantMoves)*len(playerMoves))
	for _, pm := range playerMoves {
		for _, em := range elephantMoves {
			a := newAction(world, ws, score, elephantsAllowed, pi, pm, em)
			if a == nil {
				continue
			}
			actions = append(actions, *a)
		}
	}

	// The player and the elephant are equivalent, we can prune simmetrical actions
	if elephantsAllowed && ws.location[player] == ws.location[elephant] {
		actions = purgeDuplicateActions(actions)
	}

	// DFS greedy search: we perform the actions with highest priority first.
	algo.Sort(actions, func(a, b action) bool { return a.priority > b.priority })
	return algo.MapReduce(actions, func(a action) Score { return a.exec() }, utils.Max[Score], 0)
}

func purgeDuplicateActions(actions []action) []action {
	algo.Sort(actions, func(a, b action) bool {
		ka0 := uint16(a.newState.location[0])
		ka1 := uint16(a.newState.location[1])
		ka := utils.Min(ka0, ka1)<<8 + utils.Max(ka0, ka1)

		kb0 := uint16(b.newState.location[0])
		kb1 := uint16(b.newState.location[1])
		kb := utils.Min(kb0, kb1)<<8 + utils.Max(kb0, kb1)
		return ka < kb
	})
	return actions[:algo.Unique(actions, func(a, b action) bool {
		if a.newState.location == b.newState.location {
			return true
		}
		a.newState.location[0], a.newState.location[1] = a.newState.location[1], a.newState.location[0]
		return a.newState.location == b.newState.location
	})]
}

// VID is a typedef for a Valve ID.
type VID uint8

// PID is a typedef for a Player ID.
type PID uint8

// Score is a typedef to represent what the problem statement calls
// "pressure per minute". This makes no sense, so I just wen with
// score.
type Score int32

// Valve as in the problem statement.
type Valve struct {
	Flowrate Score
	Paths    []VID
}

type checklist uint64 // A flags object. Each bit means whether a pipe is open or not.

const (
	player   PID = 0
	elephant PID = 1
)

func (cl *checklist) set(i VID) {
	*cl = cl.withSet(i)
}

func (cl checklist) withSet(i VID) checklist {
	return cl | checklist(1)<<i
}

func (cl checklist) get(i VID) bool {
	return cl&(checklist(1)<<i) != 0
}

// An action can either be to move or to open a valve.
type action struct {
	priority Score
	newState worldState
	exec     func() Score
}

func newAction(world []Valve, ws worldState, score Score, elephantsAllowed bool, pi *prunningTool, playerT, elephantT VID) *action {
	var priority Score
	var newScore Score

	timeLeft := ws.timeLeft - 1
	opened := ws.opened

	if ws.location[player] == playerT {
		// Player opens a valve
		if opened.get(playerT) || world[playerT].Flowrate == 0 {
			return nil // already opened
		}
		opened.set(playerT)
		newScore += world[playerT].Flowrate * Score(timeLeft)
		priority += newScore
	} else if !ws.opened.get(playerT) {
		priority += world[playerT].Flowrate * Score(timeLeft-1)
	}

	if elephantsAllowed {
		if ws.location[elephant] == elephantT {
			// Elephant opens a valve
			if opened.get(elephantT) || world[elephantT].Flowrate == 0 {
				return nil // already opened
			}
			opened.set(elephantT)
			newScore += world[elephantT].Flowrate * Score(timeLeft)
			priority += newScore
		} else if !opened.get(elephantT) && elephantT != playerT {
			priority += world[elephantT].Flowrate * Score(timeLeft-1)
		}
	}

	newState := ws
	newState.location[player] = playerT
	newState.location[elephant] = elephantT
	newState.timeLeft = timeLeft
	newState.opened = opened

	return &action{
		priority: priority,
		newState: newState,
		exec:     func() Score { return newScore + greedyDFS(world, newState, score+newScore, elephantsAllowed, pi) },
	}
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	world, startPoint, err := ReadData()
	if err != nil {
		return err
	}

	p1, err := part1(world, startPoint)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := part2(world, startPoint)
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
	algo.Sort(pv, func(p, q protovalve) bool { return p.flowrate > q.flowrate })
	startPoint := algo.FindIf(pv, func(pv protovalve) bool { return pv.name == "AA" })
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
			Paths: algo.Map(pv[i].paths, func(name string) VID {
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
		algo.Sort(world[i].Paths, utils.Lt[VID])
	}

	return world, VID(startPoint), nil
}
