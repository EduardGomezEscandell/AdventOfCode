#include "dijkstra.h"
#include "common/hash_table.h"
#include "common/vector.h"
#include "routing.h"
#include "gamestate.h"
#include <stdlib.h>

// Hash table template specialization
HT_DEFINE_DEFAULT_COMPARE(gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_NEW_AND_CLEAR  (gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_FIND           (gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_FIND_OR_EMPLACE(gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_REMOVE         (gamestate_t, gamestate_t, cost_t, GsDict)


gamestate_t GamestateDictHashIntegers(gamestate_t const * key, size_t n_buckets)
{
    return *key % n_buckets;
}

GsDictPair * FindMinimum(GsDict * dict)
{
    GsDictPair * min_item = dict->data.begin;
    if(min_item == dict->data.end) return NULL;

    cost_t min_value = dict->data.begin->value;

    for(GsDictPair * it = dict->data.begin + 1; it != dict->data.end; ++it)
    {
        if(it->value >= min_value) continue;

        min_item = it;
        min_value = it->value;
    }

    return min_item;
}


cost_t Dijkstra(gamestate_t initial_gamestate, RoutingTable * routing)
{
    GsDict queue = NewGsDict(GamestateDictHashIntegers);
    GsDict visited = NewGsDict(GamestateDictHashIntegers);

    *GsDictFindOrAllocate(&queue, initial_gamestate) = 0;

    GamestateArray continuations;
    CostArray cont_costs;

    NEW_VECTOR(continuations);
    NEW_VECTOR(cont_costs);

    while(SIZE(queue.data) > 0)
    {
        GsDictPair * it = FindMinimum(&queue);
        
        gamestate_t state = it->key;
        cost_t accumulated_cost = it->value;
        
        // Moving from queue to retirement
        GsDictRemove(&queue, state);
        *GsDictFindOrAllocate(&visited, state) = accumulated_cost;

        // Adding continuations to queue
        ComputePossibleContinuations(state, routing, &continuations, &cont_costs);
        for(size_t i=0; i < SIZE(continuations); ++i)
        {
            if(GsDictFind(&visited, &state).pair != NULL) continue; // Already visited

            if(WiningGamestate(continuations.begin[i]))
            {
                ClearGsDict(&queue);
                ClearGsDict(&visited);
                return accumulated_cost + cont_costs.begin[i];                
            }

            cost_t * cont_cost = GsDictFindOrEmplace(&queue, continuations.begin[i], 0);
            cost_t new_cont_cost = accumulated_cost + cont_costs.begin[i];
            *cont_cost = MAX(*cont_cost, new_cont_cost);
        }
    }

    fprintf(stderr, "Failed to find a wining gamestate (%s:%d)\n\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}
