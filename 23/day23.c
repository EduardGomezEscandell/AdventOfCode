#include "day23.h"

#include "gamestate.h"
#include "routing.h"
#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>



void Dijkstra(Node * begin, Node * end, Node * start, Node * target)
{
    Queue Q;
    NEW_VECTOR(Q);
    const size_t sz = end-begin;
    RESERVE(Q, sz);

    for(Node * it = begin; it != end; ++it)
    {
        it->distance = -1;  // Purposefully under-flowing
        // it->prev = NULL;
        it->visited = false;

        PUSH(Q, it);
    }
    start->distance = 0;

    Node ** visited_end = Q.begin;

    while(visited_end != Q.end)
    {
        Node ** it = MinEntry(visited_end, Q.end);
        
        /*Popping it from queue*/
        {
            Node *tmp = *it;
            *it = *visited_end;
            *visited_end = tmp;
            it = visited_end;
            ++visited_end;
        }

        Node * u = *it;
        u->visited = true;

        if(u->id == target->id) {
            CLEAR(Q);
            return;
        }

        for(size_t i=0; i<4; ++i)
        {
            Node * neigh = u->neighbours[i];
            if(!neigh || neigh->visited) continue;

            long long int alt = u->distance + Distance(u, neigh);

            if(alt < neigh->distance)
            {
                neigh->distance = alt;
                // neigh->prev = u;
            }
        }
    }

    CLEAR(Q);

    printf("DIJKSTRA: Failed to find a shortest path\n");

}


solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 23);
    gamestate_t gs = ReadGamestate(file);
    fclose(file);

    printf("Gamestate: %010lX\n", gs);

    RoutingTable routing = BuildRoutingTable();

    // PrintRoutingTable(&routing);

    GamestateArray continuations;
    CostArray costs;

    NEW_VECTOR(continuations);
    NEW_VECTOR(costs);

    ComputePossibleContinuations(gs, &routing, &continuations, &costs);

    for(size_t i=0; i<SIZE(continuations); ++i)
    {
        UnpackedGamestate ugs = UnpackGamestate(continuations.begin[i]);
        printf("Move with cost %d:\n", costs.begin[i]);
        PrettyPrintGamestate(&ugs);
    }


    ClearRoutingTable(&routing);
    CLEAR(continuations);
    CLEAR(costs);

    fflush(stdout);

    return is_test;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
