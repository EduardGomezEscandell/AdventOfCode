#include "routing.h"

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

/* No location. Used a a sentinel in arrays. */
#define NOL 0xF

/* 15 nodes */
#define NLOCS 0xF

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

    // For readability reasons
    const cost_t one = 1;
    const cost_t TWO = 2;

    static const cost_t costs[NLOCS][4] = {
    /*0*/ {one,   0,   0,   0},
    /*1*/ {one,   0,   0,   0},
    /*2*/ {one,   0,   0,   0},
    /*3*/ {one,   0,   0,   0},
    /*4*/ {one, TWO, TWO,   0},
    /*5*/ {one, TWO, TWO,   0},
    /*6*/ {one, TWO, TWO,   0},
    /*7*/ {one, TWO, TWO,   0},
    /*8*/ {one,   0,   0,   0},
    /*9*/ {TWO, one, one,   0},
    /*A*/ {TWO, TWO, one, one},
    /*B*/ {TWO, TWO, one, one},
    /*C*/ {TWO, TWO, one, one},
    /*D*/ {TWO, one, one,   0},
    /*E*/ {one,   0,   0,   0}
    };

    *row_connectivity = connectivity[position];
    *row_costs = costs[position];
}


inline void ReplaceRoute(
    LocationArray * reciever,
    LocationArray const * copied,
    location_t destination)
{
    reciever->end = reciever->begin; // Emptying without releasing memory
    NEW_VECTOR(*reciever);
    CONCATENATE(*reciever, *copied, location_t);
    PUSH(*reciever, destination);
}

void FloodFill(LocationArray routes[NLOCS], cost_t costs[NLOCS], location_t origin)
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
        ReplaceRoute(&routes[destination], &routes[origin], destination);

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
            NEW_VECTOR(routing.routes[i][j]);
            routing.costs[i][j] = UINT8_MAX; // Maximum real cost is 8
        }

        routing.costs[i][i] = 0;
        PUSH(routing.routes[i][i], i);

        FloodFill(routing.routes[i], routing.costs[i], i);
    }

    return routing;
}

void PrintRoutingTable(RoutingTable * t)
{
    for(location_t i=0; i<NLOCS; ++i)
    {
        for(location_t j=0; j<NLOCS; ++j)
        {
            printf("Trip %X -> %X (Cost=%d): ", i,j, t->costs[i][j]);

            for(location_t * it=t->routes[i][j].begin; it != t->routes[i][j].end; ++it)
            {
                printf(" %X", *it);
            }
            printf("\n");
        }
        printf("\n");
    }
}
