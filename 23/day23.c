#include "day23.h"

#include "gamestate.h"
#include "routing.h"
#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>




int MovementCost(short player_id)
{
    switch (player_id / 2)
    {
        case 0: return 1;
        case 1: return 10;
        case 2: return 100;
        case 3: return 1000;
        default:
            fprintf(stderr, "Error: invalid player id: %d (%s:%d)\n", player_id, __FILE__, __LINE__);
            exit(EXIT_FAILURE);
    }
}


/*
 *  Rules:
 *   - Due to geometry:
 *     IMPL:
 *      1 There is no node in front of rooms
 *      2 It never makes sense to move around the same room
 *      3 Not moving is not a move
 *
 *   - Amphipods will never move from the hallway into a room unless that room is their destination room 
 *     IMPL: 
 *      4 You can never leave a room if you have moved already
 *
 *   - Once an amphipod stops moving in the hallway, it will stay in that spot until it can move into a room.
 *     IMPL:
 *      5 Implemented in routing table
 * 
 *
 */
void ComputePlayerPossibleContinuations(
    player_t player_id,
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    GamestateArray * continuations,
    CostArray * costs)
{
    location_t location = ugs->locations[player_id];
    bool moveflag = ugs->moveflags[player_id];

    if(RoomId(location)!=HALLWAY_ID && moveflag) return; // Cannot move again! (IMPL 4)

    for(location_t destination = 0; destination < NLOCS; ++destination)
    {
        if(routing_table->costs[location][destination] == -1) continue; // Invalid path (IMPL 2, 3, 5)

        if(RoomId(destination)==HALLWAY_ID && moveflag) continue; // Cannot move around the halway->must get into room

        LocationArray const * route = &routing_table->routes[location][destination];

        // Traversing route, checking for blockades
        for(location_t const * it = route->begin; it != route->end; ++it)
        {
            if(ugs->blockades[*it]) continue;
        }

        // Adding new gamestate and cost to continuations
        UnpackedGamestate cont_packed = CopyLocationsAndFlags(ugs);
        gamestate_t cont = PackGamestate(&cont_packed);
        PUSH(*continuations, cont);

        cost_t cost = MovementCost(player_id) * routing_table->costs[location][destination];
        PUSH(*costs, cost);
    }

}


void ComputePossibleContinuations(
    gamestate_t gs,
    RoutingTable const * routing_table,
    GamestateArray * continuations,
    CostArray * costs)
{
    continuations->end = continuations->begin; // Emptying without releasing memory
    costs->end = costs->begin; // Emptying without releasing memory

    UnpackedGamestate ugs = UnpackGamestate(gs);

    for(player_t i = 0; i<NPLAYERS; ++i)
    {
        ComputePlayerPossibleContinuations(i, &ugs, routing_table, continuations, costs);
    }   
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
    ClearRoutingTable(&routing);

    CLEAR(continuations);
    CLEAR(costs);

    return is_test;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
