#include "day19.h"
#include "algebra.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/file_utils.h"
#include "common/vector.h"

Beacon NewBeacon(size_t id, Int x, Int y, Int z)
{
    Beacon b;
 
    b.data = NewVector3D(x,y,z);
    b.id = id;

    return b;
}

void PrintBeacon(Beacon * b)
{
    PrintVector3D(b->data);
}

void ClearScanner(Scanner * scanner)
{
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

        PUSH(s.beacons, NewBeacon(*n_beacons, x,y,z));
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
bool CheckMatch(BeaconArray beacons, Vector3D target)
{
    for(const Beacon * it1 = beacons.begin; it1 != beacons.end; ++it1)
    {
        for(const Beacon * it2 = beacons.begin; it2 != beacons.end; ++it2)
        {
            if(it1 == it2) continue;

            Vector3D delta_X = sub(it2->data, it1->data);

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

    for(const Beacon * source = B->beacons.begin; source != B->beacons.end; ++source)
    {
        size_t n_matches = 0;

        for(const Beacon * destination = B->beacons.begin; destination != B->beacons.end; ++destination)
        {
            if(source == destination) continue;

            Vector3D delta_Y = sub(destination->data, source->data);
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

solution_t SolvePart1(const bool is_test)
{
    ScannerArray scanners = ReadInput(is_test);

    ConnexionArray overlaps = FindOverlaps(scanners);

    scanners.begin->connected = true;
    scanners.begin->orientation = ConstructOrientation(1);

    PropagateOrientation(scanners.begin);

    BeaconArray all_beacons;
    NEW_VECTOR(all_beacons);

    for(Scanner *s = scanners.begin; s != scanners.end; ++s)
    {
        for(Beacon * it = s->beacons.begin; it != s->beacons.end; ++s)
        {
            PUSH(all_beacons, *it);

            it->data = vecmult(&s->orientation, it->data);
        }

    }

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
