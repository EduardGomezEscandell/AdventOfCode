#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void StringToRow(long * result, char * line, const size_t len)
{
    for(size_t col=0; col<len; ++col)
    {
        result[col] = line[col] - '0';
    }
}

Matrix EmptyMatrix(size_t rows, size_t cols)
{
    Matrix mat;

    size_t min_capacity = 10;

    mat.crows = rows > min_capacity ? rows : min_capacity;
    mat.ccols = cols > min_capacity ? cols : min_capacity;

    mat.nrows = rows;
    mat.ncols = cols;

    mat.data = malloc(mat.crows * sizeof(long*));

    for(size_t row=0; row < mat.nrows; ++row)
    {
        mat.data[row] = calloc(mat.ccols, sizeof(long));
    }

    return mat;
}

void AddRow(Matrix * mat)
{
    if(mat->nrows >= mat->crows)
    {
        mat->crows *= 1.6;
        mat->data = realloc(mat->data, mat->crows * sizeof(long*));
    }

    mat->data[mat->nrows] = calloc(mat->ccols, sizeof(long));

    ++mat->nrows;
}

void AddCol(Matrix * mat)
{
    if(mat->ncols >= mat->ccols)
    {
        mat->ccols *= 1.6;
        for(size_t row=0; row < mat->nrows; ++mat) {
            mat->data[row] = realloc(mat->data[row], mat->ccols * sizeof(long));
            mat->data[row][mat->ncols] = 0;
        }
    }
    else
    {
        for(size_t row=0; row < mat->nrows; ++mat)
            mat->data[row][mat->ncols] = 0;
    }
    ++mat->ncols;
}


Matrix ReadMatrix(FILE * file)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    Matrix mat;

    size_t row = 0;
    while((read = getline(&line, &len, file)) != -1)
    {
        if(row == 0) {
            mat = EmptyMatrix(0, strlen(line) - 1);
        }

        AddRow(&mat);
        StringToRow(mat.data[row], line, mat.ncols);

        ++row;
    }

    // PrintMatrix(&mat);

    free(line);
    fclose(file);

    return mat;
}


void ClearMatrix(Matrix * mat)
{
    for(size_t row=0; row<mat->nrows; ++row)
    {
        free(mat->data[row]);
        mat->data[row] = NULL;
    }
    free(mat->data);
    mat->data = NULL;
    mat->ncols = 0;
    mat->nrows = 0;
    mat->ccols = 0;
    mat->crows = 0;
}

void PrintMatrix(const Matrix * const mat)
{
    for(size_t y=0; y<mat->nrows; ++y)
    {
        for(size_t x=0; x<mat->ncols; ++x)
            if( mat->data[y][x] > 9) printf("%c", (char) ('a' + mat->data[y][x] - 10));
            else if( mat->data[y][x] < 0) printf("%c", (char) ('A' - mat->data[y][x] - 1));
            else                     printf("%ld", mat->data[y][x]);
        printf("\n");
    }
    printf("\n");
}



// Dictionary of keys sparse matrix


