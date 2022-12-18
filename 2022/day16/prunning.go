package day16

import "github.com/EduardGomezEscandell/AdventOfCode/2022/utils/lrucache"

type prunningTool struct {
	bestRemaining []Score
	bestScore     Score
	cache         *lrucache.LruCache[worldState, Score]
}

type worldState struct {
	location [2]VID
	opened   checklist
	timeLeft int
}

func newPrunningTool(world []Valve, time int, elephantsAllowed bool) *prunningTool {
	return &prunningTool{
		bestRemaining: bestRemainingScore(world, time, elephantsAllowed),
		cache:         lrucache.New[worldState, Score](10_000_000),
	}
}

func (pi *prunningTool) canPrune(score Score, timeLeft int) bool {
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
