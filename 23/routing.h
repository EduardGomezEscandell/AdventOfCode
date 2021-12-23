#ifndef DAY_23_ROUTING_TABLE_H
#define DAY_23_ROUTING_TABLE_H

#include <limits.h>
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
typedef uint cost_t;

#define NOL 0xF /* No location*/
#define NLOCS 0xF /* 15 nodes */

TEMPLATE_VECTOR(location_t) LocationArray;
TEMPLATE_VECTOR(cost_t) CostsArray;

typedef struct {
    LocationArray routes[NLOCS][NLOCS];
    cost_t costs[NLOCS][NLOCS];
} RoutingTable;


void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position);

void FloodFill(LocationArray routes[NLOCS], cost_t costs[NLOCS], short origin);

RoutingTable BuildRoutingTable();
void PrintRoutingTable(RoutingTable * t);

#endif


