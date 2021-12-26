#include "routing.h"
#include "gamestate.h"

#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>

/** IDs of nodes
 * 
 * 0---1---+---2---+---3---+---4---+---5---6
 *         |       |       |       |
 *         7       8       9       10
 *         |       |       |       |
 *         11      12      13      14
 *         |       |       |       |      ]
 *         15      16      17      18     ]  These nodes disabled
 *         |       |       |       |      ]  for problem part 1
 *         19      20      21      22     ]
 * 
 * 31 used as a no-location marker
 */

void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position)
{
    static const location_t connectivity[MAXLOCS][5] = {
    /* 0*/ {  1, NOL, NOL, NOL, NOL},
    /* 1*/ {  0,   2,   7, NOL, NOL},
    /* 2*/ {  1,   3,   7,   8, NOL},
    /* 3*/ {  2,   4,   8,   9, NOL},
    /* 4*/ {  3,   5,   9,  10, NOL},
    /* 5*/ {  4,   6,  10, NOL, NOL},
    /* 6*/ {  5, NOL, NOL, NOL, NOL},
    /* 7*/ {  1,   2,  11, NOL, NOL},
    /* 8*/ {  2,   3,  12, NOL, NOL},
    /* 9*/ {  3,   4,  13, NOL, NOL},
    /*10*/ {  4,   5,  14, NOL, NOL},
    /*11*/ {  7,  15, NOL, NOL, NOL},
    /*12*/ {  8,  16, NOL, NOL, NOL},
    /*13*/ {  9,  17, NOL, NOL, NOL},
    /*14*/ { 10,  18, NOL, NOL, NOL},
    /*15*/ { 11,  19, NOL, NOL, NOL},
    /*16*/ { 12,  20, NOL, NOL, NOL},
    /*17*/ { 13,  21, NOL, NOL, NOL},
    /*18*/ { 14,  22, NOL, NOL, NOL},
    /*19*/ { 15, NOL, NOL, NOL, NOL},
    /*20*/ { 16, NOL, NOL, NOL, NOL},
    /*21*/ { 17, NOL, NOL, NOL, NOL},
    /*22*/ { 18, NOL, NOL, NOL, NOL},
    };

    static const cost_t inf = INF_COST;
    static const cost_t costs[MAXLOCS][4] = {
    /* 0*/ {  1, inf, inf, inf},
    /* 1*/ {  1,   2,   2, inf},
    /* 2*/ {  2,   2,   2,   2},
    /* 3*/ {  2,   2,   2,   2},
    /* 4*/ {  2,   2,   2,   2},
    /* 5*/ {  2,   1,   2, inf},
    /* 6*/ {  1, inf, inf, inf},
    /* 7*/ {  2,   2,   1, inf},
    /* 8*/ {  2,   2,   1, inf},
    /* 9*/ {  2,   2,   1, inf},
    /*10*/ {  2,   2,   1, inf},
    /*11*/ {  1,   1, inf, inf},
    /*12*/ {  1,   1, inf, inf},
    /*13*/ {  1,   1, inf, inf},
    /*14*/ {  1,   1, inf, inf},
    /*15*/ {  1,   1, inf, inf},
    /*16*/ {  1,   1, inf, inf},
    /*17*/ {  1,   1, inf, inf},
    /*18*/ {  1,   1, inf, inf},
    /*19*/ {  1, inf, inf, inf},
    /*20*/ {  1, inf, inf, inf},
    /*21*/ {  1, inf, inf, inf},
    /*22*/ {  1, inf, inf, inf},
    };

    *row_connectivity = connectivity[position];
    *row_costs = costs[position];
}

void FloodFill(route_t routes[MAXLOCS], cost_t costs[MAXLOCS], location_t origin)
{
    location_t const * connectivity;
    cost_t const * connection_costs;
    GetConnectivity(&connectivity, &connection_costs, origin);

    cost_t accumulated_cost = costs[origin];

    for(size_t i=0; connectivity[i] != NOL; ++i)
    {
        location_t destination = connectivity[i];
        cost_t cost = accumulated_cost + connection_costs[i];

        if(MinCost(cost, costs[destination]) == costs[destination]) continue;

        costs[destination] = cost;
        routes[destination] = (route_t) (routes[origin] | (1u << destination));

        FloodFill(routes, costs, destination);
    }
}

RoutingTable BuildRoutingTable(ProblemData const * pdata)
{
    RoutingTable routing;
    
    for(location_t i = 0; i<MAXLOCS; ++i)
    {
        for(location_t j=0; j<MAXLOCS; ++j)
        {
            routing.routes[i][j] = 0;
            routing.costs[i][j] = INF_COST;
        }

        routing.costs[i][i] = 0;
        routing.routes[i][i] = 0;

        FloodFill(routing.routes[i], routing.costs[i], i);
    }

    // Disabling paths forbidden by rules or by optimization
    for(location_t src = 0; src != pdata->n_locations; ++src)
    {
        for(location_t dst=0; dst != pdata->n_locations; ++dst)
        {
            if(RoomId(src) == RoomId(dst))
            {
                // OPTIMIZATION: Disables paths between same room
                routing.routes[src][dst] = 0;
                routing.costs[src][dst] = INF_COST;
            }
        }
    }

    // Disabling paths forbidden by part 1
    for(location_t src = pdata->n_locations; src != MAXLOCS; ++src)
    {
        for(location_t dst = pdata->n_locations; dst != MAXLOCS; ++dst)
        {
            routing.routes[src][dst] = 0;
            routing.costs[src][dst] = INF_COST;
        }
    }

    return routing;
}

void PrintRoutingTable(RoutingTable * t, ProblemData const * data)
{
    for(location_t i=0; i<data->n_locations; ++i)
    {
        for(location_t j=0; j<data->n_locations; ++j)
        {
            if(t->costs[i][j] == INF_COST) continue;

            printf("Trip %2d -> %2d (Cost=%2d): ", i,j, t->costs[i][j]);

            route_t route = t->routes[i][j];
            for(location_t i=0; route != 0; ++i)
            {
                if(route & 1)
                    printf(" %X", i);
                route = (route_t) (route >> 1);
            }
            printf("\n");
        }
        printf("\n");
    }
}

route_t GetRoomMembers(location_t room_id)
{
    switch (room_id) {
        case HALLWAY_ID:
                return 0x00003F;  // 0000 0000 0000 0000 0111 1111 in binary (locs 0,1,2,3,4,5,6)
        case 1: return 0x088880;  // 0000 1000 1000 1000 1000 0000 in binary (locs  7,11,15,19)
        case 2: return 0x111100;  // 0001 0001 0001 0001 0000 0000 in binary (locs  8,12,16,20)
        case 3: return 0x222200;  // 0010 0010 0010 0010 0000 0000 in binary (locs  9,13,17,21)
        case 4: return 0x444400;  // 0100 0100 0100 0100 0000 0000 in binary (locs 10,14,18,22)
    }

    fprintf(stderr, "Invalid room id: %d (%s, %d)", room_id, __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}
