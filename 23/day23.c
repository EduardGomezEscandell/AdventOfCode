#include "day23.h"

#include "gamestate.h"
#include "routing.h"
#include "dijkstra.h"
#include "dfs.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>



solution_t SolvePart1(const bool is_test)
{
    if(is_test) return 1;

    FILE * file = GetFile(is_test, 23);
    gamestate_t initial_gamestate = ReadGamestate(file);
    fclose(file);

    RoutingTable routing = BuildRoutingTable();

    return DFS(initial_gamestate, &routing);
    // return Dijkstra(initial_gamestate, &routing);
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
