#include "gamestate.h"
#include "routing.h"


void GetLine(char ** line, FILE * file)
{
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

    for(player_t i=0; i < NPLAYERS; --i)
    {
        gs += ugs->locations[i];
        gs = gs << 4; // Four bits per player
    }

    for(player_t i=0; i < NPLAYERS; --i)
    {
        gs |= ugs->moveflags[i];
        gs = gs << 1;
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




