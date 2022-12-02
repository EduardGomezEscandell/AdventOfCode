// Package day02 solves day 2 of AoC2022	.
package day02

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
	today    = 2
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(input []Round) (uint, error) {
	return array.MapReduce(input, (Round).ScoreP1, fun.Add[uint]), nil
}

// Part2 solves the second half of the problem.
func Part2(input []Round) (uint, error) {
	return array.MapReduce(input, (Round).ScoreP2, fun.Add[uint]), nil
}

// ---------------- Implementation -----------------------

// The end result. The numeric value is 1,2,3 to make the input
// easier to parse, and coincidentally that is also their score.
const (
	Rock uint = iota + 1
	Paper
	Scissors
)

// The end result. The numeric value is 1,2,3 to make the input
// easier to parse. The score is obtained by subtracting 1 and
// multiplying by three: Lose, Draw, Win => 0,3,6.
const (
	Lose uint = iota + 1
	Draw
	Win
)

// Computes the score given your hand and the result of the round.
func score(hand uint, result uint) uint {
	return hand + (result-1)*3
}

// A Round of the RPS game. The first entry is always your oponent's
// hand, but the second entry depends on which part of the problem
// you are solving. Given a correct input, they are always values
// between 1 and 3.
type Round struct {
	First  uint // Oponent's Hand
	Second uint // Your hand in part1, result in part 2
}

// ScoreP1 interprets the hand as in the first part of the problem:
// (opponent, you) and returns your score.
func (r Round) ScoreP1() uint {
	return scoresP1[r]
}

// ScoreP2 interprets the hand as in the second part of the problem:
// (opponent, result) and returns your score.
func (r Round) ScoreP2() uint {
	return scoresP2[r]
}

// Part 1 result cache.
var scoresP1 = map[Round]uint{
	{Rock, Rock}:     score(Rock, Draw),
	{Paper, Rock}:    score(Rock, Lose),
	{Scissors, Rock}: score(Rock, Win),

	{Rock, Paper}:     score(Paper, Win),
	{Paper, Paper}:    score(Paper, Draw),
	{Scissors, Paper}: score(Paper, Lose),

	{Rock, Scissors}:     score(Scissors, Lose),
	{Paper, Scissors}:    score(Scissors, Win),
	{Scissors, Scissors}: score(Scissors, Draw),
}

// Part 2 result cache.
var scoresP2 = map[Round]uint{
	{Rock, Lose}: score(Scissors, Lose),
	{Rock, Draw}: score(Rock, Draw),
	{Rock, Win}:  score(Paper, Win),

	{Paper, Lose}: score(Rock, Lose),
	{Paper, Draw}: score(Paper, Draw),
	{Paper, Win}:  score(Scissors, Win),

	{Scissors, Lose}: score(Paper, Lose),
	{Scissors, Draw}: score(Scissors, Draw),
	{Scissors, Win}:  score(Rock, Win),
}

// ------------- Here be boilerplate ------------------

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	inp, err := ParseInput()
	if err != nil {
		return err
	}

	result, err := Part1(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 1: %v\n", result)

	result, err = Part2(inp)
	if err != nil {
		return err
	}
	fmt.Fprintf(stdout, "Result of part 2: %v\n", result)

	return nil
}

// ReadDataFile is a wrapper around input.ReadDataFile made to be
// easily mocked.
var ReadDataFile = func() ([]byte, error) {
	return input.ReadDataFile(today, fileName)
}

// ParseInput reads the data file and returns the list of
// reindeers and their calories.
func ParseInput() (data []Round, err error) {
	b, err := ReadDataFile()
	if err != nil {
		return data, err
	}

	sc := bufio.NewScanner(bytes.NewReader(b))
	data = []Round{}
	for sc.Scan() {
		line := sc.Text()
		var rival, you rune
		_, err := fmt.Sscanf(line, "%c %c", &rival, &you)
		if err != nil {
			return nil, err
		}
		data = append(data, Round{
			First:  (1 + uint(rival) - uint('A')),
			Second: (1 + uint(you) - uint('X')),
		})
	}

	if err := sc.Err(); err != nil {
		return data, err
	}

	return data, nil
}