HT_DEFINE_SET_COMPARISON    (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_NEW_AND_CLEAR     (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_FIND              (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_FIND_OR_EMPLACE   (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_RESERVE           (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_REMOVE            (size_t, SpIndex, spdata_type, DokMatrix)
HT_DEFINE_PRINT             (size_t, SpIndex, spdata_type, DokMatrix)

size_t HashFun(const SpIndex * key, size_t n_buckets)
{
    return (key->row * key->row + key->col * key->col) % n_buckets;
}

SparseMatrix NewSparseMatrix()
{
    SparseMatrix sp;

    sp.data = NewDokMatrix(HashFun);
    DokMatrixSetComparison(&sp.data, SpIndexCompare);

    sp.nrows = 0;
    sp.ncols = 0;
    sp.n_negrows = 0;
    sp.n_negcols = 0;

    return sp;
}

void ClearSparseMatrix(SparseMatrix * sp)
{
    ClearDokMatrix(&sp->data);
    sp->ncols = 0;
    sp->nrows = 0;
}

void SpPush(SparseMatrix * sp, ssize_t row, ssize_t col, const spdata_type * value)
{
    if(*value == 0) return;

    SpIndex key = {row, col};
    *DokMatrixFindOrEmplace(&sp->data, key, 0) += *value;

    if(row >= sp->nrows) sp->nrows = row + 1;
    if(col >= sp->ncols) sp->ncols = col + 1;

    if(row < sp->n_negrows) sp->n_negrows = row;
    if(col < sp->n_negcols) sp->n_negcols = col;
}

void SpAppend(SparseMatrix * reciever, const SparseMatrix * giver)
{
    DokMatrixReserve(&reciever->data, SIZE(reciever->data.data) + SIZE(giver->data.data));

    for(const DokMatrixPair * it = giver->data.data.begin; it != giver->data.data.end; ++it)
    {
        if(it->value == 0) continue;
        spdata_type res = *DokMatrixFindOrEmplace(&reciever->data, it->key, 0) += it->value;

        if(res != 0) continue;

        DokMatrixRemove(&reciever->data, it->key);
    }

    reciever->nrows = MAX(reciever->nrows, giver->nrows);
    reciever->ncols = MAX(reciever->ncols, giver->ncols);

    reciever->n_negrows = MIN(reciever->n_negrows, giver->n_negrows);
    reciever->n_negcols = MIN(reciever->n_negcols, giver->n_negcols);
}

int SpIndexCompare(const SpIndex * a, const SpIndex * b)
{
    return (a->row==b->row && a->col==b->col) ? 0 : 1;
}

void SpPrint(SparseMatrix * sp)
{
    printf("SparseMatrix [%ld x %ld] {\n", sp->nrows, sp->ncols);
    for(DokMatrixPair * it = sp->data.data.begin; it != sp->data.data.end; ++it)
    {
        printf("    (%ld, %ld): %lld\n", it->key.row, it->key.col, it->value);
    }
    printf("}\n");
}

void SpPurgeZeros(SparseMatrix * sp)
{
    for(DokMatrixPair * it = sp->data.data.end-1; it+1 != sp->data.data.begin; --it)
    {
        if(it->value == 0)
            DokMatrixRemove(&sp->data, it->key);
    }
}

void SpPrintExpanded(SparseMatrix * sp, const char * format, const char * blank)
{
    SpIndex index;
    for(index.row=0; index.row<sp->nrows; ++index.row)
    {
        for(index.col=0; index.col<sp->ncols; ++index.col)
        {
            DokMatrixSearchResult res = DokMatrixFind(&sp->data, &index);
            if(res.pair)
                printf(format, res.pair->value);
            else
                printf("%s", blank);
        }
        printf("\n");
    }
    printf("\n");
}

void SpPrintSparsity(SparseMatrix * sp)
{
    SpIndex index;
    for(index.row=sp->n_negrows; index.row<sp->nrows; ++index.row)
    {
        printf("[");
        if(sp->n_negrows < 0 && index.row==0){
            for(ssize_t col=sp->n_negcols; col<sp->ncols; ++col){
                if(sp->n_negcols < 0 && col==0) printf("+");
                else                            printf("-");
            }
            if(sp->n_negcols < 0) printf("-");
            printf("]\n[");
        }

        for(index.col=sp->n_negcols; index.col<sp->ncols; ++index.col)
        {
            if(sp->n_negcols < 0 && index.col==0) printf("|");

            DokMatrixSearchResult res = DokMatrixFind(&sp->data, &index);
            if(res.pair)
                printf("#");
            else
                printf(" ");
        }
        printf("]\n");
    }
    printf("\n");
}

spdata_type SpRead(SparseMatrix * sp, ssize_t row, ssize_t col)
{
    SpIndex index = {row, col};
    DokMatrixSearchResult res = DokMatrixFind(&sp->data, &index);

    if(!res.pair) return 0;

    return res.pair->value;
}

void SpReserve(SparseMatrix * sp, size_t n_entries)
{
    DokMatrixReserve(&sp->data, n_entries);
}

size_t SpNumNonZero(SparseMatrix * sp)
{
    SpPurgeZeros(sp);
    return SIZE(sp->data.data);
}
