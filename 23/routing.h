#ifndef DAY_23_ROUTING_TABLE_H
#define DAY_23_ROUTING_TABLE_H

#include <stdbool.h>
#include <inttypes.h>

#include "common/vector.h"


/** IDs of nodes
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
 * Since order doesn't matter, routes are stored bitwise. This allows for instant
 * checks for blockages without having to iterate a container
 *  
 *             222 1111 1111 11
 *             210 9876 5432 1098 7654 3210 < Location each bit refers to
 *  0000 0000 0001 0001 0000 0010 0001 0001
 *  ~~~~~~~~~~~       ^    ^      ^^ This route passes through 0>1>7>11
 *  ^ First few bits unused: always 0
 *
 */

struct problem_data_;

typedef uint_least8_t location_t;
typedef uint_least32_t route_t;
typedef int cost_t;

#define INF_COST -1

#define NOL     ((location_t) 31) /* No location*/
#define MAXLOCS ((location_t) 23) /* 23 nodes at most */

TEMPLATE_VECTOR(cost_t) CostArray;
TEMPLATE_VECTOR(location_t) LocationArray;

// Two indices: source and destination. (Table is symmetrical but who cares, theres only one instance ever)
typedef struct {
    route_t routes[MAXLOCS][MAXLOCS];
    cost_t costs[MAXLOCS][MAXLOCS];
} RoutingTable;

void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position);

void FloodFill(route_t routes[MAXLOCS], cost_t costs[MAXLOCS], location_t origin);

RoutingTable BuildRoutingTable(struct problem_data_ const * pdata);
void PrintRoutingTable(RoutingTable * t, struct problem_data_ const * data);
void ClearRoutingTable(RoutingTable * t);

/** Returns the id of the rooms.
 * 
 * The halway is considered room 0,
 * then it increases from left to right.
 *
 * Kept in header file so it can be inlined in other compilation units
 */
#define HALLWAY_ID 0
static inline location_t RoomId(location_t location)
{
    if(location < 7) return HALLWAY_ID;
    return (location_t) ((location - 7)%4 + 1);
}

route_t GetRoomMembers(location_t room_id);

cost_t MinCost(cost_t A, cost_t B);
cost_t AddCosts(cost_t A, cost_t B);



#endif
