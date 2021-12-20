#ifndef DAY_19_ALGEBRA_INCLUDED_H
#define DAY_19_ALGEBRA_INCLUDED_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* Do not change! This is defined for readibility,
 * the code is not valid for values other than 3
 */
#define DIM 3 


/**
 *  Vector3D coordinate data type
 */
typedef int Int;

/**
 * Good'ol vectors. Can be accessed with indeces or coordinates:
 *
 *            vector.data[0] == vector.coord.x
 * 
 */
typedef union {
    Int data[3];
    struct {
        Int x,y,z;
    } coord;
} Vector3D;


/* A 3x3 matrix which:
 *  - In every row, and every column, there is a single non-zero value
 *  - All non-zero entries are either +1 or -1
 *  - Has determinant 1
 *  - The following property holds: X=D*Y, where:
 *    > X are the coordinates of a beacon in the reference basis
 *    > Y are the coordinates of a beacon in this scanner's basis
 *    > D is the transformation matrix from Y to X
 */
typedef struct { 
    unsigned short sparsity[DIM]; // Which column is non-zero for each row
    short values[DIM];            // The value of the non-zero entry (+1 or -1)
} Orientation;


Vector3D NewVector3D(Int x, Int y, Int z);
Vector3D VectorFromSourceAndDestination(Vector3D source, Vector3D destination);

Orientation ConstructOrientation(size_t permutation_id);

void accumulate(Vector3D * accumulator, Vector3D added);
Vector3D vecmult(Orientation const * D, Vector3D in);
Orientation matmul(Orientation const * A, Orientation const * B);
Orientation transpose(Orientation const * input);
Orientation inv(Orientation const * D);

size_t hash_vector3d(Vector3D const * v, size_t n_buckets);

bool eq(Vector3D A, Vector3D B);

void PrintOrientation(Orientation const * D);
void PrintVector3D(Vector3D v);



#endif
