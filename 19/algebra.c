#include "algebra.h"


#define SET_ARRAY(arr, x, y, z) do {        \
    arr[0]=x;                               \
    arr[1]=y;                               \
    arr[2]=z;                               \
} while(0);

Vector3D NewVector3D(Int x, Int y, Int z)
{
    Vector3D t;
    t.coord.x = x;
    t.coord.y = y;
    t.coord.z = z;
    return t;
}

Orientation ConstructOrientation(size_t permutation_id)
{
    if(permutation_id >= 24)
    {
        fprintf(stderr, "Invalid permutation! Must be less than 24\n");
        exit(EXIT_FAILURE);
    }

    Orientation D;

    switch (permutation_id % 6) {
        case 0: SET_ARRAY(D.sparsity, 0, 1, 2); break;
        case 1: SET_ARRAY(D.sparsity, 0, 2, 1); break;
        case 2: SET_ARRAY(D.sparsity, 1, 0, 2); break;
        case 3: SET_ARRAY(D.sparsity, 1, 2, 0); break;
        case 4: SET_ARRAY(D.sparsity, 2, 0, 1); break;
        case 5: SET_ARRAY(D.sparsity, 2, 1, 0); break;
    }

    switch (permutation_id / 6) {
        case 0: SET_ARRAY(D.values,  1, 1, 1); break;
        case 1: SET_ARRAY(D.values, -1,-1, 1); break;
        case 2: SET_ARRAY(D.values, -1, 1,-1); break;
        case 3: SET_ARRAY(D.values,  1,-1,-1); break;
    }

    return D;
}

// Matrix-vector product
Vector3D vecmult(Orientation * D, Vector3D in)
{
    Vector3D out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.data[i] = D->values[i] * in.data[D->sparsity[i]];
    }
    return out;
}

unsigned short which_row_has_column(unsigned short sparsity[DIM], unsigned short column)
{
    for(size_t i=0; i<DIM; ++i)
    {
        if(sparsity[i] == column) return i;
    }
    return -1;
}

// Matrix multiplication
Orientation matmul(Orientation * A, Orientation * B)
{
    Orientation out;
    for(size_t i=0; i<DIM; ++i)
    {
        short j =  which_row_has_column(B->sparsity, A->sparsity[i]);
        out.sparsity[i] = j;
        out.values[i]   = A->values[i] * B->values[j];
    }
    return out;
}

// Matrix inversion
Orientation inv(Orientation * D)
{
    Orientation out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.sparsity[i] = D->sparsity[i];
        out.values[i]   = D->values[(i+1)%DIM] * D->values[(i+2)%DIM];
    }
    return out;
}


/**
 * Computes out=A-B
 */
Vector3D sub(Vector3D A, Vector3D B)
{
    Vector3D out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.data[i] = A.data[i] - B.data[i];
    }
    return out;
}

bool eq(Vector3D A, Vector3D B)
{
    for(size_t i=0; i<DIM; ++i)
    {
        if(A.data[i] != B.data[i]) return false;
    }
    return true;
}

/** 
 * Prints the orientation as a 3x3 matrix
 */
void PrintOrientation(Orientation * D)
{
    for(size_t i=0; i<DIM; ++i)
    {
        printf("[");
        for(size_t j=0; j<DIM; ++j)
        {
            const short value = D->sparsity[i]==j ? D->values[i] : 0;
            printf("%2d ", value);
        }
        printf("]\n");
    }
    printf("\n");
}

void PrintVector3D(Vector3D v)
{
    /* They are printed horizontally for space purposes.
     * Mathematically they are treated as colum vectors
     */
    printf("[ %2d, %2d, %2d ]\n", v.coord.x, v.coord.y, v.coord.z);
}