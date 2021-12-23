#include "gamestate.h"
#include "23/day23.h"
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

    location_t locations[8] = {NOL, NOL, NOL, NOL, NOL, NOL, NOL, NOL};

    GetLine(&line, file); // Skipping line 1
    GetLine(&line, file); // Skipping line 2
    GetLine(&line, file);

    player_t type = (player_t) (line[3] - 'A');
    locations[type*2 + counts[type]] = 0x4;
    ++counts[type];

    type = (player_t) (line[5] - 'A');
    locations[type*2 + counts[type]] = 0x5;
    ++counts[type];

    type = (player_t) (line[7] - 'A');
    locations[type*2 + counts[type]] = 0x6;
    ++counts[type];

    type = (player_t) (line[9] - 'A');
    locations[type*2 + counts[type]] = 0x7;
    ++counts[type];

    GetLine(&line, file);
    type = (player_t) (line[3] - 'A');
    locations[type*2 + counts[type]] = 0x0;
    ++counts[type];

    type = (player_t) (line[5] - 'A');
    locations[type*2 + counts[type]] = 0x1;
    ++counts[type];

    type = (player_t) (line[7] - 'A');
    locations[type*2 + counts[type]] = 0x2;
    ++counts[type];

    type = (player_t) (line[9] - 'A');
    locations[type*2 + counts[type]] = 0x3;
    ++counts[type];

    free(line);

    gamestate_t gamestate = 0;
    for(player_t i=0; i<8; ++i)
    {
        gamestate = gamestate << 4;
        gamestate |= locations[i];
    }
    gamestate = gamestate << 8; // Moved flags: none have moved

    return gamestate;
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
    for(location_t i=0; i<NLOCS; ++i) {
        ugs.blockades[i] = 0;
    }

    // Filling blockades
    for(player_t i=0; i<NPLAYERS; ++i) {
        ugs.blockades[ugs.locations[i]] = true;
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
    player_t type = id / 2;

    switch(id % 2)
    {
        case 0: return (char) ('A' + type);
        case 1: return (char) ('a' + type);
    }
    
    fprintf(stderr, "Unreachable code (%s:%d)\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}

void PrettyPrintGamestate(UnpackedGamestate * ugs)
{
    char map[NLOCS] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};

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

    // Reading locations
    player_t rooms[4] = {NO_PLAYER, NO_PLAYER, NO_PLAYER, NO_PLAYER};

    for(player_t i=NPLAYERS; i > 0; --i)
    {
        player_t colour = (player_t) ((i - 1) / 2);
        location_t loc = gs & 0xF;
        gs = gs >> 4; // Four bits per player

        location_t room_id = RoomId(loc);

        if(room_id == HALLWAY_ID) return false;  // Player in halway

        if(rooms[room_id] == NO_PLAYER)
        {
            rooms[room_id] = colour;
            continue;
        }
        
        if(rooms[room_id] != colour) return false; // Diferent color cohabitation
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


bool ValidatePath(
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    location_t source,
    location_t destination)
{
    LocationArray const * route = &routing_table->routes[source][destination];

    for(location_t const * it = route->begin + 1; it < route->end; ++it)
    {
        if(ugs->blockades[*it]) return false;
    }
    return true;
}

bool ValidateCohabitation(
    player_t player_id,
    location_t destination,
    UnpackedGamestate * ugs)
{
    location_t room = RoomId(destination);
    if(room == HALLWAY_ID) return true;

    player_t colour = (player_id / 2);

    for(player_t i=0; i<NPLAYERS; ++i)
    {
        if(colour == i/2) continue;

        if(RoomId(ugs->locations[i]) == room) return false;
    }
    return true;
}

/*
 *  Rules:
 *   - Due to geometry:
 *     IMPL:
 *      1 There is no node in front of rooms
 *      2 It never makes sense to move around the same room: infinite cost in routing table
 *      3 Not moving is not a move: infinite cost in routing table
 *
 *   - Amphipods will never move from the hallway into a room unless that room is their destination room 
 *     IMPL: 
 *      4 You can never leave a room if you have moved already
 *
 *   - Once an amphipod stops moving in the hallway, it will stay in that spot until it can move into a room...
 *     IMPL:
 *      5 Implemented in routing table: halway->halway have infinite cost
 *
 *   - ...and that room contains no amphipods which do not also have that room as their own destination.
 *     IMPL:
 *      6 Cannot enter a room with diferent-colored player
 *
 */
void ComputePlayerPossibleContinuations(
    player_t player_id,
    UnpackedGamestate * ugs,
    RoutingTable const * routing_table,
    GamestateArray * continuations,
    CostArray * costs)
{
    location_t source = ugs->locations[player_id];
    bool moveflag = ugs->moveflags[player_id];

    if(RoomId(source)!=HALLWAY_ID && moveflag) return; // Cannot move again! (IMPL 4)

    for(location_t destination = 0; destination < NLOCS; ++destination)
    {
        if(routing_table->costs[source][destination] == INF_COST) continue; // Invalid path (IMPL 2, 3, 5)

        if(RoomId(destination)==HALLWAY_ID && moveflag) continue; // Cannot move around the halway->must get into room

        // Traversing route, checking for blockades
        bool valid = ValidatePath(ugs, routing_table, source, destination);
        if(!valid) continue;
        

        // Adding new gamestate and cost to continuations
        UnpackedGamestate ucont = CopyLocationsAndFlags(ugs);
        ucont.locations[player_id] = destination;
        ucont.moveflags[player_id] = true;
        gamestate_t cont = PackGamestate(&ucont);
        PUSH(*continuations, cont);

        cost_t cost = MovementCost(player_id) * routing_table->costs[source][destination];
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