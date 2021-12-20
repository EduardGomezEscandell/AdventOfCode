#include "day19.h"
#include "algebra.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/file_utils.h"
#include "common/matrix.h"
#include "common/vector.h"

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
    Orientation * D,
    BeaconPtr * sample_match_B)
{
    unsigned int n_valid_vectors = 0;
    *sample_match_B = NULL;

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
                *sample_match_B = target_B;
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
        unsigned int n_matches_global = 0;
        Beacon * matcher_B = NULL;

        for(Beacon * source_B = B->beacons.begin; source_B != B->beacons.end; ++source_B)
        {
            // Assume source_A and source_B are the same point seen form diferent coordinates
            // Can we find 11 other shared points?
            unsigned int n_matches = CountMatches(source_A, source_B, A->beacons, B->beacons, &D, &matcher_B);

            if(n_matches >= 11) {
                match[0] = source_A;
                match[1] = source_B;
                return true;
            }

            n_matches_global += n_matches;

            if(n_matches_global >= 11)
            {
                match[0] = source_A;
                match[1] = matcher_B;
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

        printf("--------------------------------\n");
        printf("Propagating %ld -> %ld with:\n", this->id, other->id);
        printf("  Master: "); PrintVector3D(x);
        printf("  Slave: "); PrintVector3D(y);
        printf("Transform1 Orientation: \n"); PrintOrientation(&this->orientation);
        printf("Transform2 Orientation: \n"); PrintOrientation(&T);
        printf("Combined orientation: \n"); PrintOrientation(&other->orientation);
        printf("Location: "); PrintVector3D(other->location);

        other->connected = true;
        TransformBeaconCoordinates(other);
        PropagateInformation(other);
    }

    fflush(stdout);
}



solution_t SolvePart1(const bool is_test)
{
    if(!is_test) return 0;

    ScannerArray scanners = ReadInput(is_test);

    ConnexionArray overlaps = FindOverlaps(scanners);

    scanners.begin->connected = true;
    scanners.begin->orientation = ConstructOrientation(0);
    scanners.begin->location = NewVector3D(0,0,0);

    PropagateInformation(scanners.begin);

    for(Scanner *s = scanners.begin; s != scanners.end; ++s)
        ClearScanner(s);
    CLEAR(scanners);
    CLEAR(overlaps);


    return is_test;
}

solution_t SolvePart2(const bool is_test)
{
    // FILE * file = GetFile(is_test, 18);

    return is_test;
}


DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
