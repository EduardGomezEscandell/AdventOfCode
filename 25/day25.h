#ifndef DAY_25_H
#define DAY_25_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "common/testing.h"
#include "common/matrix.h"

typedef int solution_t;
enum CellType {EMPTY, EAST, SOUTH};

/**
 * This structure is used to avoid having to allocate a new matrix each turn
 * The current board state is always at the active matrix, and the state during
 * the previous half-move is at the buffer.
 * 
 * It is basicaly a circular array with only 2 entries.
 *
 * `NewMatrixFlipper` initializes the object, but not the matrices
 * `Flip` swaps the pointers (rotates the circular array)
 * `ClearMatrixFlipper` frees the allocated memory
 */
typedef struct
{
    Matrix * active;
    Matrix * buffer;
} MatrixFlipper;

MatrixFlipper NewMatrixFlipper();
void ClearMatrixFlipper(MatrixFlipper * f);
void Flip(MatrixFlipper * flipper);

// Input
void ParseLine(char * const line, Matrix * board);
Matrix ReadData(const bool is_test);
void PrintBoard(const Matrix * const mat);

/* Running
 * 
 * All these functions return true if any change has happened
 */
bool RunCell(Matrix * next, Matrix const * prev, size_t r, size_t c, enum CellType turn);
bool PerformHalfMove(MatrixFlipper * boards, enum CellType turn);
bool PerformMove(MatrixFlipper * boards);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
