#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"
#include "common/vector.h"
#include "common/hash_table.h"

#include "algebra.h"

typedef struct 
{
    size_t id; // helpful for debugging
    Vector3D loc;
} Beacon;

struct scanner_connection;

TEMPLATE_VECTOR(Vector3D) PointsArray;
TEMPLATE_VECTOR(Beacon) BeaconArray;
TEMPLATE_VECTOR(struct scanner_connection) ConnexionArray;
TEMPLATE_HASH_TABLE(size_t, Vector3D, int, Vector3DSet);

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

typedef struct scanner_connection
{
    Scanner * ruler;
    Scanner * dependent;
    
    Orientation orientation_change;
    
    // Same beacon seen from both perspectives:
    Beacon * beacon_ruler;
    Beacon * beacon_dependent;
} ScannerConnection;


Beacon NewBeacon(size_t id, Int x, Int y, Int z);
void PrintBeacon(Beacon * b);
void ClearScanner(Scanner * scanner);

Scanner ReadScanner(FILE* file, bool * last_scanner, size_t * line_count);
ScannerArray ReadInput(const bool is_test);

typedef Beacon * BeaconPtr;

unsigned int CountMatches(
    Beacon const * source_A,
    Beacon const * source_B,
    BeaconArray targets_A,
    BeaconArray targets_B,
    Orientation * D);
bool ValidOrientation(Scanner * A, Scanner * B, size_t orientation_id, BeaconPtr match[2]);
bool CheckOverlap(Scanner * A, Scanner * B, ConnexionArray * overlaps);
ConnexionArray FindOverlaps(ScannerArray scanners);

void TransformBeaconCoordinates(Scanner * scanner);
void PropagateInformation(Scanner * this);

int compare(Vector3D const * u, Vector3D const * v);
Vector3DSet CreateVector3DSet(ScannerArray scanners);


long Manhattan(Vector3D * u, Vector3D * v);
long FindGreatestManhatanDistance(ScannerArray scanners);

// Solving
typedef struct {
    size_t part_1;
    long part_2;
} solution_t;


solution_t Solve(const bool is_test);

typedef struct {
    solution_t expected;
    solution_t obtained;
    bool success[2];
} day19_test_result;

day19_test_result Test();

#endif
