#include "gamestate.h"

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


gamestate_t ReadGamestate(FILE * file)
{
    char * line = NULL;

    short counts[4] = {0,0,0,0}; // Number of read As, Bs, Cs and Ds

    UnpackedGamestate gs;

    GetLine(&line, file); // Skipping line 1
    GetLine(&line, file); // Skipping line 2
    GetLine(&line, file);

    player_t type = (player_t) (line[3] - 'A');
    gs.locations[type*2 + counts[type]] = 0x4;
    ++counts[type];

    type = (player_t) (line[5] - 'A');
    gs.locations[type*2 + counts[type]] = 0x5;
    ++counts[type];

    type = (player_t) (line[7] - 'A');
    gs.locations[type*2 + counts[type]] = 0x6;
    ++counts[type];

    type = (player_t) (line[9] - 'A');
    gs.locations[type*2 + counts[type]] = 0x7;
    ++counts[type];

    GetLine(&line, file);
    type = (player_t) (line[3] - 'A');
    gs.locations[type*2 + counts[type]] = 0x0;
    ++counts[type];

    type = (player_t) (line[5] - 'A');
    gs.locations[type*2 + counts[type]] = 0x1;
    ++counts[type];

    type = (player_t) (line[7] - 'A');
    gs.locations[type*2 + counts[type]] = 0x2;
    ++counts[type];

    type = (player_t) (line[9] - 'A');
    gs.locations[type*2 + counts[type]] = 0x3;
    ++counts[type];

    free(line);

    for(size_t i=0; i<NPLAYERS; ++i)
        gs.moveflags[i] = false;

    return PackGamestate(&gs);
}


UnpackedGamestate UnpackGamestate(gamestate_t gs)
{
    UnpackedGamestate ugs;

    // Reading flags
    for(player_t i=NPLAYERS; i > 0; --i)
    {
        ugs.moveflags[i-1] = gs & 1;
        gs = gs >> 1;
    }

    // Reading locations
    for(player_t i=NPLAYERS; i > 0; --i)
    {
        ugs.locations[i-1] = gs & 0xF;
        gs = gs >> 4; // Four bits per player
    }

    // Initializing blockades
    ugs.blockades = 0;

    // Filling blockades
    for(player_t i=0; i<NPLAYERS; ++i)
    {
        ugs.blockades |= (route_t) (1 << ugs.locations[i]);
    }

    return ugs;
}

gamestate_t PackGamestate(UnpackedGamestate * ugs)
{
    gamestate_t gs = 0;

    for(player_t i=0; i < NPLAYERS; ++i)
    {
        gs = gs << 4; // Four bits per player
        gs += ugs->locations[i];
    }

    for(player_t i=0; i < NPLAYERS; ++i)
    {
        gs = gs << 1;
        gs |= ugs->moveflags[i];
    }

    return gs;
}

UnpackedGamestate CopyLocationsAndFlags(UnpackedGamestate const * source)
{
    UnpackedGamestate out;

    for(player_t i=0; i < NPLAYERS; ++i)
    {
        out.locations[i] = source->locations[i];
        out.moveflags[i] = source->moveflags[i];
    }

    return out;
}


char HexChar(location_t num)
{
    if(num <= 9)
    {
        return (char) ('0' + num);
    }
    if(num > 9 && num <= 0xE)
    {
        return (char) ('A' + num);
    }
    return ' ';
}

char PlayerLetter(player_t id)
{
    int correct_room = CorrectRoom(id) - 1;

    switch(id % 2)
    {
        case 0: return (char) ('A' + correct_room);
        case 1: return (char) ('a' + correct_room);
    }
    
    fprintf(stderr, "Unreachable code (%s:%d)\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}

void PrettyPrintGamestate(UnpackedGamestate * ugs)
{
    char map[NLOCS];
    for(size_t i=0; i!=NLOCS; ++i)
        map[i] = ' ';

    for(player_t i=0; i<NPLAYERS; ++i)
    {
        map[ugs->locations[i]] = PlayerLetter(i);
    }


    printf("#############\n");
    printf("#%c%c %c %c %c %c%c#\n", map[8],map[9],map[0xA],map[0xB],map[0xC],map[0xD],map[0xE]);
    printf("###%c#%c#%c#%c###\n",               map[4],  map[5],  map[6],  map[7]);
    printf("  #%c#%c#%c#%c#\n",                 map[0],  map[1],  map[2],  map[3]);
    printf("  #########\n\n");

}

bool WiningGamestate(gamestate_t gs)
{
    // Ignoring flags
    gs = gs >> 8;

    for(player_t player_id=NPLAYERS-1; player_id >= 0; --player_id)
    {
        location_t loc = gs & 0xF;
        gs = gs >> 4; // Four bits per player

        location_t room_id   = RoomId(loc);
        player_t target_room = CorrectRoom(player_id);

        if(room_id != target_room) return false;
    }

    return true;
}

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


bool ValidateNoObstruction(
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    location_t source,
    location_t destination)
{
    return (routing_table->routes[source][destination] & ugs->blockades) == 0;
}

bool ValidateDestinationRoom(
    player_t player_id,
    location_t destination,
    UnpackedGamestate * ugs)
{
    location_t destination_room = RoomId(destination);

    if(destination_room == HALLWAY_ID) return true;

    player_t correct_room = CorrectRoom(player_id);

    if(correct_room != destination_room) return false;  // Wrong destination (IMPL 2)

    for(player_t other=0; other<NPLAYERS; ++other)
    {
        if(CorrectRoom(other) == correct_room) continue;

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
 * where A is blocking Room 1
 */
bool ValidateNoPointlessBlockage(
    location_t destination,
    UnpackedGamestate * ugs)
{
    if(destination < 4 || destination > 7) return true;

    route_t room = GetRoomMembers(RoomId(destination));

    if((room & ugs->blockades) == 0)
    {
        // Room is empty: should have gone to the bottom of the room
        return false;
    }

    return true;
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
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    ContinuationArray * continuations)
{
    location_t source = ugs->locations[player_id];
    bool moveflag = ugs->moveflags[player_id];

    if(RoomId(source)!=HALLWAY_ID && moveflag) return; // Cannot move again! (IMPL 8)

    for(location_t destination = 0; destination < NLOCS; ++destination)
    {
        if(routing_table->costs[source][destination] == INF_COST) continue; // IMPL 4,6,7

        if(!ValidateNoObstruction(ugs, routing_table, source, destination)) continue; // IMPL 5
        
        if(!ValidateDestinationRoom(player_id, destination, ugs)) continue;  // IMPL 2,3

        if(!ValidateNoPointlessBlockage(destination, ugs)) continue; // IMPL 9

        // Adding to continuations
        UnpackedGamestate ucont = CopyLocationsAndFlags(ugs);
        ucont.locations[player_id] = destination;
        ucont.moveflags[player_id] = true;

        Continuation c;
        c.state = PackGamestate(&ucont);
        c.cost  = MovementCost(player_id) * routing_table->costs[source][destination];
        PUSH(*continuations, c);
    }
}


void ComputePossibleContinuations(
    gamestate_t gs,
    RoutingTable const * routing_table,
    ContinuationArray * continuations)
{
    continuations->end = continuations->begin; // Emptying without releasing memory

    UnpackedGamestate ugs = UnpackGamestate(gs);

    for(player_t i = 0; i<NPLAYERS; ++i)
    {
        ComputePlayerPossibleContinuations(i, &ugs, routing_table, continuations);
    }
}
