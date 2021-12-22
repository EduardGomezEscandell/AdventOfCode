#ifndef DAY_21_H
#define DAY_21_H

#include "common/testing.h"
#include "common/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Note:
 * Unlike in the problem statement, all ranges use the follwing notation:
 *
 *                 [start, finish)
 *
 * hence the right side is NOT included
 */

typedef long long int solution_t; // Just in case we have to switch to long
typedef long int coord_t;

typedef union {
    coord_t data[3];
    struct {
        coord_t x,y,z;
    } coords;
} Vector3d;

typedef struct {
    int axis;    // Axis where it cuts (0,1,2 -> x,y,z)
    coord_t value; // Coordinate at the cutting axis
} Plane;

typedef struct {
    Vector3d min;
    Vector3d max;
    int orientation;
} Cube;

typedef coord_t (*Getter)(Cube const * it); // Function type to use same function for X, Y, Z axes

coord_t GetStartX(Cube const * it);
coord_t GetFinishX(Cube const * it);

coord_t GetStartY(Cube const * it);
coord_t GetFinishY(Cube const * it);

coord_t GetStartZ(Cube const * it);
coord_t GetFinishZ(Cube const * it);

TEMPLATE_VECTOR(coord_t) CoordinateVector;
TEMPLATE_VECTOR(Cube) CubesArray;
TEMPLATE_VECTOR(Cube*) CubePtrArray;

DECLARE_QUICKSORT(QuickSort, coord_t);

CubesArray ReadCubes(bool is_test);

void SortAndRemoveDuplicates(CoordinateVector * vec);

void EnforceLimits(
    CoordinateVector * vec,
    coord_t const * limits_low,
    coord_t const * limits_high);

void GenerateGrid(
    CubesArray const * cubes,
    CoordinateVector * X,
    CoordinateVector * Y,
    CoordinateVector * Z,
    coord_t const * limits_low,
    coord_t const * limits_high);

CubePtrArray CreatePtrArray(CubesArray * cubes);

bool CopyCubesWithCoordinate(
    CubePtrArray * out,
    CubePtrArray const * in,
    coord_t coord,
    Getter Start,
    Getter Finish);

bool VoxelStatus(
    CubePtrArray const * cubes,
    coord_t coord,
    Getter Start,
    Getter Finish);

solution_t Volume(
    CoordinateVector * X,
    CoordinateVector * Y,
    CoordinateVector * Z,
    size_t i,
    size_t j,
    size_t k);

// Solving

solution_t Solve(
    const bool is_test,
    coord_t const * limits_low,
    coord_t const * limits_high);


solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
