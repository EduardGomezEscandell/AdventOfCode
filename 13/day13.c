#include "day13.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common/file_utils.h"
#include "common/matrix.h"
#include "common/vector.h"

DEFINE_TEST(1, 17)
DEFINE_TEST(2, 1)

void ReadLine(SparseMatrix * sp, char * line)
{
    char * begin = line;
    size_t i=0;
    long data[2] = {-1, -1};

    for(char * it = line; *it != '\0'; ++it)
    {
        if(*it == ',' || *it == '\n')
        {
            *it = '\0';
            data[i++] = atol(begin);
            begin = it+1;
        }
    }
    const size_t row = data[1];
    const size_t col = data[0];
    const spdata_type value = 1;
    
    SpPush(sp, row, col, &value);
}

void ReadData(FILE * file, SparseMatrix * sp, FoldVector * folds)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    *sp = NewSparseMatrix();
    NEW_VECTOR(*folds);

    while((read = getline(&line, &len, file)) != -1)
    {
        if(*line == '\n') break;

        ReadLine(sp, line);
    }

    Fold f;
    while((read = getline(&line, &len, file)) != -1)
    {
        f.axis = line[11];
        f.value = atoi(&line[13]);
        PUSH(*folds, f);
    }

    free(line);
    fclose(file);
}

void YFold(SparseMatrix * sp, const size_t y_fold)
{
    const size_t size = SIZE(sp->data.data);
    for(size_t i=0; i < size; ++i)
    {
        DokMatrixPair * it = sp->data.data.begin + i;

        if(it->key.row < y_fold) continue;
 
        spdata_type value = it->value;
        it->value = 0;
        
        if(it->key.row > y_fold)
        {
            const size_t row = 2*y_fold - it->key.row;
            const size_t col = it->key.col;
            SpPush(sp, row, col, &value);
        }
    }

    sp->nrows = y_fold;
    SpPurgeZeros(sp);
}

void XFold(SparseMatrix * sp, const size_t x_fold)
{
    const size_t size = SIZE(sp->data.data);
    for(size_t i=0; i < size; ++i)
    {
        DokMatrixPair * it = sp->data.data.begin + i;

        if(it->key.col < x_fold) continue;

        spdata_type value = it->value;
        it->value = 0;
 
        if(it->key.col > x_fold)
        {
            const size_t row = it->key.row;
            const size_t col = 2*x_fold - it->key.col;
            SpPush(sp, row, col, &value);
        }
    }

    sp->ncols = x_fold;
    SpPurgeZeros(sp);
}


void MatrixPrint(const DokMatrix * ht)
{
    printf("{\n");
    
    // printf("  Bucket with hash %ld {\n", b - ht->buckets.begin);     
    for(DokMatrixPair * it =  ht->data.begin; it != ht->data.end; ++it)  
    { 
        printf("    ");      
        printf("[%ld, %ld]", it->key.row, it->key.col);
        printf(" : ");
        printf(" %lld ", it->value);
        printf("\n"); 

        it->value = 1;
    } 
    printf("  }\n");  
    
}

int SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 13);

    SparseMatrix sp;
    FoldVector folds;

    ReadData(file, &sp, &folds);

    if(folds.begin == folds.end)
    {
        fprintf(stderr, "No fold instructions!");
        ClearSparseMatrix(&sp);
        CLEAR(folds);
        exit(EXIT_FAILURE);
    }
    
    switch(folds.begin->axis)
    {
        case 'x': XFold(&sp, folds.begin->value); break;
        case 'y': YFold(&sp, folds.begin->value); break;
        default:
            fprintf(stderr, "Wrong axis: %c!\n", folds.begin->axis);
            ClearSparseMatrix(&sp);
            CLEAR(folds);
            exit(EXIT_FAILURE);
    }

    const size_t n_entries = SIZE(sp.data.data);

    ClearSparseMatrix(&sp);
    CLEAR(folds);

    return n_entries;
}

int SolvePart2(const bool is_test)
{
    FILE * file = GetFile(is_test, 13);

    SparseMatrix sp;
    FoldVector folds;

    ReadData(file, &sp, &folds);
    
    for(Fold * it=folds.begin; it != folds.end; ++it)
    {
        switch(it->axis)
        {
            case 'x': XFold(&sp, it->value); break;
            case 'y': YFold(&sp, it->value); break;
            default:
                fprintf(stderr, "Wrong axis: %c!\n", folds.begin->axis);
                ClearSparseMatrix(&sp);
                CLEAR(folds);
                exit(EXIT_FAILURE);
        }
    }

    SpPrintSparsity(&sp);

    // Yeah... not going to implement an intepreter for this.
    // Solution for my dataset looks like:
    // 
    // ###  #### #  # #### #    ###   ##  #  # 
    // #  #    # # #     # #    #  # #  # #  # 
    // #  #   #  ##     #  #    #  # #    #### 
    // ###   #   # #   #   #    ###  # ## #  # 
    // # #  #    # #  #    #    #    #  # #  # 
    // #  # #### #  # #### #### #     ### #  # 
    // 

    ClearSparseMatrix(&sp);
    CLEAR(folds);

    return is_test;
}
