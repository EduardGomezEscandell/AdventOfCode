#include "routing.h"
#include "common/vector.h"

#include <stdio.h>

/** IDs of nodes (in hex)
 * 
 * 8---9---+---A---+---B---+---C---+---D---E
 *         |       |       |       |
 *         4       5       6       7
 *         |       |       |       |
 *         0       1       2       3
 * 
 * F used as a no-location marker
 */

void GetConnectivity(location_t const ** row_connectivity, cost_t const ** row_costs, location_t position)
{
    static const location_t connectivity[NLOCS][5] = {
    /*0*/ {0x4, NOL, NOL, NOL, NOL},
    /*1*/ {0x5, NOL, NOL, NOL, NOL},
    /*2*/ {0x6, NOL, NOL, NOL, NOL},
    /*3*/ {0x7, NOL, NOL, NOL, NOL},
    /*4*/ {0x0, 0x9, 0xA, NOL, NOL},
    /*5*/ {0x1, 0xA, 0xB, NOL, NOL},
    /*6*/ {0x2, 0xB, 0xC, NOL, NOL},
    /*7*/ {0x3, 0xC, 0xD, NOL, NOL},
    /*8*/ {0x9, NOL, NOL, NOL, NOL},
    /*9*/ {0x4, 0x8, 0xA, NOL, NOL},
    /*A*/ {0x4, 0x5, 0x9, 0xB, NOL},
    /*B*/ {0x5, 0x6, 0xA, 0xC, NOL},
    /*C*/ {0x6, 0x7, 0xB, 0xD, NOL},
    /*D*/ {0x7, 0xC, 0xE, NOL, NOL},
    /*E*/ {0xD, NOL, NOL, NOL, NOL}
    };

    static const cost_t costs[NLOCS][4] = {
    /*0*/ {    1, INF_COST, INF_COST, INF_COST},
    /*1*/ {    1, INF_COST, INF_COST, INF_COST},
    /*2*/ {    1, INF_COST, INF_COST, INF_COST},
    /*3*/ {    1, INF_COST, INF_COST, INF_COST},
    /*4*/ {    1,        2,        2, INF_COST},
    /*5*/ {    1,        2,        2, INF_COST},
    /*6*/ {    1,        2,        2, INF_COST},
    /*7*/ {    1,        2,        2, INF_COST},
    /*8*/ {    1, INF_COST, INF_COST, INF_COST},
    /*9*/ {    2,        1,        2, INF_COST},
    /*A*/ {    2,        2,        2,        2},
    /*B*/ {    2,        2,        2,        2},
    /*C*/ {    2,        2,        2,        2},
    /*D*/ {    2,        2,        1, INF_COST},
    /*E*/ {    1, INF_COST, INF_COST, INF_COST}
    };

    *row_connectivity = connectivity[position];
    *row_costs = costs[position];
}


void ReplaceRoute(
    LocationArray * reciever,
    LocationArray const * copied,
    location_t destination)
{
    reciever->end = reciever->begin; // Emptying without releasing memory
    CLEAR(*reciever);
    NEW_VECTOR(*reciever);
    CONCATENATE(*reciever, *copied, location_t);
    PUSH(*reciever, destination);
}

void FloodFill(route_t routes[NLOCS], cost_t costs[NLOCS], location_t origin)
{
    location_t const * connectivity;
    cost_t const * connection_costs;
    GetConnectivity(&connectivity, &connection_costs, origin);

    cost_t accumulated_cost = costs[origin];

    for(size_t i=0; connectivity[i] != NOL; ++i)
    {
        location_t destination = connectivity[i];
        cost_t cost = accumulated_cost + connection_costs[i];

        if(cost > costs[destination]) continue;

        costs[destination] = cost;
        routes[destination] = (route_t) (routes[origin] | (1 << destination));

        FloodFill(routes, costs, destination);
    }
}

RoutingTable BuildRoutingTable()
{
    RoutingTable routing;
    
    for(location_t i = 0; i<NLOCS; ++i)
    {
        for(location_t j=0; j<NLOCS; ++j)
        {
            routing.routes[i][j] = 0;
            routing.costs[i][j] = UINT8_MAX; // Maximum real cost is 8
        }

        routing.costs[i][i] = 0;
        routing.routes[i][i] = 0;

        FloodFill(routing.routes[i], routing.costs[i], i);
    }

    // Disabling paths forbidden by rules or by optimization
    for(location_t src = 0; src<NLOCS; ++src)
    {
        for(location_t dst=0; dst<NLOCS; ++dst)
        {
            if(src == dst || RoomId(src) == RoomId(dst))
            {
                // OPTIMIZATION: Disables paths between same room
                routing.routes[src][dst] = 0;
                routing.costs[src][dst] = INF_COST;
                continue;
            }

        }   
    }

    return routing;
}

void PrintRoutingTable(RoutingTable * t)
{
    for(location_t i=0; i<NLOCS; ++i)
    {
        for(location_t j=0; j<NLOCS; ++j)
        {
            if(t->costs[i][j] == INF_COST) continue;

            printf("Trip %X -> %X (Cost=%2d): ", i,j, t->costs[i][j]);

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

