#include "gamestate.h"
#include "common/file_utils.h"

#include "routing.h"
#include <string.h>

void GetLine(char ** line, FILE * file)
{
    free(*line);
    size_t len = 0;
    if(getline(line, &len, file) == -1)
    {
        fprintf(stderr, "Failed to read line\n");
        exit(EXIT_FAILURE);
    }
}


void ReadLine(char const * line, size_t row, GameState * gs, short counts[4])
{
    location_t loc = (location_t) (7 + 4*row);
    for(size_t col=0; col<4; ++col)
    {
        char letter = line[3 + 2*col];
        
        player_t colour    = (player_t)   (letter - 'A');
        player_t player_id = (location_t) (colour + 4*counts[colour]);

        gs->locations[player_id] = loc++;

        ++counts[colour];
    }
}

GameState ReadGamestate(ProblemData const * pdata)
{
    char * line = NULL;

    short counts[4] = {0,0,0,0}; // Number of read As, Bs, Cs and Ds

    GameState gs;

    FILE * file = GetFile(pdata->is_test, 23);
    GetLine(&line, file); // Skipping line 1
    GetLine(&line, file); // Skipping line 2
    
    size_t row = 0;
    GetLine(&line, file);
    ReadLine(line, row++, &gs, counts);

    if(pdata->part == 2)
    {
        if(pdata->is_test)
        {
            ReadLine("  #B#A#C#D#", row++, &gs, counts);
            ReadLine("  #A#B#C#D#", row++, &gs, counts);
        }
        else
        {
            ReadLine("  #D#C#B#A#", row++, &gs, counts);
            ReadLine("  #D#B#A#C#", row++, &gs, counts);
        }
    }

    GetLine(&line, file);
    ReadLine(line, row, &gs, counts);
    
    free(line);
    fclose(file);

    for(size_t i=0; i<MAX_PLAYERS; ++i)
    {
        gs.moveflags[i] = false;
    }

    return gs;
}

char PlayerLetter(player_t id)
{
    int correct_room = Colour(id) - 1;

    return (char) ('A' + correct_room);
}

void PrettyPrintGamestate(GameState const * gs, ProblemData const * pd)
{    
    char * map = malloc(pd->n_locations * sizeof(*map));

    for(size_t i=0; i!=pd->n_locations; ++i)
    {
        map[i] = ' ';
    }

    for(player_t i=0; i<pd->n_players; ++i)
    {
        map[gs->locations[i]] = PlayerLetter(i);
    }

    printf("#############\n");
    printf("#%c%c %c %c %c %c%c#\n", map[0],map[1],map[2],map[3],map[4],map[5],map[6]);
    printf("###%c#%c#%c#%c###\n",              map[7],  map[8],  map[9],  map[10]);
    printf("  #%c#%c#%c#%c#\n",               map[11], map[12], map[13], map[14]);

    if(pd->part == 2)
    {
        printf("  #%c#%c#%c#%c###\n",          map[15], map[16], map[17], map[18]);
        printf("  #%c#%c#%c#%c#\n",            map[19], map[20], map[21], map[22]);
    }

    printf("  #########\n\n");


    free(map);
}

bool WinningGamestate(GameState const * gs, ProblemData const * pd)
{
    for(player_t player_id=0; player_id != pd->n_players; ++player_id)
    {
        location_t room_id   = RoomId(gs->locations[player_id]);
        player_t target_room = Colour(player_id);

        if(room_id != target_room) return false;
    }

    return true;
}

