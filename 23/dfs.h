#ifndef DAY_23_DFS_H
#define DAY_23_DFS_H

#include "gamestate.h"
#include "routing.h"

#include "common/vector.h"
#include "common/hash_table.h"


cost_t DFS(gamestate_t gamestate, RoutingTable * routing);
void DFS_impl(gamestate_t gamestate, RoutingTable * routing, cost_t  acc_cost, cost_t * current_best);

#endif
