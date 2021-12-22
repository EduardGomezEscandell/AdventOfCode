#include "day22.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

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


Grid GenerateGrid(
    CubesArray const * cubes,
    coord_t const * limits_low,
    coord_t const * limits_high)
{
    size_t n_cubes = SIZE(*cubes);

    Grid grid;

    NEW_VECTOR(grid.X); RESERVE(grid.X, n_cubes);
    NEW_VECTOR(grid.Y); RESERVE(grid.Y, n_cubes);
    NEW_VECTOR(grid.Z); RESERVE(grid.Z, n_cubes);

    for(Cube * it = cubes->begin; it != cubes->end; ++it)
    {
        PUSH(grid.X, it->min.coords.x);  PUSH(grid.X, it->max.coords.x);
        PUSH(grid.Y, it->min.coords.y);  PUSH(grid.Y, it->max.coords.y);
        PUSH(grid.Z, it->min.coords.z);  PUSH(grid.Z, it->max.coords.z);
    }

    if(limits_low)
    {
        PUSH(grid.X, *limits_low);
        PUSH(grid.Y, *limits_low);
        PUSH(grid.Z, *limits_low);
    }

    if(limits_high)
    {
        PUSH(grid.X, *limits_high);
        PUSH(grid.Y, *limits_high);
        PUSH(grid.Z, *limits_high);
    }

    SortAndRemoveDuplicates(&grid.X);
    SortAndRemoveDuplicates(&grid.Y);
    SortAndRemoveDuplicates(&grid.Z);

    EnforceLimits(&grid.X, limits_low, limits_high);
    EnforceLimits(&grid.Y, limits_low, limits_high);
    EnforceLimits(&grid.Z, limits_low, limits_high);

    return grid;
}

void ClearGrid(Grid * grid)
{
    CLEAR(grid->X);
    CLEAR(grid->Y);
    CLEAR(grid->Z);
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
    Grid * grid,
    size_t i,
    size_t j,
    size_t k)
{
    return (grid->X.begin[i+1] - grid->X.begin[i])
         * (grid->Y.begin[j+1] - grid->Y.begin[j]) 
         * (grid->Z.begin[k+1] - grid->Z.begin[k]);
}

void * SolveSingleThread(void * parameters)
{
    Grid const * grid          = ((ParameterPack *) parameters)->grid;
    const size_t begin         = ((ParameterPack *) parameters)->begin;
    const size_t end           = ((ParameterPack *) parameters)->end;
    CubePtrArray const * cubes = ((ParameterPack *) parameters)->cubes;
    solution_t * active_voxel_count = ((ParameterPack *) parameters)->active_voxel_count;

    // Vector of pointers to cubes that contain a certain x (updated during loop)
    CubePtrArray filtered_x;
    NEW_VECTOR(filtered_x);
    RESERVE(filtered_x, SIZE(*cubes));

    // Vector of pointers to cubes that intersect a certain xy plane (updated during loop)
    CubePtrArray filtered_xy;
    NEW_VECTOR(filtered_xy);
    RESERVE(filtered_xy, SIZE(*cubes));

    *active_voxel_count = 0;

    for(size_t i=begin; i<end; ++i)
    {

        coord_t x = grid->X.begin[i];
        solution_t delta_x = grid->X.begin[i+1] - grid->X.begin[i];
        CopyCubesWithCoordinate(&filtered_x, cubes, x, GetStartX, GetFinishX);

        if(SIZE(filtered_x) == 0) 
        {
            CLEAR(filtered_x);
            CLEAR(filtered_xy);
            return NULL; // No active cubes for this x
        }

        for(size_t j=0; j<SIZE(grid->Y)-1; ++j)
        {
            coord_t y = grid->Y.begin[j];
            solution_t area = delta_x * (grid->Y.begin[j+1] - grid->Y.begin[j]);

            CopyCubesWithCoordinate(&filtered_xy, &filtered_x, y, GetStartY, GetFinishY);

            if(SIZE(filtered_xy) == 0) continue; // No active cubes for this xy plane

            for(size_t k=0; k<SIZE(grid->Z)-1; ++k)
            {
                coord_t z = grid->Z.begin[k];
                solution_t volume = area * (grid->Z.begin[k+1] - grid->Z.begin[k]);

                solution_t status = volume * VoxelStatus(&filtered_xy, z, GetStartZ, GetFinishZ);
                *active_voxel_count += status;
            }
        }
    }

    CLEAR(filtered_x);
    CLEAR(filtered_xy);

    return NULL;
}

solution_t Solve(
    const bool is_test,
    coord_t const * limits_low,
    coord_t const * limits_high,
    const size_t nthreads)
{

    CubesArray cubes = ReadCubes(is_test);

    Grid grid = GenerateGrid(&cubes, limits_low, limits_high);

    // Vector of pointers to the cubes (should be const but then I can't free it :S)
    CubePtrArray cubes_ptr = CreatePtrVector(&cubes);

    solution_t active_voxel_count = 0; // Result

    const size_t n_xvals = SIZE(grid.X)-1;
    
    // Solutions
    SolutionArray voxel_counts;
    NEW_VECTOR(voxel_counts);
    RESERVE(voxel_counts, nthreads);
    voxel_counts.end += nthreads;

    // Parameters
    ParameterPackArray parameters;
    NEW_VECTOR(parameters);
    RESERVE(parameters, nthreads);
    parameters.end += nthreads;

    // ids
    ThreadArray thread_ids;
    NEW_VECTOR(thread_ids);
    RESERVE(thread_ids, nthreads);
    thread_ids.end += nthreads;

    // boundaries
    size_t begin = 0;
    size_t chunk_size = n_xvals/nthreads + 1;
    size_t end = chunk_size;

    for(size_t t=0; t < nthreads; ++t)
    {
        end = MIN(end, n_xvals);

        ParameterPack p = {&grid, &cubes_ptr, begin, end, voxel_counts.begin + t};
        parameters.begin[t] = p;
        pthread_create(thread_ids.begin + t, NULL, SolveSingleThread, parameters.begin + t);

        begin = end;
        end += chunk_size;
    }

    // Reduction
    for(size_t t=0; t < nthreads; ++t)
    {
        pthread_join(thread_ids.begin[t], NULL);
        active_voxel_count += voxel_counts.begin[t];
    }

    ClearGrid(&grid);
    CLEAR(cubes);
    CLEAR(cubes_ptr);

    return active_voxel_count;
}

size_t RetriveNumThreads()
{
    size_t nthreads = 4; // default value

    char buffer[256];
    char * envvar = "AOC_NUM_THREADS";

    if(!getenv(envvar)) return nthreads;

    if(snprintf(buffer, 256, "%s", getenv(envvar)) >= 256) return nthreads;

    nthreads = atoi(buffer);

    return nthreads;
}


solution_t SolvePart1(const bool is_test)
{
    const size_t nthreads = RetriveNumThreads();
    printf("Solving with %ld threads\n", nthreads);

    coord_t limits_lo = -50;
    coord_t limits_hi =  51;
    return Solve(is_test, &limits_lo, &limits_hi, nthreads);
}


solution_t SolvePart2(const bool is_test)
{
    const size_t nthreads = RetriveNumThreads();
    printf("Solving with %ld threads\n", nthreads);

    return Solve(is_test, NULL, NULL, nthreads);
}

DEFINE_TEST(1, 474140)
DEFINE_TEST(2, 2758514936282235)