cost_t MovementCost(player_t player_id)
{
    switch (Colour(player_id))
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


bool ValidateNoObstruction(
    route_t obstructions,
    RoutingTable const * routing_table,
    location_t source,
    location_t destination)
{
    return (routing_table->routes[source][destination] & obstructions) == 0;
}

bool ValidateDestinationRoom(
    player_t player_id,
    location_t destination,
    GameState const * ugs,
    ProblemData const * pd)
{
    location_t destination_room = RoomId(destination);

    if(destination_room == HALLWAY_ID) return true;

    player_t correct_room = Colour(player_id);

    if(correct_room != destination_room) return false;  // Wrong destination (IMPL 2)

    for(player_t other=0; other<pd->n_players; ++other)
    {
        if(Colour(other) == correct_room) continue;

        if(RoomId(ugs->locations[other]) == destination_room) return false; // Mixed cohabitation (IMPL 3)
    }
    return true;
}


/*
 * Prevents situations like this:
 *
 *  #############
 *  # B         #
 *  ###A#C#b#D###
 *    # #d#c#a#
 *    #########
 *
 * where A is blocking Room 1.
 */
bool ValidateNoPointlessBlockage(
    location_t destination,
    route_t obstructions,
    ProblemData const * pdata)
{
    location_t room = RoomId(destination);

    if(room == HALLWAY_ID) return true;

    route_t available_spaces = GetRoomMembers(room) &(~obstructions);

    location_t deepest_loc = NOL;

    location_t curr_loc = 0;
    for(; available_spaces != 0; available_spaces >>= 1)
    {
        if(available_spaces & 1) deepest_loc = curr_loc;
        ++curr_loc;

        if(curr_loc >= pdata->n_locations) break;
    }
    
    return (destination == deepest_loc);
}


/*
 *  Rules:
 *   - Amphipods will never stop on the space immediately outside any room
 *     IMPL:
 *      1 Implemented in routing table: There is no node in front of rooms
 *
 *   - Amphipods will never move from the hallway into a room unless that room is their destination room
 *     and that room contains no amphipods which do not also have that room as their own destination.
 *     IMPL: 
 *      2 You can never move into the wrong room
 *      3 No mixed cohabitation
 *
 *   - Once an amphipod stops moving in the hallway, it will stay in that spot until it can move into a room.
 *     IMPL:
 *      4 Implemented in routing table: hallway->hallway has infinite cost
 *
 *   - Amphipods can move up, down, left, or right so long as they are moving into an unoccupied open space.
 *      5 No moveing through blocked spaces
 *
 *   ==Optimizations==
 *
 *   -  It never makes sense to move around the same room
 *      IMPL:
 *       6 Implemented in routing table: infinite cost in inter-room movements
 *
 *   -  Not moving is not a move
 *      IMPL:
 *       7 (Automatically true with IMPL 4 and 6)
 * 
 *   -  Once an amphipod has moved into a room, it never makes sense to move again
 *      IMPL:
 *       8 No moving outside second room
 *
 *   - It never makes sense to not move to the bottom of the room
 *       9 Disalled moving to the front of an empty room
 */
void ComputePlayerPossibleContinuations(
    player_t player_id,
    GameState const * gs,
    route_t obstructions,
    RoutingTable const * routing_table,
    ProblemData const * problem_data,
    ContinuationArray * continuations)
{
    location_t source = gs->locations[player_id];
    bool moveflag = gs->moveflags[player_id];

    if(RoomId(source)!=HALLWAY_ID && moveflag) return; // Cannot move again! (IMPL 8)

    for(location_t destination=0; destination < problem_data->n_locations; ++destination)
    {
        if(routing_table->costs[source][destination] == INF_COST) continue; // IMPL 4,6,7

        if(!ValidateNoObstruction(obstructions, routing_table, source, destination)) continue; // IMPL 5
        
        if(!ValidateDestinationRoom(player_id, destination, gs, problem_data)) continue;  // IMPL 2,3

        if(!ValidateNoPointlessBlockage(destination, obstructions, problem_data)) continue; // IMPL 9

        // Adding to continuations
        Continuation c;
        
        c.state = *gs;
        c.state.locations[player_id] = destination;
        c.state.moveflags[player_id] = true;

        c.cost = MovementCost(player_id) * routing_table->costs[source][destination];
        
        PUSH(*continuations, c);
    }
}

route_t ComputeObstructions(GameState const * gs, ProblemData const * pdata)
{
    route_t obstructions = 0;
    for(location_t const * it = gs->locations; it != gs->locations + pdata->n_players; ++it)
    {
        obstructions |= (1u << *it);
    }
    return obstructions;
}


void ComputePossibleContinuations(
    GameState const * gs,
    RoutingTable const * routing_table,
    ProblemData const * problem_data,
    ContinuationArray * continuations)
{
    route_t obstructions = ComputeObstructions(gs, problem_data);

    for(player_t i = 0; i<problem_data->n_players; ++i)
    {
        ComputePlayerPossibleContinuations(i, gs, obstructions, routing_table, problem_data, continuations);
    }
}
