#include "day19.h"
#include "algebra.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/file_utils.h"
#include "common/hash_table.h"
#include "common/matrix.h"
#include "common/testing.h"
#include "common/vector.h"

HT_DEFINE_SET_COMPARISON   (size_t, Vector3D, int, Vector3DSet)
HT_DEFINE_NEW_AND_CLEAR    (size_t, Vector3D, int, Vector3DSet)
HT_DEFINE_FIND             (size_t, Vector3D, int, Vector3DSet)
HT_DEFINE_FIND_OR_EMPLACE  (size_t, Vector3D, int, Vector3DSet)
HT_DEFINE_RESERVE          (size_t, Vector3D, int, Vector3DSet)
// HT_DEFINE_REMOVE           (size_t, Vector3D, int, Vector3DSet)
// HT_DEFINE_PRINT            (size_t, Vector3D, int, Vector3DSet)



Beacon NewBeacon(size_t id, Int x, Int y, Int z)
{
    Beacon b;
 
    b.loc = NewVector3D(x,y,z);
    b.id = id;

    return b;
}

void PrintBeacon(Beacon * b)
{
    PrintVector3D(b->loc);
}

void ClearScanner(Scanner * scanner)
{
    CLEAR(scanner->beacons);
    CLEAR(scanner->connections);
}

Scanner ReadScanner(FILE* file, bool * last_scanner, size_t * line_count)
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
    ++(*line_count);

    s.id = (Int) atol(line + 12);

    s.connected = false;
    NEW_VECTOR(s.beacons);
    NEW_VECTOR(s.connections);
    
    while(true) {
        status = getline(&line, &len, file);
        ++(*line_count);

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

        PUSH(s.beacons, NewBeacon(*line_count, x,y,z));
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

    size_t line_count = 0;
    while(!last_scanner)
    {
        PUSH(scanners, ReadScanner(file, &last_scanner, &line_count));
    }

    fclose(file);
    return scanners;
}

typedef Beacon * BeaconPtr;

/**
 * D is the matrix to convert:
 *    X = D*Y
 * - X are the coordinate in A's basis
 * - Y are the coordinate in B's basis
 */
unsigned int CountMatches(
    Beacon const * source_A,
    Beacon const * source_B,
    BeaconArray targets_A,
    BeaconArray targets_B,
    Orientation * D)
{
    unsigned int n_valid_vectors = 0;

    for(Beacon * target_B=targets_B.begin; target_B != targets_B.end; ++target_B)
    {
        if(target_B == source_B) continue; // Skipping zero vector

        const Vector3D delta_y = VectorFromSourceAndDestination(source_B->loc, target_B->loc);
        const Vector3D searched_delta_x = vecmult(D, delta_y);

        for(Beacon const * target_A=targets_A.begin; target_A != targets_A.end; ++target_A)
        {
            if(target_A == source_A) continue; // Skipping zero vector

            const Vector3D delta_x = VectorFromSourceAndDestination(source_A->loc, target_A->loc);

            if(eq(searched_delta_x, delta_x))
            {
                n_valid_vectors += 1;
            }
        }
    }

    return n_valid_vectors;
}


