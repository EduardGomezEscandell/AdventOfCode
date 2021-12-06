#ifndef DAY_04_H
#define DAY_04_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	unsigned short value;
	bool marked;
} Cell;

typedef struct {
	Cell data[5][5];
	bool finished;
} Board;

typedef struct {
	Board * begin;
	Board * end;
	size_t capacity;
} BoardVector;


// Memory management
BoardVector NewBoardVector();
size_t BVLength(const BoardVector v);
Board * BVExpand(BoardVector * v);

// Reading bingo
void FillRow(Cell row[9], char * line);
BoardVector ReadBoards(FILE* file);

// Playing bingo
Cell * UpdateBoard(Board * b, const int n);
int CountPoints(const Board b, int winner_number);

// Solving
int SolvePart1(const bool is_test);
int SolvePart2(const bool is_test);

#endif
