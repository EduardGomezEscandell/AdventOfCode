#ifndef DAY_23_ROUTING_TABLE_H
#define DAY_23_ROUTING_TABLE_H

#include <stdbool.h>

#include "problem_data.h"

#include "common/vector.h"
#include "common/math.h"


/** 
 * This header and its accompanying source file are tasked with dealing
 * with the problems geometry. To accelerate the game's graph exporation,
 * all route costs are pre-computed and stored in a RoutingTable.
 * 
 * Invalid or sub-optimal moves are purged to prune the exploration tree.
 * 
 * GEOMETRY
 * ____________________________________________________________________________
 *
 * IDs of nodes
 * 
 * 0---1---+---2---+---3---+---4---+---5---6
 *         |       |       |       |
 *         7       8       9       10
 *         |       |       |       |
 *         11      12      13      14
 *         |       |       |       |
 *         15      16      17      18     ]  These nodes disabled
 *         |       |       |       |      ]  for problem part 1
 *         19      20      21      22     ]
 * 
 * 31 used as invalid location marker
 * 
 * 
 * ROUTING
 * ____________________________________________________________________________
 *
 * Since order doesn't matter, routes are stored bitwise. This allows for
 * instant checks for blockages without having to iterate a container
 *  
 * Each bit refers to a position. The rightmost bit is position 0 and increases
 * the left.
 *
 *  0000 0000 0001 0001 0000 1000 1000 0011
 *  ~~~~~~~~~~~              ^    ^      ^^  : This route passes through 0>1>7>11
 *            ^
 *            First few bits unused: always 0
 *
 *
 * To see if two routes have any nodes in common, one can use:
 *
 *    route1 & route2 == 0   --> true if no nodes in common
 *
 * This is particularly useful when comparing a route to a set of interesting
 * nodes, such as for checking obstructions in the route or checking if a room
 * is empty.
 *
 */

TEMPLATE_VECTOR(cost_t) CostArray;
TEMPLATE_VECTOR(location_t) LocationArray;

// Two indices: source and destination. (Table is symmetrical but there's only one instance ever)
typedef struct {
    route_t routes[MAXLOCS][MAXLOCS];
    cost_t costs[MAXLOCS][MAXLOCS];
} RoutingTable;

void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position);
void FloodFill(route_t routes[MAXLOCS], cost_t costs[MAXLOCS], location_t origin);
RoutingTable BuildRoutingTable(ProblemData const * pdata);

void PrintRoutingTable(RoutingTable * t, ProblemData const * data);

route_t GetRoomMembers(location_t room_id);


#define HALLWAY_ID 0
/** Returns the id of the rooms.
 * 
 * The halway is considered room 0,
 * then it increases from left to right.
 *
 * Kept in header file so it can be inlined in other compilation units
 */
static inline location_t RoomId(location_t location)
{
    if(location < 7) return HALLWAY_ID;
    return (location_t) ((location - 7)%4 + 1);
}

static inline cost_t MinCost(cost_t A, cost_t B)
{
    if(A == INF_COST) return B;
    if(B == INF_COST) return A;
    return MIN(A, B);
}

static inline cost_t AddCosts(cost_t A, cost_t B)
{
    if(A == INF_COST || B==INF_COST) return INF_COST;
    return A+B;
}



#endif
