#ifndef DAY_18_H
#define DAY_18_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "common/testing.h"
#include "common/vector.h"

#include "algebra.h"

typedef int solution_t; // Just in case we have to switch to long

typedef struct 
{
    size_t id;
    Vector3D data;
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

typedef struct scanner_connection {
    Scanner * parent;
    Scanner * child;
    Orientation orientation_change;
} ScannerConnection;

TEMPLATE_VECTOR(Scanner) ScannerArray;

Beacon NewBeacon(size_t id, Int x, Int y, Int z);
void PrintBeacon(Beacon * b);

// Solving
solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
