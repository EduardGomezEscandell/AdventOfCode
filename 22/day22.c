#include "day22.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

coord_t GetStartX(Cube const * it) { return it->min.coords.x;}
coord_t GetFinishX(Cube const * it) { return it->max.coords.x;}

coord_t GetStartY(Cube const * it) { return it->min.coords.y;}
coord_t GetFinishY(Cube const * it) { return it->max.coords.y;}

coord_t GetStartZ(Cube const * it) { return it->min.coords.z;}
coord_t GetFinishZ(Cube const * it) { return it->max.coords.z;}

DEFINE_QUICKSORT(QuickSort, coord_t)

CubesArray ReadCubes(bool is_test)
{
    FILE* file = GetFile(is_test, 22);

    size_t len = 0;
    char * line = NULL;

    CubesArray cubes;
    NEW_VECTOR(cubes);

    while(getline(&line, &len, file) != -1)
    {
        Cube cube;
        char * line_ptr = line;

        switch (line[1]) {
        case 'n': cube.orientation =  1;  break;
        case 'f': cube.orientation = -1; break;
        default:
            fprintf(stderr, "Wrong input file format!\n");
            exit(EXIT_FAILURE);
        }

        line_ptr = strchr(line_ptr, '=') + 1;
        cube.min.coords.x = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        cube.max.coords.x = atoi(line_ptr) + 1;


        line_ptr = strchr(line_ptr, '=') + 1;
        cube.min.coords.y = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        cube.max.coords.y = atoi(line_ptr) + 1;


        line_ptr = strchr(line_ptr, '=') + 1;
        cube.min.coords.z = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        cube.max.coords.z = atoi(line_ptr) + 1;

        PUSH(cubes, cube);
    }

    free(line);
    fclose(file);

    return cubes;
}

void SortAndRemoveDuplicates(CoordinateVector * vec)
{
    QuickSort(vec->begin, vec->end);

    for(coord_t *it = vec->end-1; it != vec->begin; --it)
    {
        coord_t *next = it-1;

        if(*it == *next)
        {
            REMOVE(*vec, coord_t, it);
        }
    }
}

void EnforceLimits(
    CoordinateVector * vec,
    coord_t const * limits_low,
    coord_t const * limits_high)
{
    if(!limits_low && !limits_low) return;

    for(coord_t *it = vec->end-1; it >= vec->begin; --it)
    {
        if((limits_low && (*it < *limits_low)) || (limits_high && (*it > *limits_high)))
        {
            REMOVE(*vec, coord_t, it);
        }
    }
}


void GenerateGrid(
    CubesArray const * cubes,
    CoordinateVector * X,
    CoordinateVector * Y,
    CoordinateVector * Z,
    coord_t const * limits_low,
    coord_t const * limits_high)
{
    size_t n_cubes = SIZE(*cubes);

    NEW_VECTOR(*X); RESERVE(*X, n_cubes);
    NEW_VECTOR(*Y); RESERVE(*Y, n_cubes);
    NEW_VECTOR(*Z); RESERVE(*Z, n_cubes);

    for(Cube * it = cubes->begin; it != cubes->end; ++it)
    {
        PUSH(*X, it->min.coords.x);  PUSH(*X, it->max.coords.x);
        PUSH(*Y, it->min.coords.y);  PUSH(*Y, it->max.coords.y);
        PUSH(*Z, it->min.coords.z);  PUSH(*Z, it->max.coords.z);
    }

    if(limits_low)
    {
        PUSH(*X, *limits_low);
        PUSH(*Y, *limits_low);
        PUSH(*Z, *limits_low);
    }

    if(limits_high)
    {
        PUSH(*X, *limits_high);
        PUSH(*Y, *limits_high);
        PUSH(*Z, *limits_high);
    }

    SortAndRemoveDuplicates(X);
    SortAndRemoveDuplicates(Y);
    SortAndRemoveDuplicates(Z);

    EnforceLimits(X, limits_low, limits_high);
    EnforceLimits(Y, limits_low, limits_high);
    EnforceLimits(Z, limits_low, limits_high);
}

