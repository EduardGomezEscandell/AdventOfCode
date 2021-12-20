#include "day19.h"

#include <stddef.h>
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

BeaconPtr NewBeaconPtr(size_t id, Int x, Int y, Int z)
{
    BeaconPtr ptr;
    ptr.data = malloc(sizeof(*ptr.data));
    ptr.id = malloc(sizeof(*ptr.id));
    ptr.refcount = malloc(sizeof(*ptr.refcount));

    *ptr.data = NewVector3D(x,y,z);
    *ptr.refcount = 1;
    *ptr.id = id;

    return ptr;
}

BeaconPtr NullPtr()
{
    BeaconPtr ptr;
    ptr.data = NULL;
    ptr.refcount = NULL;

    return ptr;
}

BeaconPtr CopyPtr(BeaconPtr ptr)
{
    ++(*ptr.refcount);
    return ptr;
}

void DeletePtr(BeaconPtr ptr)
{
    if(ptr.refcount != NULL) // Ensuring it has not been freed already is NULLPTR
        --(*ptr.refcount);

    if(ptr.refcount && *ptr.refcount == 0)
    {
        free(ptr.id);
        free(ptr.data);
        free(ptr.refcount);
    }

    ptr.id = NULL;
    ptr.data = NULL;
    ptr.refcount = NULL;
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

Scanner ReadScanner(FILE* file, bool * last_scanner, size_t * n_beacons)
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

    s.connected = false;
    NEW_VECTOR(s.beacons);
    NEW_VECTOR(s.connections);
    
    printf("Scanner #%ld\n\n", s.id);

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

        PUSH(s.beacons, NewBeaconPtr(*n_beacons, x,y,z));
        ++(*n_beacons);
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
    size_t n_beacons = 0;

    while(!last_scanner)
    {
        PUSH(scanners, ReadScanner(file, &last_scanner, &n_beacons));
    }

    fclose(file);
    return scanners;
}

/**
 * Given a vector t, returns the first ordered beacon pair {U,V} such that
 *  V-U == t
 *
 * Validated assumption: Only one match is ever found
 */
bool CheckMatch(BeaconPtrArray beacons, Vector3D target)
{
    for(const BeaconPtr * it1 = beacons.begin; it1 != beacons.end; ++it1)
    {
        for(const BeaconPtr * it2 = beacons.begin; it2 != beacons.end; ++it2)
        {
            if(it1 == it2) continue;

            Vector3D delta_X = sub(*it2->data, *it1->data);

            if(eq(delta_X, target))
            {
                return true;
            }
        }
    }
    return false;
}

bool ValidOrientation(Scanner * A, Scanner * B, size_t permutation_id)
{
    Orientation D = ConstructOrientation(permutation_id);

    for(const BeaconPtr * source = B->beacons.begin; source != B->beacons.end; ++source)
    {
        size_t n_matches = 0;

        for(const BeaconPtr * destination = B->beacons.begin; destination != B->beacons.end; ++destination)
        {
            if(source == destination) continue;

            Vector3D delta_Y = sub(*destination->data, *source->data);
            Vector3D delta_X = vecmult(&D, delta_Y);


            if(!CheckMatch(A->beacons, delta_X)) continue;
            
            n_matches += 1;

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
bool CheckOverlap(Scanner * A, Scanner * B, ConnexionArray * overlaps)
{
    for(size_t orientation=0; orientation<24; ++orientation)
    {
        if(ValidOrientation(B, A, orientation))
        {
            Orientation D = ConstructOrientation(orientation);
            ScannerConnection c = { A, B, D };

            PUSH(*overlaps, c);

            PUSH(A->connections, overlaps->end[-1]);
            PUSH(B->connections, overlaps->end[-1]);

            return true;
        }
    }
    return false;
}

void PropagateOrientation(Scanner * this)
{
    for(ScannerConnection * it = this->connections.begin; it != this->connections.end; ++it)
    {

        bool this_is_parent = it->parent == this;
        Scanner * other = this_is_parent ? it->child : it->parent;

        if(other->connected) continue;
        printf("Propagating %ld -> %ld\n", this->id, other->id);
        
        Orientation T;
        if(this_is_parent)
        {
            T = it->orientation_change;
        }
        else
        {
            T = inv(&it->orientation_change);
        }

        other->orientation = matmul(&this->orientation, &T);
        other->connected = true;

        PropagateOrientation(other);
    }
}


solution_t SolvePart1(const bool is_test)
{
    ScannerArray scanners = ReadInput(is_test);

    ConnexionArray overlaps;
    NEW_VECTOR(overlaps);

    for(Scanner * it1=scanners.begin; it1 != scanners.end; ++it1) {
        for(Scanner * it2=it1+1; it2 != scanners.end; ++it2)
        {
            if(CheckOverlap(it1, it2, &overlaps))
            {
                printf("Scanners #%ld and %ld are compatible\n", it1->id, it2->id);
            }
        }
    }

    scanners.begin->connected = true;
    scanners.begin->orientation = ConstructOrientation(1);

    PropagateOrientation(scanners.begin);

    BeaconPtrArray all_beacons;
    NEW_VECTOR(all_beacons);

    for(Scanner *it=scanners.begin; it != scanners.end; ++it)
    {
        
        for(BeaconPtr * ptr = it->beacons.begin; ptr != it->beacons.end; ++it)
        {
            PUSH(all_beacons, CopyPtr(*ptr));

            ptr->data = vecmult(it->orientation, *ptr);
        }

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
