package day07

import (
	"fmt"
	"strings"
	"sync"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/channel"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/charray"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/input"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/tree"
	"github.com/EduardGomezEscandell/algo/algo"
	"github.com/EduardGomezEscandell/algo/dstruct"
	"github.com/EduardGomezEscandell/algo/utils"
)

// Filesystem is a tree of file descriptors. It mocks a drive's filesystem.
type Filesystem = tree.Tree[FileDescriptor]

// fsNode is the node in the tree. It contains a pointer to a file
// descriptor and pointers to its children.
type fsNode = tree.Node[FileDescriptor]

// FileDescriptor contains data about a file/directory
// in the filesystem.
type FileDescriptor struct {
	Name  string
	Size  size
	IsDir bool
}

// ParseFilesystem reads the log line by line and reconstructs the observed
// filesystem.
func ParseFilesystem(input <-chan input.Line) (Filesystem, error) {
	p := NewParser()

	for ln := range input {
		if err := ln.Err(); err != nil {
			return p.Fs, fmt.Errorf("failed to read line %d: %v", p.LineNo+1, err)
		}
		err := p.NextLine(ln.Str())
		if err != nil {
			return p.Fs, err
		}
	}
	channel.Exhaust(input) // Sync point
	computeDirectorySizes(&p.Fs.Root)
	return p.Fs, nil
}

// path is a stack of nested file descriptors. It represents the route to
// get from the root to a particular file.
type path = dstruct.Stack[*fsNode]

// FsToStrig represents the filesystem as a list. Items are indented to
// show their nesting.
func FsToStrig(fs Filesystem) string {
	return fsNodeToString(fs.Root, "")
}

// FsToStrig represents a subtree of the filesystem as a nested list. Items
// are indented to show their nesting.
func fsNodeToString(fn fsNode, indent string) string {
	if !fn.Data.IsDir {
		return fmt.Sprintf("%s- %s (file, size=%d)", indent, fn.Data.Name, fn.Data.Size)
	}
	s := fmt.Sprintf("%s- %s (dir)", indent, fn.Data.Name)
	children := strings.Join(algo.Map(fn.Children, func(child *fsNode) string { return fsNodeToString(*child, indent+"  ") }), "\n")
	if len(children) == 0 {
		return s
	}
	return s + "\n" + children
}

// getChild finds if there is a child file with the specified name, and returns its
// index. Returns -1 if it is not found.
func getChild(dir fsNode, filename string) int {
	return algo.FindIf(dir.Children, func(f *fsNode) bool { return f.Data.Name == filename })
}

// mkdir creates a new directory with the specified name in the current working
// directory. It'll fail on collisions with pre-exisiting non-directory files.
func (sm *StateMachine) mkdir(name string) error {
	idx := getChild(*sm.Cwd(), name)
	if idx != -1 {
		original := sm.Cwd().Children[idx]
		if !original.Data.IsDir {
			return fmt.Errorf("Attempting to overwrite file %q as a directory", name)
		}
	}

	sm.Cwd().Children = append(sm.Cwd().Children, &fsNode{
		Data: &FileDescriptor{
			Name:  name,
			IsDir: true,
		},
	})

	return nil
}

// mkfile creates a new file with the specified name and size in the current working
// directory. It'll fail on collisions with pre-exisiting non-directory files, or with
// files with a different size.
func (sm *StateMachine) mkfile(name string, sz size) error {
	idx := getChild(*sm.Cwd(), name)
	if idx != -1 {
		original := sm.Cwd().Children[idx]
		if original.Data.IsDir {
			return fmt.Errorf("Attempting to overwrite directory %q as a file", name)
		}
		if sz != original.Data.Size {
			return fmt.Errorf("Attempting to overwrite file %q with different size:\n\toriginal:%d\n\tnew:     %d", name, original.Data.Size, sz)
		}
		return nil
	}
	sm.Cwd().Children = append(sm.Cwd().Children, &fsNode{
		Data: &FileDescriptor{
			Name: name,
			Size: sz,
		},
	})
	return nil
}

// cd changes the current working directory.
func (sm *StateMachine) cd(target string) error {
	// cd /
	if target == "/" {
		sm.cwdPath = path{}
		sm.cwdPath.Push(&sm.Fs.Root)
		return nil
	}

	// cd ..
	if target == ".." {
		if sm.cwdPath.Size() <= 1 {
			return fmt.Errorf("attempting to cd .. from root")
		}
		sm.cwdPath.Pop()
		return nil
	}

	// cd directory
	idx := getChild(*sm.Cwd(), target)
	if idx == -1 {
		sm.Cmd = failed
		return fmt.Errorf("cd: directory %q does not exist", target)
	}
	child := sm.Cwd().Children[idx]
	if !child.Data.IsDir {
		sm.Cmd = failed
		return fmt.Errorf("cd: file %q is not a directory", target)
	}
	sm.cwdPath.Push(child)
	return nil
}

// computeDirectorySizes computes the size of all files contained inside
// every directory of the file system.
func computeDirectorySizes(root *fsNode) size {
	if !root.Data.IsDir {
		return root.Data.Size
	}

	ch := make(chan size)
	var wg sync.WaitGroup

	algo.Foreach(root.Children, func(child **fsNode) {
		wg.Add(1)
		go func() {
			defer wg.Done()
			ch <- computeDirectorySizes(*child)
		}()
	})

	go func() {
		wg.Wait()
		close(ch)
	}()

	root.Data.Size = charray.Reduce(ch, utils.Add[size], 0)
	return root.Data.Size
}
