#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"
#include "common/vector.h"
#include "common/hash_table.h"

#include "algebra.h"

typedef int solution_t; // Just in case we have to switch to long

typedef struct 
{
    size_t id; // helpful for debugging
    Vector3D loc;
} Beacon;

TEMPLATE_VECTOR(Vector3D) PointsArray;
TEMPLATE_VECTOR(Beacon) BeaconArray;

struct scanner_connection;
TEMPLATE_VECTOR(struct scanner_connection) ConnexionArray;

typedef struct scanner
{
    size_t id; // Probably won't use it
    
    ConnexionArray connections;
    bool connected;

    Orientation orientation;
    Vector3D location;
    
    BeaconArray beacons;
} Scanner;

typedef struct scanner_connection
{
    Scanner * ruler;
    Scanner * dependent;
    
    Orientation orientation_change;
    
    // Same beacon seen from both perspectives:
    Beacon * beacon_ruler;
    Beacon * beacon_dependent;
} ScannerConnection;

TEMPLATE_VECTOR(Scanner) ScannerArray;

TEMPLATE_HASH_TABLE(size_t, Vector3D, int, Vector3DSet);

Beacon NewBeacon(size_t id, Int x, Int y, Int z);
void PrintBeacon(Beacon * b);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
