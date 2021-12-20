#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "07/day7.h"
#include "common/testing.h"
#include "common/vector.h"

typedef int solution_t; // Just in case we have to switch to long
typedef int Int; // Vector3D coordinate data type

TEMPLATE_VECTOR(int) IntArray;

/* Do not change! This is defined for readibility,
 * the code is not valid for values other than 3
 */
#define DIM 3 

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
 *    > X are the coordinates of a beacons in the reference basis
 *    > Y are the coordinates of a beacon in this scanner's basis
 *    > D is the orientation matrix 
 */
typedef struct { 
    unsigned short sparsity[DIM]; // Which column is non-zero for each colum
    short values[DIM];            // The value of the non-zero entry (+1 or -1)
} Orientation;

typedef struct 
{
    size_t id;
    Vector3D data;
} Beacon;

TEMPLATE_VECTOR(Vector3D) PointsArray;
TEMPLATE_VECTOR(Beacon) BeaconArray;

struct scanner_connection;
TEMPLATE_VECTOR(struct scanner_connection) ConnexionArray;

typedef struct scanner
{
    size_t id; // Probably won't use it
    
    ConnexionArray connections;
    bool connected;

    Orientation orientation;
    Vector3D location;
    
    BeaconArray beacons;
} Scanner;

TEMPLATE_VECTOR(Scanner) ScannerArray;

typedef struct scanner_connection {
    Scanner * parent;
    Scanner * child;
    Orientation orientation_change;
} ScannerConnection;

Beacon NewBeacon(size_t id, Int x, Int y, Int z);
void PrintBeacon(Beacon * b);

Vector3D NewVector3D(Int x, Int y, Int z);
Orientation ConstructOrientation(size_t permutation_id);

void PrintOrientation(Orientation * D);
void PrintVector3D(Vector3D v);

Vector3D mult(Orientation * D, Vector3D in);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
