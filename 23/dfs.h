#ifndef DAY_23_DFS_H
#define DAY_23_DFS_H

#include "gamestate.h"
#include "routing.h"

#include "common/vector.h"
#include "common/hash_table.h"


cost_t DFS(
    GameState const * gamestate,
    RoutingTable const * routing,
    ProblemData const * pdata);

void DFS_impl(
    GameState const * gamestate,
    RoutingTable const * routing,
    ProblemData const * pdata,
    cost_t acc_cost,
    cost_t * curr_best);

#endif
