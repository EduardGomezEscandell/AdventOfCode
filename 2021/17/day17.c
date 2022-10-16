#include "day17.h"
#include "common/file_utils.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

int ReadNextNum(char ** line, char delim_start, char delim_end)
{
    char* begin = *line;
    for(; *begin != '\0'; ++begin) {
        if(*begin == delim_start) {
            ++begin;
            break;
        }
    }

    char* end = begin;
    for(; *end != '\0'; ++end) {
        if(*end == delim_end) break;
    }

    *end = '\0';
    *line = end+1;

    return atoi(begin);
}

Target ReadTarget(const bool is_test)
{
    FILE * file = GetFile(is_test, 17);
    Target target;

    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    if((read = getline(&line, &len, file)) == -1)
    {
        fprintf(stderr, "Failed to open file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char * it = line;
    target.xmin = ReadNextNum(&it, '=', '.');
    target.xmax = ReadNextNum(&it, '.', ',');
    target.ymin = ReadNextNum(&it, '=', '.');
    target.ymax = ReadNextNum(&it, '.', '\n');

    free(line);
    fclose(file);

    return target;
}

int ComputeMaxCoordinate(int v_initial)
{
    return (v_initial + 1) * v_initial / 2;
}

bool HitsTarget(Target *t, int vx, int vy)
{    
    int x = 0;
    int y = 0;

    const size_t maxiter = 1e6;
    for(size_t i=0; i<maxiter; ++i)
    {
        x += vx;
        y += vy;
        vx = MAX(vx-1, 0);
        vy -= 1;

        if(x > t->xmax) return false;          // Gone too far to the right
        if(y < t->ymin && vy<0) return false;  // Gone too far down

        if(   x >= t->xmin 
           && y >= t->ymin 
           && y <= t->ymax) return true;
    }

    fprintf(stderr, "Reached maxiter! (%s:%d)\n", __FILE__, __LINE__);
    return false;
}

solution_t SolvePart1(const bool is_test)
{
    Target t = ReadTarget(is_test);

    int vy_max = - (t.ymin+1);
    int y_max = ComputeMaxCoordinate(vy_max);

    return y_max;
}

solution_t SolvePart2(const bool is_test)
{
    Target t = ReadTarget(is_test);

    int vy_max = -(t.ymin+1);

    unsigned int count = 0;
    for(int vx=t.xmax; ComputeMaxCoordinate(vx) >= t.xmin ; --vx)
    {
        for(int vy = vy_max; vy >= t.ymin; --vy)
        {
            count += HitsTarget(&t, vx, vy);
        }
    }

    return count;
}

DEFINE_TEST(1, 45)
DEFINE_TEST(2, 112)
