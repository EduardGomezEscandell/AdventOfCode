package day22

import (
	"fmt"

	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/array"
	"github.com/EduardGomezEscandell/AdventOfCode/2022/utils/fun"
)

type Topology interface {
	wrapHorizontally(world [][]Cell, row, col int, head Heading) (r, c int, h Heading)
	wrapVertically(world [][]Cell, row, col int, head Heading) (r, c int, h Heading)
}

type Toroidal struct{}

func (Toroidal) wrapHorizontally(world [][]Cell, row, col int, head Heading) (r, c int, h Heading) {
	c = 0
	r = row
	h = head

	end := len(world[r])
	incr := 1

	if h == Left {
		c = len(world[r]) - 1
		end = -1
		incr = -1
	}

	for ; c != end; c += incr {
		if world[r][c] != OffWorld {
			return
		}
	}
	return
}

func (Toroidal) wrapVertically(world [][]Cell, row, col int, head Heading) (r, c int, h Heading) {
	c = col
	r = 0
	h = head

	end := len(world)
	incr := 1

	if h == Up {
		r = len(world) - 1
		end = -1
		incr = -1
	}

	for ; r != end; r += incr {
		if world[r][c] != OffWorld {
			return
		}
	}
	return
}

type Cubic struct {
	faces    [6]face
	faceSize int
}

func (topo Cubic) wrapHorizontally(world [][]Cell, row, col int, heading Heading) (r, c int, h Heading) {
	return topo.wrap(world, row, col, heading)
}

func (topo Cubic) wrapVertically(world [][]Cell, row int, col int, heading Heading) (r, c int, h Heading) {
	return topo.wrap(world, row, col, heading)
}

func (topo Cubic) wrap(world [][]Cell, row, col int, heading Heading) (r, c int, h Heading) {
	// First: identify which cube we're in
	R := row / topo.faceSize
	C := col / topo.faceSize
	it := array.FindIf(topo.faces[:], func(f face) bool { return f.row == R && f.col == C })
	if it == -1 {
		panic(fmt.Errorf("cell %dx%d is outside of the cube", row, col))
	}
	relativeDirection := Heading(positiveMod(int(heading-1-topo.faces[it].top), 4))

	// Second: how far along the edge are we?
	var s int
	switch heading {
	case Right:
		s = row - R*topo.faceSize
	case Left:
		s = (R+1)*topo.faceSize - 1 - row
	case Down:
		s = (C+1)*topo.faceSize - 1 - col
	case Up:
		s = col - C*topo.faceSize
	}

	neighbour := cubeTopology[it][relativeDirection].id
	enteringFrom := cubeTopology[it][relativeDirection].side

	wallHugRelative := Steer(enteringFrom, Clockwise)
	wallHug := Heading(positiveMod(int(wallHugRelative+topo.faces[neighbour].top+1), 4))

	r = topo.faces[neighbour].row * topo.faceSize
	c = topo.faces[neighbour].col * topo.faceSize
	switch wallHug {
	case Right:
		c += (topo.faceSize - 1) - s
	case Left:
		r += topo.faceSize - 1
		c += s
	case Down:
		c += topo.faceSize - 1
		r += (topo.faceSize - 1) - s
	case Up:
		r += s
	}
	h = Steer(wallHug, Clockwise)
	return
}

func NewCubicTopology(world [][]Cell, faceSize int) Cubic {
	// Obtaining sparsity pattern
	filled := [4][4]bool{} // At most, the unfolded can take up 4x3 OR 3x4
	for i := 0; i < 4; i++ {
		I := i * faceSize
		if I >= len(world) {
			break
		}

		for j := 0; j < 4; j++ {
			J := j * faceSize
			if J >= len(world[I]) {
				break
			}
			filled[i][j] = world[I][J] != OffWorld
		}
	}

	// Generating cube
	topology := Cubic{faceSize: faceSize}
	topology.faces[0] = face{top: Up, row: 0, col: array.FindIf(filled[0][:], fun.Identity[bool]), init: true}
	parseFaces(filled, 0, &topology.faces)

	return topology
}

func parseFaces(folded [4][4]bool, id int, faces *[6]face) {
	r := faces[id].row
	c := faces[id].col
	look := faces[id].top

	// Looking to its right, bottom, left and above
	for side := Right; side < 4; side++ {
		look = Steer(look, Clockwise)
		dr, dc := headingToDiff(look)
		if (0 <= r+dr && r+dr < 4) && (0 <= c+dc && c+dc < 4) && folded[r+dr][c+dc] {
			other := cubeTopology[id][side]
			if faces[other.id].init {
				continue
			}
			orientation := Heading(positiveMod(1+int(look)-int(other.side), 4))
			faces[other.id] = face{row: r + dr, col: c + dc, top: orientation, init: true}
			parseFaces(folded, other.id, faces)
		}
	}
}

/*
		 +---+
		 | 0 |
	 +---+---+---+---+
	 | 4 | 1 | 2 | 3 |
	 +---+---+---+---+
		 | 5 |
		 +---+
*/
var cubeTopology = [6][4]contactedSquare{
	// What edge each face touches
	// "0: {Left: {4, Right} ..." means that the left edge of face 0 is the same as face 4's right edge
	0: {Left: {4, Up}, Right: {2, Up}, Up: {3, Up}, Down: {1, Up}},
	1: {Left: {4, Right}, Right: {2, Left}, Up: {0, Down}, Down: {5, Up}},
	2: {Left: {1, Right}, Right: {3, Left}, Up: {0, Right}, Down: {5, Right}},
	3: {Left: {2, Right}, Right: {4, Left}, Up: {0, Up}, Down: {5, Down}},
	4: {Left: {3, Right}, Right: {1, Left}, Up: {0, Left}, Down: {5, Left}},
	5: {Left: {4, Down}, Right: {2, Down}, Up: {1, Down}, Down: {3, Down}},
}

type face struct {
	row, col int
	top      Heading
	init     bool
}

type contactedSquare struct {
	id   int
	side Heading
}
