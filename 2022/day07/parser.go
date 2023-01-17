package day07

import (
	"errors"
	"fmt"
	"strconv"
	"strings"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/tree"
	"github.com/EduardGomezEscandell/algo/algo"
)

// StateMachine is a finite state machine that transitions between
// states defined as Cmd. Each state can have some variables,
// contained in Args.
//
// It constructs a filesystem (stored as a tree), where each
// file or directory is a FileDescriptor (a node in the tree)
//
// To transition, a line must be fed into it.
type StateMachine struct {
	// Current command and its args
	Cmd  state
	Args []string

	// Filesystem
	Fs      Filesystem
	cwdPath path

	// For better error reporting
	LineNo int
}

// size is the size of a file or group of files.
type size int

// state is an enum containing the states of the finite state machine. Most of the time,
// they refer to the command being run, or the command the response of which we're reading.
type state int

const (
	clean state = iota
	ls
	cd
	failed
)

// Linetype is an enum containing the type of text lines we may read.
type lineType int

const (
	command lineType = iota
	response
	empty
)

// NewParser creates a Parser and initializes its file system.
func NewParser() StateMachine {
	var p StateMachine
	p.Fs = tree.New[FileDescriptor]()
	p.Fs.Root.Data = &FileDescriptor{
		Name:  "/",
		IsDir: true,
	}
	p.cwdPath.Push(&p.Fs.Root)
	return p
}

func (c state) String() string {
	switch c {
	case clean:
		return "blank state"
	case ls:
		return "command ls"
	case cd:
		return "command cd"
	case failed:
		return "error state"
	}
	panic("Unknown state")
}

func (t lineType) String() string {
	switch t {
	case command:
		return "command"
	case response:
		return "response"
	case empty:
		return "empty"
	}
	panic("Unknown state")
}

// Cwd is the Current Working Directory of the finite state
// machine's filesystem.
func (p StateMachine) Cwd() *fsNode {
	return p.cwdPath.Peek()
}

// NextLine consumes the next line and advances the finite state machine.
func (p *StateMachine) NextLine(line string) (err error) {
	defer func() {
		if err == nil {
			return
		}
		path := algo.Map(p.cwdPath.Data(), func(f *fsNode) string { return f.Data.Name })
		err = fmt.Errorf("error in line %d\nline: %s\ncwd:  %s\ncmd:  %s\nargs: %s\nerr: %v", p.LineNo, line, strings.Join(path, "/"), p.Cmd, p.Args, err)
	}()
	p.LineNo++
	lineType, args := classifyLineType(line)

	switch lineType {
	case empty:
		p.Cmd = clean
		p.Args = nil
		return nil
	case command:
		return p.evalCommand(args[0], args[1:]...)
	case response:
		return p.evalResponse(args...)
	}

	panic("Unreachable")
}

// evalResponse parses and evaluates a line printed
// in response to a launched command.
func (p *StateMachine) evalResponse(args ...string) (err error) {
	defer func() {
		if err == nil {
			return
		}
		err = fmt.Errorf("parsing response: %v", err)
	}()
	return evalResponseMap[p.Cmd](p, args...)
}

// evalCommand evaluates the command specified.
func (p *StateMachine) evalCommand(verb string, args ...string) (err error) {
	defer func() {
		if err == nil {
			return
		}
		err = fmt.Errorf("parsing command: %v", err)
	}()

	eval, found := evalCmdMap[verb]
	if !found {
		verbs := []string{}
		for v := range evalCmdMap {
			verbs = append(verbs, v)
		}
		return fmt.Errorf("unknown verb %q.\nExpected any of: %s", verb, strings.Join(verbs, ", "))
	}

	return eval(p, args...)
}

// classifyLineType takes a raw string and decides whether it is
// a command, a response or an empty line.
func classifyLineType(line string) (lineType, []string) {
	data := strings.Split(line, " ")
	if len(data) == 0 {
		return empty, nil
	}
	if data[0] != "$" {
		return response, data
	}
	return command, data[1:]
}

// evalCmdMap contains the subroutines to be executed upon executing
// a specified command.
var evalCmdMap = map[string]func(*StateMachine, ...string) error{
	"ls": func(p *StateMachine, args ...string) error {
		p.Args = nil
		if len(args) != 0 {
			p.Cmd = failed
			return fmt.Errorf("unexpected token afer ls: ls accepts no arguments")
		}
		p.Cmd = ls
		return nil
	},
	"cd": func(p *StateMachine, args ...string) error {
		if len(args) == 0 {
			p.Cmd = failed
			return fmt.Errorf("unexpected endline after 'cd': cd needs exactly one argument")
		}
		if len(args) > 1 {
			p.Cmd = failed
			return fmt.Errorf("unexpected token after %q: cd needs exactly one argument", args[0])
		}
		return p.cd(args[0])
	},
}

// evalResponseMap contains the subroutines to be executed upon reading
// a response to the last command launched.
var evalResponseMap = map[state]func(*StateMachine, ...string) error{
	clean:  func(*StateMachine, ...string) error { return errors.New("unexpected response after blank state") },
	cd:     func(*StateMachine, ...string) error { return errors.New("unexpected response after command cd") },
	failed: func(*StateMachine, ...string) error { return errors.New("unexpected response after error state") },
	ls: func(p *StateMachine, fields ...string) error {
		if len(fields) != 2 {
			return fmt.Errorf("expected two tokens per line, found %d", len(fields))
		}
		// Is it a directory?
		if fields[0] == "dir" {
			return p.mkdir(fields[1])
		}
		// Is it a file?
		sz, err := strconv.Atoi(fields[0])
		if err != nil {
			return fmt.Errorf("Unrecognized first token. Should be either 'dir' or an integer")
		}
		return p.mkfile(fields[1], size(sz))
	},
}
