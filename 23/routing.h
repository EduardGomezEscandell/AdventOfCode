#ifndef DAY_23_ROUTING_TABLE_H
#define DAY_23_ROUTING_TABLE_H

#include <stdbool.h>
#include <inttypes.h>

#include "common/vector.h"


/** IDs of nodes ()
 * 
 * 8---9---+---A---+---B---+---C---+---D---E
 *         |       |       |       |
 *         4       5       6       7
 *         |       |       |       |
 *         0       1       2       3
 * 
 * F used as invalid location marker
 */


typedef uint_least8_t location_t;
typedef int cost_t;

#define INF_COST -1

#define NOL   ((location_t) 0xF) /* No location*/
#define NLOCS ((location_t) 0xF) /* 15 nodes */

TEMPLATE_VECTOR(location_t) LocationArray;
TEMPLATE_VECTOR(cost_t) CostArray;


// Two indices: source and destination. (Table is symmetrical but who cares, theres only one instance ever)
typedef struct {
    LocationArray routes[NLOCS][NLOCS];
    cost_t costs[NLOCS][NLOCS];
} RoutingTable;


void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position);

void FloodFill(LocationArray routes[NLOCS], cost_t costs[NLOCS], location_t origin);

RoutingTable BuildRoutingTable();
void PrintRoutingTable(RoutingTable * t);
void ClearRoutingTable(RoutingTable * t);

/** Returns the id of the rooms.
 * 
 * The halway is considered room 0,
 * then it increases from left to right.
 *
 * Kept in header file so it can be inlined in other compilation units
 */
#define HALLWAY_ID 0xF
static inline location_t RoomId(location_t location)
{
    if(location > 7) return HALLWAY_ID;
    return (location_t) (location % 4);
}

#endif


