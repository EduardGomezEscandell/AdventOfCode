#include "matrix.h"

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






SparseMatrix NewSparseMatrix()
{
    SparseMatrix sp;

    sp.capacity = 10;

    sp.begin = malloc(sp.capacity * sizeof(SpTriplet));
    sp.end = sp.begin;

    sp.nrows = 0;
    sp.ncols = 0;

    return sp;
}

void ClearSparseMatrix(SparseMatrix * sp)
{
    free(sp->begin);

    sp->begin = NULL;
    sp->end = NULL;

    sp->capacity = 0;
}

void SpPush(SparseMatrix * sp, SpTriplet * t)
{
    const size_t size = sp->end - sp->begin;
    
    if(size >= sp->capacity)
    {
        sp->capacity *= 1.6;
        sp->begin = realloc(sp->begin, sp->capacity * sizeof(SpTriplet));
        sp->end = sp->begin + size;
    }

    *sp->end = *t;

    if(t->row >= sp->nrows) sp->nrows = t->row + 1;
    if(t->col >= sp->ncols) sp->ncols = t->col + 1;

    ++sp->end;
}

void SpAppend(SparseMatrix * reciever, const SparseMatrix * const giver)
{
    const size_t size = reciever->end - reciever->begin;
    const size_t appended_size = giver->end - giver->begin;

    if(size + appended_size >= reciever->capacity)
    {
        reciever->capacity = size + appended_size;
        reciever->begin = realloc(reciever->begin, reciever->capacity * sizeof(SpTriplet));
        reciever->end = reciever->begin + size;
    }

    if(giver->nrows > reciever->nrows) reciever->nrows = giver->nrows;
    if(giver->ncols > reciever->ncols) reciever->ncols = giver->ncols;

    for(SpTriplet const * it = giver->begin; it != giver->end; ++it)
    {
        *reciever->end = *it;
        ++reciever->end;
    }
}

int TripletCompare(SpTriplet * a, SpTriplet * b)
{
    if(a->row > b->row) return 1;
    
    if(a->row == b->row)
    {
        if(a->col > b->col) return 1;
        if(a->col == b->col) return 0;
    }
    
    return -1;
}

void SpPopZeros(SparseMatrix * sp)
{
    SpTriplet dummy;
    dummy.row = 0;
    dummy.col = 0;
    dummy.data = 1;
    SpPush(sp, &dummy);
    --sp->end;

    for(SpTriplet * it = sp->end; it != sp->begin; --it)
    {
        if(it->data == 0)
        {
            *it = *(sp->end-1);
            --sp->end;
        }
    }
}

DEFINE_QUICKSORT_COMP(SpQuickSort, SpTriplet, TripletCompare)

void SpMergeDuplicates(SparseMatrix * sp)
{
    SpQuickSort(sp->begin, sp->end);

    for(SpTriplet * it = sp->end - 1; it != sp->begin; --it)
    {
        if(TripletCompare(it, it-1) == 0)
        {
            (it-1)->data += it->data;
            *it = *(sp->end-1);
            --sp->end;
        }
    }
}

void SpPrint(SparseMatrix * sp)
{
    printf("SparseMatrix [%ld x %ld] {\n", sp->nrows, sp->ncols);
    for(SpTriplet * it = sp->begin; it != sp->end; ++it)
    {
        printf("    (%ld, %ld): %lld\n", it->row, it->col, it->data);
    }
    printf("}\n");
}

void SpPrintExpanded(SparseMatrix * sp)
{
    SpQuickSort(sp->begin, sp->end);
    SpTriplet * it = sp->begin;

    for(size_t i=0; i<sp->nrows; ++i)
    {
        for(size_t j=0; j<sp->ncols; ++j)
        {
            if(it != sp->end && it->row==i && it->col ==j)
            {
                printf("%lld", it->data);
                ++it;
            }
            else
            {
                printf(".");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void SpPrintSparsity(SparseMatrix * sp)
{
    SpQuickSort(sp->begin, sp->end);
    SpTriplet * it = sp->begin;

    for(size_t i=0; i<sp->nrows; ++i)
    {
        for(size_t j=0; j<sp->ncols; ++j)
        {
            if(it != sp->end && it->row==i && it->col ==j)
            {
                printf("#");
                ++it;
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
    printf("\n");
}
