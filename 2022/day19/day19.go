// Package day19 solves day 19 of AoC2022.
package day19

import (
	"bufio"
	"bytes"
	"errors"
	"fmt"
	"io"
	"math"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/lrucache"
)

const (
	today    = 19
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(blueprints []Blueprint) uint {
	ch := make(chan uint)
	defer close(ch)

	for _, bp := range blueprints {
		bp := bp
		go func() {
			ch <- SolveBlueprint(bp, 24) * bp.ID
		}()
	}
	r := charray.Take(ch, len(blueprints))
	return charray.Reduce(r, fun.Add[uint], 0)
}

// Part2 solves the second half of today's problem.
func Part2(blueprints []Blueprint) uint {
	blueprints = blueprints[:3]

	ch := make(chan uint)
	defer close(ch)

	for _, bp := range blueprints {
		bp := bp
		go func() {
			ch <- SolveBlueprint(bp, 32)
		}()
	}
	r := charray.Take(ch, len(blueprints))
	return charray.Reduce(r, fun.Mul[uint], 1)
}

// ------------ Implementation ---------------------

type Material int
type Machine Material

const (
	Ore Material = iota
	Clay
	Obsidian
	Geode
)

var (
	ores     = [4]Material{Ore, Clay, Obsidian, Geode}
	machines = [4]Machine{Machine(Ore), Machine(Clay), Machine(Obsidian), Machine(Geode)} // Syntax sugar
)

type Blueprint struct {
	ID    uint
	Costs [4][3]uint
}

type inventory struct {
	currency  [4]uint
	harvester [4]uint
}

type state struct {
	inv  inventory
	time uint
}

func SolveBlueprint(bp Blueprint, time uint) uint {
	s := state{
		inv:  inventory{},
		time: time,
	}
	s.inv.harvester[Ore] = 1

	var best uint
	cache := lrucache.New[state, struct{}](100_000)
	dfs(bp, s, &best, cache)
	return best
}

func dfs(bp Blueprint, s state, best *uint, cache *lrucache.LruCache[state, struct{}]) {
	if s.time == 0 {
		*best = fun.Max(*best, s.inv.currency[Geode])
		return
	}

	// Prunning: max possible yield if geode machines were free
	maxPossibleYield := s.inv.currency[Geode] + s.time*s.inv.harvester[Geode] + triangular(s.time)
	if maxPossibleYield <= *best {
		return
	}

	// Prunning: already been here
	if s.time > 5 {
		if _, found := cache.Get(s); found {
			return
		}
	}

	// Exploring continuations
	cont := []state{}
	for _, mach := range array.Reverse(machines[:]) {
		w := waitNeeded(bp, s.inv, mach) + 1 // It takes one minute to build the machine
		if s.time < w {
			continue
		}
		cont = append(cont, s)
		cont[len(cont)-1].wait(w).purchase(bp, mach)
	}

	// If you can do nothing, then wait for the clock to run out
	if len(cont) == 0 {
		s.wait(s.time)
		dfs(bp, s, best, cache)
		return
	}

	// We are greedy with resources to establish a high "best" ASAP. We want to rush the
	// end of the problem to establish a "best" and start prunning ASAP.
	array.Sort(cont, func(a, b state) bool {
		for material := Geode; material >= 0; material-- {
			if a.inv.harvester[material] != b.inv.harvester[material] {
				return a.inv.harvester[material] > b.inv.harvester[material]
			}
			if a.inv.currency[material] != b.inv.currency[material] {
				return a.inv.currency[material] > b.inv.currency[material]
			}
		}
		return a.time <= b.time
	})

	array.Foreach(cont, func(s *state) {
		dfs(bp, *s, best, cache)
	})

	if s.time > 5 {
		cache.Set(s, struct{}{})
	}
}

func waitNeeded(bp Blueprint, inv inventory, machine Machine) uint {
	shortage := array.ZipWith(inv.currency[:], bp.Costs[machine][:], func(have, need uint) uint {
		if need < have {
			return 0
		}
		return need - have
	})

	return array.ZipReduce(inv.harvester[:], shortage, func(growth, target uint) uint {
		if target == 0 {
			return 0
		}
		if growth == 0 {
			return math.MaxInt // aka forever
		}
		return (target-1)/growth + 1 // Rounded up
	}, fun.Max[uint], 0)
}

func (s *state) wait(w uint) *state {
	s.time -= w
	for _, o := range ores {
		s.inv.currency[o] += s.inv.harvester[o] * w
	}
	return s
}

func (s *state) purchase(bp Blueprint, machine Machine) *state {
	for ore, cost := range bp.Costs[machine] {
		s.inv.currency[ore] -= cost
	}
	s.inv.harvester[machine]++
	return s
}

func triangular(n uint) uint {
	return n*(n+1)/2 + 1
}

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	r1 := make(chan uint)
	r2 := make(chan uint)

	go func() {
		r1 <- Part1(input)
		close(r1)
	}()
	go func() {
		r2 <- Part2(input)
		close(r2)
	}()

	fmt.Fprintf(stdout, "Result of part 1: %d\n", <-r1)
	fmt.Fprintf(stdout, "Result of part 2: %d\n", <-r2)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ReadData reads the data file and a list of all cubes.
func ReadData() ([]Blueprint, error) {
	b, err := ReadDataFile()
	if err != nil {
		return nil, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	blueprints := []Blueprint{}

	for sc.Scan() {
		t := sc.Text()
		blueprints = append(blueprints, Blueprint{})
		b := &blueprints[len(blueprints)-1]
		fmt.Sscanf(t,
			"Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.",
			&b.ID,
			&b.Costs[Ore][Ore],
			&b.Costs[Clay][Ore],
			&b.Costs[Obsidian][Ore],
			&b.Costs[Obsidian][Clay],
			&b.Costs[Geode][Ore],
			&b.Costs[Geode][Obsidian],
		)
	}

	if sc.Scan() {
		return nil, errors.New("unexpected second line in input")
	}

	return blueprints, sc.Err()
}
