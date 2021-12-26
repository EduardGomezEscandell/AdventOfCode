#include "dijkstra.h"
#include "gamestate.h"

#include "common/hash_table.h"
#include "common/vector.h"
#include "routing.h"

#include <stdlib.h>

// Hash table template specialization
HT_DEFINE_DEFAULT_COMPARE(gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_NEW_AND_CLEAR  (gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_FIND           (gamestate_t, gamestate_t, cost_t, GsDict)
HT_DEFINE_FIND_OR_EMPLACE(gamestate_t, gamestate_t, cost_t, GsDict)


GsDictPair ** FindBucketReference(GsDict *hs, GsDictBucket * bucket, gamestate_t const * key)
{
    for(GsDictPair ** it = bucket->begin; it != bucket->end; ++it)
    {
        if(hs->Compare(&(*it)->key, key) == 0) return it;
    }
    return NULL;
}

bool DictRemove(GsDict * hs, GsDictPair * it)
{
    // Finding bucket ptr
    GsDictSearchResult r_it = GsDictFind(hs, &it->key);
    if(r_it.pair == NULL) return false;
    GsDictPair ** it_bucket_ptr = FindBucketReference(hs, r_it.bucket, &it->key);

    // Finding bucket ptr for last element
    GsDictPair * last = &hs->data.end[-1];
    GsDictSearchResult r_last = GsDictFind(hs, &last->key);
    if(r_last.pair == NULL) return false;
    GsDictPair ** last_bucket_ptr = FindBucketReference(hs, r_last.bucket, &last->key);

    // Overwritting contents of `it` with last element's
    *it = *last;
    
    // Updating bucket ptr for last element
    *last_bucket_ptr = it;

    // Shrinking data container
    --hs->data.end;

    // Removing `it`'s bucket ptr
    *it_bucket_ptr = r_it.bucket->end[-1];
    r_it.bucket->end--;

    return true;
}


gamestate_t GsDictHashIntegers(gamestate_t const * key, size_t n_buckets)
{
    return ((*key >> 8) + (*key << 32)) % n_buckets;
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
    GsDict queue = NewGsDict(GsDictHashIntegers);
    GsDict visited = NewGsDict(GsDictHashIntegers);

    *GsDictFindOrAllocate(&queue, initial_gamestate) = 0;

    ContinuationArray continuations;

    NEW_VECTOR(continuations);

    while(SIZE(queue.data) > 0)
    {
        GsDictPair * it = FindMinimum(&queue);
        
        gamestate_t state = it->key;
        cost_t cost       = it->value;

        printf("Current cost: %d\n", it->value);
        printf("Queue size: %ld\n", SIZE(queue.data));
        printf("Vsted size: %ld\n", SIZE(visited.data));
        printf("\n");
        
        // Moving from queue to retirement
        if(DictRemove(&queue, it) == false)
        {
            fprintf(stderr, "Failed to remove gamestate %lx (%s:%d)", state, __FILE__, __LINE__);
            fflush(stderr);
            exit(EXIT_FAILURE);
        }

        *GsDictFindOrAllocate(&visited, state) = cost;

        // Adding continuations to queue
        ComputePossibleContinuations(state, routing, &continuations);
        
        for(Continuation * it = continuations.begin; it != continuations.end; ++it)
        {
            if(GsDictFind(&visited, &it->state).pair != NULL) continue; // Already visited

            cost_t total_cost = cost + it->cost;
            
            if(WiningGamestate(it->state))
            {
                printf("COMPLETED!\n");
                
                ClearGsDict(&queue);
                ClearGsDict(&visited);
                CLEAR(continuations);

                return total_cost;
            }

            cost_t * cont_cost = GsDictFindOrEmplace(&queue, it->state, total_cost);
            *cont_cost = MIN(*cont_cost, total_cost);
        }
    }

    fprintf(stderr, "Failed to find a wining gamestate (%s:%d)\n\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}
