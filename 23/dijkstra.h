#ifndef DAY_23_DIJKSTRA_H
#define DAY_23_DIJKSTRA_H

#include "common/hash_table.h"
#include "routing.h"
#include "gamestate.h"

TEMPLATE_HASH_TABLE(gamestate_t, gamestate_t, cost_t, GsDict);

cost_t Dijkstra(gamestate_t initial_gamestate, RoutingTable * routing);

#endif
