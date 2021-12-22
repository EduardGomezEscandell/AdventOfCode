#include "day22.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <string.h>

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

TEMPLATE_VECTOR(Cube) CubesArray;

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

// Takes an array of cubes and splits them wih a specified plane
void Slice(CubesArray * cubes, Plane plane)
{
    CubesArray result;
    NEW_VECTOR(result);
    size_t size = SIZE(*cubes) * 2;
    RESERVE(result, size);

    for(Cube * it = cubes->begin; it != cubes->end; ++it)
    {
        if(it->min.data[plane.axis] > plane.value
           || it->max.data[plane.axis] < plane.value) 
        {
            PUSH(result, *it);
            continue;
        }

        Cube lower = *it;
        Cube upper = *it;

        upper.min.data[plane.axis] = plane.value;
        lower.max.data[plane.axis] = plane.value;

        PUSH(result, lower);
        PUSH(result, upper);
    }

    CLEAR(*cubes);
    *cubes = result;
}

void Decompose(Cube const * A, Plane planes[6])
{
    for(size_t i=0; i<3; ++i) // 6 faces, 2 per axis
    {
        planes[2*i].axis = i;
        planes[2*i].value = A->min.data[i];  // lower face
        
        planes[2*i+1].axis = i;
        planes[2*i+1].value = A->max.data[i];  // upper face
    }
}


int CompareVec3(Vector3d const * A, Vector3d const * B)
{
    for(size_t i=0; i<3;  ++i)
    {
        if(A->data[i] > B->data[i]) return  1;
        if(A->data[i] < B->data[i]) return -1;
    }
    return 0;
}

int CompareCubes(Cube const * A, Cube const * B)
{
    return CompareVec3(&A->min, &B->min);
}

DEFINE_QUICKSORT_COMP(SortCubes, Cube, CompareCubes)
DEFINE_FIND_COMP(FindCube, Cube, CompareCubes)

bool DoIntersect(Cube const * old, Cube const * new)
{
    for(size_t i=0; i<3; ++i)
    {
        if(old->min.data[i] > new->max.data[i]) return false;
        if(old->max.data[i] < new->min.data[i]) return false;
    }
    return true;
}


void Intersect(Cube const * old, Cube const * new, CubesArray * old_result, CubesArray * new_result)
{
    if(old->orientation == -1) {
        return;
    }

    if(!DoIntersect(old, new))
    { // No intersection -> cube is simply pushed to list
        PUSH(*old_result, *old);
        PUSH(*new_result, *new);
        return;
    }

    Plane old_decomposed[6];
    Plane new_decomposed[6];

    Decompose(old, old_decomposed);
    Decompose(new, new_decomposed);

    for(size_t i=0; i<6; ++i)
    {
        Slice(old_result, new_decomposed[i]);
        Slice(new_result, old_decomposed[i]);
    }

    // Finding intersection
    SortCubes(new_result->begin, new_result->end);

    for(Cube * it = old_result->begin; it != old_result->end; ++it)
    {
        Cube * res = FindCube(new_result->begin, new_result->end, it, true);

        // Overlapping: can be popped from new result
        if(res) {
            // If negative: must be popped from old result as well
            if(res->orientation == -1)
            {
                SWAP(Cube, it, old_result->end-1);
                POP(*old_result);
            } 

            SWAP(Cube, res, new_result->end-1);
            POP(*new_result);
        }
    }
}

solution_t Volume(Cube const * cube)
{
    solution_t vol = 1;

    for(size_t i=0; i<3; ++i) {
        vol *= cube->max.data[i] - cube->min.data[i];
    }

    return vol;
}

void PushCube(CubesArray * cubes/*, Cube const * new_cube*/)
{
    for(ssize_t i = SIZE(*cubes)-1; i >= 0; --i)
    {
        // TODO
        // Cube * it = cubes->begin + i;
        // Intersect(it, new_cube, cubes);

    }

}


solution_t SolvePart1(const bool is_test)
{
    CubesArray cubes = ReadCubes(is_test);

    for(Cube * it = cubes.begin; it != cubes.end; ++it)
    {

    }

    return is_test;
}

solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 590784)
DEFINE_TEST(2, 2758514936282235)
