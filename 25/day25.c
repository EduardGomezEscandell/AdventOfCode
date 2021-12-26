#include "day25.h"

#include "common/file_utils.h"
#include "common/matrix.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

MatrixFlipper NewMatrixFlipper()
{
    MatrixFlipper f;

    f.active = malloc(sizeof(*f.active));
    f.buffer = malloc(sizeof(*f.buffer));

    f.active->data = NULL;
    f.active->nrows = 0;
    f.active->ncols = 0;

    f.buffer->data = NULL;
    f.buffer->nrows = 0;
    f.buffer->ncols = 0;

    return f;
}

void ClearMatrixFlipper(MatrixFlipper * f)
{
    ClearMatrix(f->active);
    ClearMatrix(f->buffer);
    
    free(f->active);
    free(f->buffer);

    f->active = NULL;
    f->buffer = NULL;
}

void Flip(MatrixFlipper * flipper)
{
    Matrix * tmp = flipper->active;
    flipper->active = flipper->buffer;
    flipper->buffer = tmp;
}

void ParseLine(char * const line, Matrix * board)
{
    const size_t row = board->nrows;
    AddRow(board);

    size_t col = 0;
    for(char const *it=line; *it != '\n' && *it != '\0'; ++it)
    {
        switch (*it) {
            case '.': board->data[row][col++] = EMPTY; break;
            case '>': board->data[row][col++] = EAST;  break;
            case 'v': board->data[row][col++] = SOUTH; break;
        }

        if(col > board->ncols)
        {
            fprintf(stderr, "In file %s:%d:\nIrregularly sized matrix: ncols=%ld, line len >= %ld\n", __FILE__, __LINE__, board->ncols, col);
            exit(EXIT_FAILURE);
        }
    }
}


Matrix ReadData(const bool is_test)
{
    FILE* file = GetFile(is_test, 25);

    size_t len = 0;
    char * line = NULL;

    if(getline(&line, &len, file) == -1)
    {
        fprintf(stderr, "Empty file! (%s, %d)\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    size_t ncols = strlen(line) - 1; // -1: Excluding \n
    Matrix board = EmptyMatrix(0, ncols);

    ParseLine(line, &board);


    while(getline(&line, &len, file) != -1)
    {
        ParseLine(line, &board);
    }

    free(line);
    fclose(file);

    return board;
}

void PrintBoard(const Matrix * const mat)
{
    for(size_t r=0; r<mat->nrows; ++r)
    {
        for(size_t c=0; c<mat->ncols; ++c) {
            
            char out = 'X';
            switch (mat->data[r][c]) {
                case 0: out = '.'; break;
                case 1: out = '>'; break;
                case 2: out = 'v'; break;
            }
            printf("%c", out);
        }
        printf("\n");
    }
    printf("\n");
}

bool RunCell(Matrix * next, Matrix const * prev, size_t r, size_t c, enum CellType turn)
{
    long cucumber = prev->data[r][c];
    
    if(cucumber == EMPTY) return false;

    if(cucumber != turn)
    {
        next->data[r][c] = prev->data[r][c];
        return false;
    }

    size_t r_out = r;
    size_t c_out = c;

    
    switch (cucumber) 
    {
        case EAST:  c_out = (c + 1) % next->ncols; break;
        case SOUTH: r_out = (r + 1) % next->nrows; break;
    }

    if(prev->data[r_out][c_out] == EMPTY) // Movement allowed
    {
        next->data[r_out][c_out] = prev->data[r][c];
        return true;
    }

    // Movement blocked. Stays in place
    next->data[r][c] = prev->data[r][c];
    return false;
}

bool PerformHalfMove(MatrixFlipper * boards, enum CellType turn)
{
    // Resetting
    Flip(boards);
    FillMatrix(boards->active, EMPTY);

    // Eastwards movement
    bool any_changes = false;
    for(size_t i = 0; i < boards->active->nrows; ++i) {
        for(size_t j = 0; j < boards->active->ncols; ++j)
        {
            any_changes |= RunCell(boards->active, boards->buffer, i, j, turn);
        }
    }

    return any_changes;
}

bool PerformMove(MatrixFlipper * boards)
{
    bool any_changes = false;
    any_changes |= PerformHalfMove(boards, EAST);
    any_changes |= PerformHalfMove(boards, SOUTH);
    return any_changes;
}

solution_t SolvePart1(const bool is_test)
{
    MatrixFlipper boards = NewMatrixFlipper();

    *boards.active = ReadData(is_test);
    *boards.buffer = EmptyMatrix(boards.active->nrows, boards.active->ncols);

    size_t n_moves = 1;

    while(PerformMove(&boards))
    {
        ++n_moves;
    }

    ClearMatrixFlipper(&boards);

    return n_moves;
}

solution_t SolvePart2(const bool is_test)
{
    if(!is_test)
    {
        printf("\n\tMerry Christmas!\n\n");
    }

    return is_test;
}

DEFINE_TEST(1, 58)
DEFINE_TEST(2, 1)
