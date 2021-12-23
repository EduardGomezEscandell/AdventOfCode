#include "day23.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>




int MovementCost(short player_id)
{
    switch (player_id / 2)
    {
        case 1: return 1;
        case 2: return 10;
        case 3: return 100;
        case 4: return 1000;
        default:
            fprintf(stderr, "Error: invalid player id: %d (%s:%d)\n", player_id, __FILE__, __LINE__);
            exit(EXIT_FAILURE);
    }
}





// void ExplorePossibleGamestates(gamestate_t gs, GamestateArray * moves, CostsArray * costs)
// {
//     moves->end = moves->begin; // Emptying without releasing memory
//     costs->end = costs->begin; // Emptying without releasing memory

//     bool blockades[0xF] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//     short players[8];
    
//     for(size_t i=0; i<8; ++i)
//     {
//         players[i] = GetLocation(gs, i);
//         blockades[players[i]] = true;
//     }

// }


solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 23);
    gamestate_t gs = ReadGamestate(file);
    fclose(file);

    printf("Gamestate: %010lX\n", gs);

    RoutingTable routing = BuildRoutingTable();

    PrintRoutingTable(&routing);

    return is_test;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
