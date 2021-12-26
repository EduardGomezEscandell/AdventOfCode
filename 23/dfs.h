#ifndef DAY_23_DFS_H
#define DAY_23_DFS_H

#include "gamestate.h"
#include "routing.h"

#include "common/vector.h"
#include "common/hash_table.h"

int CompareCosts(Continuation const * A, Continuation const * B);

DECLARE_QUICKSORT_COMP(SortByCost, Continuation);

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
