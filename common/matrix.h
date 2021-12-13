#ifndef MATRIX_INCLUDED_H
#define MATRIX_INCLUDED_H

#include <stdlib.h>
#include <stdio.h>

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

void PrintMatrix(const Matrix * const mat);
void ClearMatrix(Matrix * mat);

// Sparse matrices

typedef long spdata_type;

typedef struct
{
    size_t row;
    size_t col;
    spdata_type data;
} SpTriplet;

typedef struct
{
    SpTriplet * begin;
    SpTriplet * end;
    size_t capacity;
    size_t nrows;
    size_t ncols;
} SparseMatrix;

SparseMatrix NewSparseMatrix();
void ClearSparseMatrix(SparseMatrix * sp);

void SpPush(SparseMatrix * sp, SpTriplet * t);
void SpAppend(SparseMatrix * reciever, const SparseMatrix * const giver);

int TripletCompare(SpTriplet * a, SpTriplet * b);
void SpQuickSort(SparseMatrix * sp);

void SpPopZeros(SparseMatrix * sp);
void SpMergeDuplicates(SparseMatrix * sp);

void SpPrint(SparseMatrix * sp);
void SpPrintExpanded(SparseMatrix * sp);
void SpPrintSparsity(SparseMatrix * sp);

#endif
