#include "day19.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/file_utils.h"
#include "common/vector.h"

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

BeaconPtr NewBeaconPtr(Int x, Int y, Int z)
{
    BeaconPtr ptr;
    ptr.data = malloc(sizeof(*ptr.data));
    ptr.refcount = malloc(sizeof(*ptr.refcount));

    *ptr.data = NewVector3D(x,y,z);
    *ptr.refcount = 1;

    return ptr;
}

BeaconPtr CopyPtr(BeaconPtr ptr)
{
    printf("Copied beacon rc=%ld data=", *ptr.refcount);
    PrintBeacon(ptr);
    ++(*ptr.refcount);
    return ptr;
}

void DeletePtr(BeaconPtr ptr)
{
    --(*ptr.refcount);

    if(*ptr.refcount == 0)
    {
        free(ptr.data);
        free(ptr.refcount);
    }

    ptr.data = NULL;
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

Vector3D mult(Orientation * D, Vector3D in)
{
    Vector3D out;
    for(size_t i=0; i<DIM; ++i)
    {
        out.data[i] = D->values[i] * in.data[D->sparsity[i]];
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

void PrintBeacon(BeaconPtr b)
{
    PrintVector3D(*b.data);
}

void ClearScanner(Scanner * scanner)
{
    for(BeaconPtr * it=scanner->beacons.begin; it != scanner->beacons.end; ++it)
    {
        DeletePtr(*it);
    }
    CLEAR(scanner->beacons);
}

Scanner ReadScanner(FILE* file, bool * last_scanner)
{
    size_t len = 0;
    ssize_t status = 0;
    char * line = NULL;
    *last_scanner = false;
    
    Scanner s;
    if((status = getline(&line, &len, file)) == -1) {
        fprintf(stderr, "Wrong input file! (%s: %d)", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    s.id = (Int) atol(line + 12);
    NEW_VECTOR(s.beacons);

    printf("Scanner #%ld\n\n", s.id);

    if(s.id == 0) s.orientation = ConstructOrientation(0);

    while(true) {
        status = getline(&line, &len, file);

        if(status == -1) {
            free(line);
            *last_scanner = true;
            return s;
        }

        if(*line == '\n') break;

        const char * token; // points to the start of the next number

        token = line;
        const Int x = atoi(token);

        token = strchr(line, ',') + 1;        
        const Int y = atoi(token);

        token = strchr(token, ',') + 1;
        const Int z = atoi(token);

        PUSH(s.beacons, NewBeaconPtr(x,y,z));
    }

    free(line);
    return s;
}


ScannerArray ReadInput(const bool is_test)
{
    FILE * file = GetFile(is_test, 19);

    ScannerArray scanners;
    NEW_VECTOR(scanners);

    bool last_scanner = false;

    while(!last_scanner)
    {
        PUSH(scanners, ReadScanner(file, &last_scanner));
    }

    fclose(file);
    return scanners;
}

/**
 * Given a vector t, returns all ordered beacon pairs {U,V} such that
 *  V-U == t
 */
MatchArray CheckMatch(BeaconPtrArray beacons, Vector3D target)
{
    MatchArray matches;
    NEW_VECTOR(matches);

    for(const BeaconPtr * it1 = beacons.begin; it1 != beacons.end; ++it1)
    {
        for(const BeaconPtr * it2 = beacons.begin; it2 != beacons.end; ++it2)
        {
            if(it1 == it2) continue;

            Vector3D delta_X = sub(*it2->data, *it1->data);

            if(eq(delta_X, target))
            {
                Match m = {*it1, *it2};
                PUSH(matches, m);
            }
        }
    }

    return matches;
}


int CheckOrientation(Scanner * A, Scanner * B, size_t permutation_id)
{
    Orientation D = ConstructOrientation(permutation_id);

    for(const BeaconPtr * Y_i = B->beacons.begin; Y_i != B->beacons.end; ++Y_i)
    {
        size_t n_matches = 0;
        
        for(const BeaconPtr * Y_j = B->beacons.begin; Y_j != B->beacons.end; ++Y_j)
        {
            if(Y_i == Y_j) continue;

            Vector3D delta_Y = sub(*Y_j->data, *Y_i->data);
            Vector3D delta_X = mult(&D, delta_Y);

            MatchArray result = CheckMatch(A->beacons, delta_X);

            if(SIZE(result)) {
                n_matches +=  1;
            }

            CLEAR(result);

            if(n_matches >= 11) // 11 pairs -> 12 numbers!
            {
                return true;
            }
        }
    }

    return false;
}

/**
 * Returns the first orientation which works
 */
int CheckCompatibility(Scanner * A, Scanner * B)
{
    for(size_t i=0; i<24; ++i)
    {
        if(CheckOrientation(B, A, i))
            return i;
    }
    return -1;
}


solution_t SolvePart1(const bool is_test)
{
    ScannerArray scanners = ReadInput(is_test);

    for(Scanner * it1=scanners.begin; it1 != scanners.end; ++it1) {
        for(Scanner * it2=it1+1; it2 != scanners.end; ++it2)
        {
            int n = CheckCompatibility(it1, it2);
            if(n != -1)
                printf("Scanners #%ld and %ld are compatible (n=%d)!\n", it1->id, it2->id, n);
        }
    }

    for(Scanner *it=scanners.begin; it != scanners.end; ++it)
    {
        ClearScanner(it);
    }
    CLEAR(scanners);

    return is_test;
}

solution_t SolvePart2(const bool is_test)
{
    // FILE * file = GetFile(is_test, 18);

    return is_test;
}


DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
