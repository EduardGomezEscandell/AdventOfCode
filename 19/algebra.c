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

Int LeviCivita(const unsigned short indices[DIM])
{
    const unsigned short i = indices[0];
    const unsigned short j = indices[1];
    const unsigned short k = indices[2];
    return ((i+1)%DIM==j && (i+2)%DIM==k) ? 1 : -1;
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

    Int e = LeviCivita(D.sparsity);

    switch (permutation_id / 6) {
        case 0: SET_ARRAY(D.values,  e, e, e); break;
        case 1: SET_ARRAY(D.values, -e,-e, e); break;
        case 2: SET_ARRAY(D.values, -e, e,-e); break;
        case 3: SET_ARRAY(D.values,  e,-e,-e); break;
    }

    return D;
}

// accumulator += added
void accumulate(Vector3D * accumulator, Vector3D added)
{
    for(size_t i=0; i<DIM; ++i)
    {
        accumulator->data[i] += added.data[i];
    }
}

// Matrix-vector product
Vector3D vecmult(Orientation const * D, Vector3D in)
{
    Vector3D out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.data[i] = D->values[i] * in.data[D->sparsity[i]];
    }
    return out;
}

size_t find_row_with_non_zero_column(const unsigned short sparsity[DIM], unsigned short non_zero_column)
{
    for(size_t i=0; i<DIM; ++i)
    {
        if(sparsity[i] == non_zero_column) return i;
    }
    return -1;
}

// Matrix multiplication
Orientation matmul(Orientation const * A, Orientation const * B)
{
    Orientation out;
    for(size_t i=0; i<DIM; ++i)
    {
        short j = find_row_with_non_zero_column(B->sparsity, A->sparsity[i]);
        out.sparsity[i] = j;
        out.values[i]   = A->values[i] * B->values[j];
    }
    return out;
}

Orientation transpose(Orientation const * input)
{
    Orientation out;
    for(size_t i=0; i<DIM; ++i)
    {
        const Int col = find_row_with_non_zero_column(input->sparsity, i);
        out.sparsity[i] = col;
        out.values[i] = input->values[col];
    }
    return out;
}

// Matrix inversion
Orientation inv(Orientation const * D)
{
    return transpose(D); // Bless you orthonormal matrices
}


/**
 * Computes out=A-B
 */
Vector3D VectorFromSourceAndDestination(Vector3D source, Vector3D destination)
{
    Vector3D out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.data[i] = destination.data[i] - source.data[i];
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

size_t hash_vector3d(Vector3D const * v, size_t n_buckets)
{
    Int mod = 0;
    for(Int i=0; i<DIM; ++i)
        mod += v->data[i];
    return mod % n_buckets;
}

/** 
 * Prints the orientation as a 3x3 matrix
 */
void PrintOrientation(Orientation const * D)
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