bool ValidOrientation(Scanner * A, Scanner * B, size_t permutation_id, BeaconPtr match[2])
{
    Orientation D = ConstructOrientation(permutation_id);

    for(Beacon * source_A = A->beacons.begin; source_A != A->beacons.end; ++source_A)
    {
        for(Beacon * source_B = B->beacons.begin; source_B != B->beacons.end; ++source_B)
        {
            // Assume source_A and source_B are the same point seen form diferent coordinates
            // Can we find 11 other shared points?
            unsigned int n_matches = CountMatches(source_A, source_B, A->beacons, B->beacons, &D);

            if(n_matches >= 11) {
                match[0] = source_A;
                match[1] = source_B;
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
        BeaconPtr match[2];
        if(ValidOrientation(A, B, orientation, match))
        {
            Orientation D = ConstructOrientation(orientation);
            ScannerConnection c = { A, B, D, match[0], match[1] };

            PUSH(*overlaps, c);

            PUSH(A->connections, overlaps->end[-1]);
            PUSH(B->connections, overlaps->end[-1]);

            return true;
        }
    }
    return false;
}

ConnexionArray FindOverlaps(ScannerArray scanners)
{
    ConnexionArray overlaps;
    NEW_VECTOR(overlaps);

    for(Scanner * it1=scanners.begin; it1 != scanners.end; ++it1) {
        for(Scanner * it2=it1+1; it2 != scanners.end; ++it2)
        {
            if(CheckOverlap(it1, it2, &overlaps))
            {
                printf("Scanners #%ld and %ld are compatible\n", it1->id, it2->id);
                fflush(stdout);
            }
        }
    }

    return overlaps;
}

void TransformBeaconCoordinates(Scanner * scanner)
{
    for(Beacon * it=scanner->beacons.begin; it!=scanner->beacons.end; ++it)
    {
        it->loc = vecmult(&scanner->orientation, it->loc);
        accumulate(&it->loc, scanner->location);
    }
}


void PropagateInformation(Scanner * this)
{
    for(ScannerConnection * it = this->connections.begin; it != this->connections.end; ++it)
    {
        bool this_is_ruler = it->ruler == this;
        
        Scanner * other = this_is_ruler ? it->dependent : it->ruler;

        if(other->connected) continue;

        // Obtaining orientation relative to scanner #0
        Orientation T;
        if(this_is_ruler)
        {
            T = inv(&it->orientation_change);
        }
        else
        {
            T = it->orientation_change;
        }

        other->orientation = matmul(&this->orientation, &T);

        // Obtaining location relative to scanner #0
        Vector3D x = this_is_ruler ? it->beacon_ruler->loc     : it->beacon_dependent->loc;
        Vector3D y = this_is_ruler ? it->beacon_dependent->loc : it->beacon_ruler->loc;

        Vector3D T_y = vecmult(&other->orientation, y);
        other->location = VectorFromSourceAndDestination(T_y, x);

        other->connected = true;
        TransformBeaconCoordinates(other);
        PropagateInformation(other);
    }

    fflush(stdout);
}

int compare(Vector3D const * u, Vector3D const * v)
{
    return eq(*u, *v) ? 0 : 1;
}

Vector3DSet CreateVector3DSet(ScannerArray scanners)
{
    Vector3DSet unique_beacons = NewVector3DSet(hash_vector3d);
    Vector3DSetSetComparison(&unique_beacons, compare);

    size_t needed_capacity = 0;
    for(Scanner const * it = scanners.begin; it!=scanners.end; ++it) {
        needed_capacity += SIZE(it->beacons);
    }

    Vector3DSetReserve(&unique_beacons, needed_capacity);

    for(Scanner const * s = scanners.begin; s!=scanners.end; ++s)
    {
        for(Beacon const * it = s->beacons.begin; it != s->beacons.end; ++it)
        {
            *Vector3DSetFindOrEmplace(&unique_beacons, it->loc, 0) += 1;
        }
    }

    return unique_beacons;
}

long Manhattan(Vector3D * u, Vector3D * v)
{
    long r = 0;
    for(size_t i=0; i!=DIM; ++i)
        r += abs(u->data[i] - v->data[i]);
    return r;
}


long FindGreatestManhatanDistance(ScannerArray scanners)
{
    long max_distance = 0;
    for(Scanner *it1 = scanners.begin; it1 != scanners.end; ++it1) {
        for(Scanner *it2 = it1 + 1; it2 != scanners.end; ++it2)
        {
            long distance = Manhattan(&it1->location, &it2->location);

            max_distance = MAX(max_distance, distance);
        }
    }
    return max_distance;
}


solution_t Solve(const bool is_test)
{
    solution_t solution = {0,0};
    
    if(!is_test) return solution;
    
    // Reading
    ScannerArray scanners = ReadInput(is_test);

    // Creating map
    ConnexionArray overlaps = FindOverlaps(scanners);
    scanners.begin->connected = true;
    scanners.begin->orientation = ConstructOrientation(0);
    scanners.begin->location = NewVector3D(0,0,0);
    PropagateInformation(scanners.begin);
    
    // Solving
    Vector3DSet unique_beacons = CreateVector3DSet(scanners);
    solution.part_1 = SIZE(unique_beacons.data);
    solution.part_2 = FindGreatestManhatanDistance(scanners);

    // Cleaning
    for(Scanner *s = scanners.begin; s != scanners.end; ++s)
        ClearScanner(s);
    ClearVector3DSet(&unique_beacons);
    CLEAR(scanners);
    CLEAR(overlaps);

    return solution;
}


day19_test_result Test()
{
    day19_test_result t;
    
    t.expected.part_1 = 79;
    t.expected.part_2 = 3621;

    t.obtained = Solve(true);
    
    t.success[0] = t.expected.part_1 == t.obtained.part_1;
    t.success[1] = t.expected.part_2 == t.obtained.part_2;
    
    return t;
}