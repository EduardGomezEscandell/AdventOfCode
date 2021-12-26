#include "dfs.h"
#include "common/vector.h"

cost_t MinCost(cost_t A, cost_t B)
{
    if(A == INF_COST) return B;
    if(B == INF_COST) return A;
    return MIN(A, B);
}

cost_t AddCosts(cost_t A, cost_t B)
{
    if(A == INF_COST || B==INF_COST) return INF_COST;
    return A+B;
}

int CompareGamestate(gamestate_t const * A, gamestate_t const * B)
{
    if(*A == *B) return 0;
    return 1;
}

int CompareCosts(Continuation const * A, Continuation const * B)
{
    if(A->cost == B->cost) return 0;
    
    bool A_is_larger = MinCost(A->cost, B->cost) == B->cost;
    return A_is_larger ? 1 : -1;
}

gamestate_t HashGamestate(gamestate_t const * key, size_t n_buckets)
{
    return ((*key >> 8) + (*key << 32)) % n_buckets;
}

cost_t DFS(gamestate_t gamestate, RoutingTable * routing)
{
    cost_t best_cost = INF_COST;

    DFS_impl(gamestate, routing, 0, &best_cost);
    
    return best_cost;
}


DEFINE_QUICKSORT_COMP(SortByCost, Continuation, CompareCosts)



void DFS_impl(gamestate_t gamestate, RoutingTable * routing, cost_t acc_cost, cost_t * curr_best)
{
    ContinuationArray continuations;
    NEW_VECTOR(continuations);

    ComputePossibleContinuations(gamestate, routing, &continuations);

    SortByCost(continuations.begin, continuations.end);

    for(Continuation * it = continuations.begin; it != continuations.end; ++it)
    {
        cost_t total_cost = acc_cost + it->cost;
        
        if(MinCost(total_cost, *curr_best) == *curr_best)
        {
            break; // Prunning
        }

        if(WiningGamestate(it->state))
        {
            *curr_best = total_cost;
            printf("Found a winning gamestate with cost %d\n", *curr_best);
            UnpackedGamestate ugs = UnpackGamestate(it->state);
            PrettyPrintGamestate(&ugs);
            fflush(stdout);
        }
        else
        {
            DFS_impl(it->state, routing, total_cost, curr_best);
        }
    }
    CLEAR(continuations);
}
