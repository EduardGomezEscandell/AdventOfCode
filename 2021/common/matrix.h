#ifndef MATRIX_INCLUDED_H
#define MATRIX_INCLUDED_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "vector.h"
#include "hash_table.h"

// Dense matrices

typedef struct {
    long ** data;
    size_t ncols;
    size_t nrows;
    size_t crows;
    size_t ccols;
} Matrix;


Matrix EmptyMatrix(size_t rows, size_t cols);
void StringToRow(long * result, char * line, const size_t len);
Matrix ReadMatrix(FILE * file);

void AddRow(Matrix * mat);
void AddCol(Matrix * mat);

void FillMatrix(Matrix * mat, long value);

void PrintMatrix(const Matrix * const mat);
void ClearMatrix(Matrix * mat);

// Sparse matrices

typedef long long int spdata_type;

typedef struct
{
    ssize_t row;
    ssize_t col;
} SpIndex;

TEMPLATE_HASH_TABLE(size_t, SpIndex, spdata_type, DokMatrix);

typedef struct
{
    DokMatrix data;
    ssize_t nrows;
    ssize_t ncols;

    ssize_t n_negrows;
    ssize_t n_negcols;
} SparseMatrix;

SparseMatrix NewSparseMatrix();
void ClearSparseMatrix(SparseMatrix * sp);

spdata_type SpRead(SparseMatrix * sp, ssize_t row, ssize_t col);
size_t SpNumNonZero(SparseMatrix * sp);

void SpPush(SparseMatrix * sp, ssize_t row, ssize_t col, const spdata_type * value);
void SpAppend(SparseMatrix * reciever, const SparseMatrix * giver);
void SpReserve(SparseMatrix * sp, size_t n_entries);

int SpIndexCompare(const SpIndex * A, const SpIndex * B);
void SpPurgeZeros(SparseMatrix * sp);

void SpPrint(SparseMatrix * sp);
void SpPrintExpanded(SparseMatrix * sp, const char * format, const char * blank);
void SpPrintSparsity(SparseMatrix * sp);

#endif
