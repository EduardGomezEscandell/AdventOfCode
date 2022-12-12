// Package day10 solves day 10 of AoC2022.
package day10

import (
	"context"
	"fmt"
	"io"
	"strconv"
	"strings"
	"time"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
)

const (
	today    = 10
	fileName = "input.txt"
)

// Part1 solves the first half of the problem.
func Part1(in <-chan input.Line) (int, error) {
	signalStrength := 0
	cpu := newCPU(in)
	for !cpu.done {
		cpu.counter++
		if (cpu.counter-20)%40 == 0 {
			signalStrength += cpu.X * cpu.counter
		}
		cpu.instr.eval()
	}

	if cpu.err != nil {
		return 0, cpu.err
	}

	return signalStrength, nil
}

// Part2 solves the second half of the problem.
func Part2(in <-chan input.Line) (string, error) {
	screen := array.Generate(6, func() []rune { return array.Generate(40, func() rune { return ' ' }) })
	cpu := newCPU(in)
	for !cpu.done {
		col := cpu.counter % 40
		row := cpu.counter / 40

		if fun.Abs(cpu.X-col) < 2 {
			screen[row][col] = '#'
		}

		cpu.counter++
		cpu.instr.eval()
	}

	if cpu.err != nil {
		return "", cpu.err
	}

	return showScreen(screen), nil
}

// ------------- Implementation ------------------

func showScreen(screen [][]rune) string {
	return strings.Join(array.Map(screen, func(r []rune) string { return string(r) }), "\n")
}

func newCPU(in <-chan input.Line) *cpu {
	c := cpu{
		instrPipe: in,
		err:       nil,
		done:      false,
		X:         1,
		counter:   0,
	}
	c.instr = fetchDecode{cpu: &c}
	return &c
}

type cpu struct {
	instrPipe <-chan input.Line // pipe to receive instructions from
	instr     instruction       // next instruction to evaluate
	err       error             // error state of the CPU
	done      bool              // indicates if the machine is done running
	X         int               // X register
	counter   int               // instruction counter
}

type instruction interface {
	eval()
}

type addx struct {
	cpu *cpu
	v   int
}

func (a addx) eval() {
	a.cpu.X += a.v
	a.cpu.instr = fetchDecode{a.cpu}
}

type fetchDecode struct {
	cpu *cpu
}

func (f fetchDecode) eval() {
	// Fetch
	ln, read := <-f.cpu.instrPipe
	if !read {
		f.cpu.done = true
		return
	}
	if ln.Err() != nil {
		f.cpu.err = ln.Err()
		f.cpu.done = true
		return
	}
	// Decode
	instr := strings.Fields(ln.Str())
	if len(instr) == 1 && instr[0] == "noop" {
		f.cpu.instr = fetchDecode{f.cpu}
		return
	}
	if len(instr) == 2 && instr[0] == "addx" {
		v, err := strconv.Atoi(instr[1])
		if err != nil {
			f.cpu.err = fmt.Errorf("non-integer argument to addx: %s", ln.Str())
			f.cpu.done = true
			return
		}
		f.cpu.instr = addx{f.cpu, v}
		return
	}
	f.cpu.err = fmt.Errorf("unknown instruction: %s", ln.Str())
	f.cpu.done = true
}

// ------------- Here be boilerplate ------------------

type problemResult struct {
	id  int
	res string
	err error
}

// Main is the entry point to today's problem solution.
func Main(stdout io.Writer) error {
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	reader, err := DataReader()
	if err != nil {
		return err
	}

	ch, err := input.ReadDataAsync(ctx, reader, 10)
	if err != nil {
		return err
	}

	channels := channel.Split(ctx, ch, 2)

	resultCh := make(chan problemResult)
	go func() {
		result, err := Part1(channels[0])
		channel.Exhaust(channels[0])
		if err != nil {
			resultCh <- problemResult{0, "", err}
			cancel()
			return
		}
		resultCh <- problemResult{0, fmt.Sprintf("Result of part 1: %v", result), nil}
	}()

	go func() {
		result, err := Part2(channels[1])
		channel.Exhaust(channels[1])
		if err != nil {
			resultCh <- problemResult{1, "", err}
			cancel()
			return
		}
		resultCh <- problemResult{1, fmt.Sprintf("Result of part 2:\n%s", result), nil}
	}()

	var results [2]problemResult
	for i := 0; i < 2; i++ {
		r := <-resultCh
		results[r.id] = r
	}

	for _, v := range results {
		if v.err != nil {
			return v.err
		}
		fmt.Fprintln(stdout, v.res)
	}

	return nil
}

// DataReader is a wrapper around input.DataFile made to be
// easily mocked.
var DataReader = func() (r io.ReadCloser, e error) {
	f, e := input.DataFile(today, fileName)
	if e != nil {
		return nil, e
	}
	return f, nil
}
