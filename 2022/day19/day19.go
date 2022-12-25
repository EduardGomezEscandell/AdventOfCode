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
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 19
	fileName = "input.txt"
)

// Part1 solves the first half of today's problem.
func Part1(blueprints []Blueprint) (int, error) {
	var acc int
	for _, b := range blueprints {
		v, err := SolveBlueprint(b)
		if err != nil {
			return 0, err
		}
		acc += v * b.ID
	}
	return acc, nil
}

// Part2 solves the second half of today's problem.
func Part2([]Blueprint) (int, error) {
	return 1, nil
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
	ID    int
	Costs [4][3]int
}

type inventory struct {
	currency  [4]int
	harvester [4]int
}

func SolveBlueprint(bp Blueprint) (int, error) {
	inv := inventory{}
	inv.harvester[Ore] = 1

	best := dfs(bp, inv, 24)

	return best, nil
}

func dfs(bp Blueprint, inv inventory, time int) int {
	if time == 0 {
		return inv.currency[Geode]
	}

	best := -1
	for _, mach := range array.Reverse(machines[:]) {
		missing := shortage(bp, inv, mach)
		d := shortageDuration(inv, missing)
		if time-d < 0 {
			continue
		}
		d++ // It takes one minute to build the machine
		newInv := wait(inv, d)
		purchase(bp, &newInv, mach)
		v := dfs(bp, newInv, time-d)
		best = fun.Max(best, v)
	}

	if best == -1 {
		// If you can do nothing, then wait for the clock to run out
		return dfs(bp, wait(inv, time), 0)
	}

	return best
}

func wait(inv inventory, time int) inventory {
	for _, o := range ores {
		inv.currency[o] += inv.harvester[o] * time
	}
	return inv
}

func shortage(bp Blueprint, inv inventory, machine Machine) []int {
	return array.ZipWith(inv.currency[:], bp.Costs[machine][:], func(have, need int) int {
		return fun.Max(0, need-have)
	})
}

const Forever = math.MaxInt

func shortageDuration(inv inventory, shortage []int) int {
	return array.ZipReduce(inv.harvester[:], shortage, func(growth, target int) int {
		if target == 0 {
			return 0
		}
		if growth == 0 {
			return Forever
		}
		return (target-1)/growth + 1 // Rounded up
	}, fun.Max[int], 0)
}

func purchase(bp Blueprint, inv *inventory, machine Machine) {
	for ore, cost := range bp.Costs[machine] {
		inv.currency[ore] -= cost
	}
	inv.harvester[machine]++
}

// func dfs(bp Blueprint, inv inventory, waited bool, time int) int {
// 	if time == 0 {
// 		return inv.currency[Obsidian]
// 	}

// 	var best int
// 	// Each continuation contains the updated inventory with a newly
// 	// built machine.
// 	continuations := actions(bp, inv, waited)
// 	for _, newInv := range continuations {
// 		v := dfs(bp, newInv, false, time-1)
// 		best = fun.Max(v, best)
// 	}

// 	// If there is some machine that we cannot yet afford, it may make
// 	// sense to wait and accumulate resources until we can pay for it.
// 	if len(continuations) < len(machines) {
// 		for _, o := range ores {
// 			inv.currency[o] += inv.harvester[o]
// 		}
// 		v := dfs(bp, inv, true, time-1)
// 		best = fun.Max(v, best)
// 	}

// 	return best

// }

// func actions(bp Blueprint, inv inventory, waited bool) []inventory {
// 	newInv := inv
// 	for _, o := range ores {
// 		newInv.currency[o] += newInv.harvester[o]
// 	}

// 	continuations := []inventory{}

// 	for _, m := range machines {
// 		if !affordable(bp, newInv, m) {
// 			// Cannot afford to buy this machine
// 			continue
// 		}
// 		if waited && affordable(bp, inv, m) {
// 			// Could afford to buy this machine last turn, so it made no sense to wait
// 			continue
// 		}
// 		continuations = append(continuations, newInv)
// 		continuations[len(continuations)-1].harvester[m]++
// 	}

// 	return continuations
// }

// func affordable(bp Blueprint, inv inventory, machine Machine) bool {
// 	return array.ZipReduce(inv.currency[:], bp.Costs[machine][:], fun.Ge[int], fun.And, true)
// }

// ---------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	input, err := ReadData()
	if err != nil {
		return fmt.Errorf("error reading data: %v", err)
	}

	p1, err := Part1(input)
	if err != nil {
		return fmt.Errorf("error in part 1: %v", err)
	}
	fmt.Fprintf(stdout, "Result of part 1: %d\n", p1)

	p2, err := Part2(input)
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