CubePtrArray CreatePtrVector(CubesArray * cubes)
{
    CubePtrArray out;
    NEW_VECTOR(out);
    RESERVE(out, SIZE(*cubes));

    for(Cube * it = cubes->begin; it != cubes->end; ++it)
    {
        PUSH(out, it);
    }

    return out;
}

bool CopyCubesWithCoordinate(
    CubePtrArray * out,
    CubePtrArray const * in,
    coord_t coord,
    Getter Start,
    Getter Finish)
{
    out->end = out->begin; // Emptying without releasing memory

    bool any_write = false;

    for(Cube ** it = in->begin; it != in->end; ++it)
    {
        if(Start(*it) > coord || coord >= Finish(*it)) continue;

        PUSH(*out, *it);

        if((*it)->orientation == 1) any_write = true;
    }

    return any_write;
}

bool VoxelStatus(
    CubePtrArray const * cubes,
    coord_t coord,
    Getter Start,
    Getter Finish)
{
    for(Cube * const * it = cubes->end-1; it >= cubes->begin; --it)
    {
        if(Start(*it) <= coord && coord < Finish(*it))
            return (*it)->orientation == 1;
    }

    return false;
}

solution_t Volume(
    CoordinateVector * X,
    CoordinateVector * Y,
    CoordinateVector * Z,
    size_t i,
    size_t j,
    size_t k)
{
    return (X->begin[i+1] - X->begin[i]) * (Y->begin[j+1] - Y->begin[j]) * (Z->begin[k+1] - Z->begin[k]);
}

solution_t Solve(
    const bool is_test,
    coord_t const * limits_low,
    coord_t const * limits_high)
{
    CubesArray cubes = ReadCubes(is_test);

    CoordinateVector X, Y, Z;
    GenerateGrid(&cubes, &X, &Y, &Z, limits_low, limits_high);

    // Vector of pointers to the cubes (should be const but then I can't free it :S)
    CubePtrArray unfiltered = CreatePtrVector(&cubes);

    // Vector of pointers to cubes that contain a certain x (updated during loop)
    CubePtrArray filtered_x;
    NEW_VECTOR(filtered_x);
    RESERVE(filtered_x, SIZE(cubes));

    // Vector of pointers to cubes that intersect a certain xy plane (updated during loop)
    CubePtrArray filtered_xy;
    NEW_VECTOR(filtered_xy);
    RESERVE(filtered_xy, SIZE(cubes));

    solution_t active_voxel_count = 0; // Result

    for(size_t i=0; i<SIZE(X); ++i)
    {
        coord_t x = X.begin[i];
        solution_t delta_x = X.begin[i+1] - X.begin[i];
        CopyCubesWithCoordinate(&filtered_x, &unfiltered, x, GetStartX, GetFinishX);

        if(SIZE(filtered_x) == 0) continue; // No active cubes for this x

        for(size_t j=0; j<SIZE(Y); ++j)
        {
            coord_t y = Y.begin[j];
            solution_t area = delta_x * (Y.begin[j+1] - Y.begin[j]);

            CopyCubesWithCoordinate(&filtered_xy, &filtered_x, y, GetStartY, GetFinishY);

            if(SIZE(filtered_xy) == 0) continue; // No active cubes for this xy plane

            for(size_t k=0; k<SIZE(Z); ++k)
            {
                coord_t z = Z.begin[k];
                solution_t volume = area * (Z.begin[k+1] - Z.begin[k]);

                solution_t status = volume * VoxelStatus(&filtered_xy, z, GetStartZ, GetFinishZ);
                active_voxel_count += status;
            }
        }
    }

    CLEAR(cubes);
    CLEAR(X);          CLEAR(Y);          CLEAR(Z);
    CLEAR(unfiltered); CLEAR(filtered_x); CLEAR(filtered_xy);

    return active_voxel_count;
}


solution_t SolvePart1(const bool is_test)
{
    coord_t limits_lo = -50;
    coord_t limits_hi =  51;
    return Solve(is_test, &limits_lo, &limits_hi);
}


solution_t SolvePart2(const bool is_test)
{
    return Solve(is_test, NULL, NULL);
}

DEFINE_TEST(1, 474140)
DEFINE_TEST(2, 2758514936282235)
