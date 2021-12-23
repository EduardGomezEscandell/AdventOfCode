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

    short type = line[3] - 'A';
    locations[type*2 + counts[type]] = 0x4;
    ++counts[type];

    type = line[5] - 'A';
    locations[type*2 + counts[type]] = 0x5;
    ++counts[type];

    type = line[7] - 'A';
    locations[type*2 + counts[type]] = 0x6;
    ++counts[type];

    type = line[9] - 'A';
    locations[type*2 + counts[type]] = 0x7;
    ++counts[type];

    GetLine(&line, file);
    type = line[3] - 'A';
    locations[type*2 + counts[type]] = 0x0;
    ++counts[type];

    type = line[5] - 'A';
    locations[type*2 + counts[type]] = 0x1;
    ++counts[type];

    type = line[7] - 'A';
    locations[type*2 + counts[type]] = 0x2;
    ++counts[type];

    type = line[9] - 'A';
    locations[type*2 + counts[type]] = 0x3;
    ++counts[type];

    free(line);

    gamestate_t gamestate = 0;
    for(size_t i=0; i<8; ++i)
    {
        gamestate = gamestate << 4;
        gamestate |= locations[i];
    }
    gamestate = gamestate << 8; // Moved flags: none have moved

    return gamestate;
}


int GetLocation(gamestate_t gs, size_t player)
{
    return (gs >> (36 - 4*player)) & 0xF;
}
