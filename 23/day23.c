#include "day23.h"
#include "gamestate.h"
#include "routing.h"
#include "dijkstra.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>


solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 23);
    gamestate_t initial_gamestate = ReadGamestate(file);
    fclose(file);

    // printf("Gamestate: %010lX\n", initial_gamestate);

    RoutingTable routing = BuildRoutingTable();

    // PrintRoutingTable(&routing);

    // GamestateArray continuations;
    // CostArray costs;

    // NEW_VECTOR(continuations);
    // NEW_VECTOR(costs);

    // ComputePossibleContinuations(gs, &routing, &continuations, &costs);

    // for(size_t i=0; i<SIZE(continuations); ++i)
    // {
    //     UnpackedGamestate ugs = UnpackGamestate(continuations.begin[i]);
    //     printf("Move with cost %d:\n", costs.begin[i]);
    //     PrettyPrintGamestate(&ugs);
    // }

    // CLEAR(continuations);
    // CLEAR(costs);

    cost_t result = Dijkstra(initial_gamestate, &routing);

    return result;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
