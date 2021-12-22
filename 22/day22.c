#include "day22.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

InstructionVector ReadCubes(bool is_test)
{
    FILE* file = GetFile(is_test, 22);

    size_t len = 0;
    char * line = NULL;

    InstructionVector instructions;
    NEW_VECTOR(instructions);

    while(getline(&line, &len, file) != -1)
    {
        Instruction instr;
        char * line_ptr = line;

        switch (line[1]) {
        case 'n': instr.active = true;  break;
        case 'f': instr.active = false; break;
        default:
            fprintf(stderr, "Wrong input file format!\n");
            exit(EXIT_FAILURE);
        }

        line_ptr = strchr(line_ptr, '=') + 1;
        instr.x0 = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        instr.x1 = atoi(line_ptr) + 1; // non-inclusive


        line_ptr = strchr(line_ptr, '=') + 1;
        instr.y0 = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        instr.y1 = atoi(line_ptr) + 1;


        line_ptr = strchr(line_ptr, '=') + 1;
        instr.z0 = atoi(line_ptr);

        line_ptr = strchr(line_ptr, '.') + 2;
        instr.z1 = atoi(line_ptr) + 1;

        PUSH(instructions, instr);
    }

    free(line);
    fclose(file);

    return instructions;
}

void PrintInstruction(Instruction * instr)
{
    printf("[%d..%d, %d..%d, %d..%d](%d)\n", instr->x0, instr->x1, instr->y0, instr->y1, instr->z0, instr->z1, instr->active);
}

InstructionPtrVector CreatePtrVector(InstructionVector reference, coord_t *min_x, coord_t *max_x)
{
    InstructionPtrVector out;
    NEW_VECTOR(out);
    RESERVE(out, SIZE(reference));

    const coord_t min_x_0 = *min_x;
    const coord_t max_x_0 = *max_x;

    *min_x =  INT_MAX;
    *max_x = -INT_MAX;

    for(Instruction * it = reference.begin; it != reference.end; ++it)
    {
        PUSH(out, it);

        *min_x = MIN(*min_x, it->x0);
        *max_x = MAX(*max_x, it->x1);
    }

    *min_x = MAX(*min_x, min_x_0);
    *max_x = MIN(*max_x, max_x_0);

    if(out.begin == out.end)
    {
        *min_x = 0;
        *max_x = -1;
    }

    return out;
}

void CopyValidX(InstructionPtrVector * out, InstructionPtrVector const * in, coord_t x, coord_t * min_y, coord_t * max_y)
{
    out->end = out->begin; // Emptying without releasing memory

    const coord_t min_y_0 = *min_y;
    const coord_t max_y_0 = *max_y;

    *min_y =  INT_MAX;
    *max_y = -INT_MAX;

    bool none_write = true;

    for(Instruction ** it = in->begin; it != in->end; ++it)
    {
        if((*it)->x0 > x || x >= (*it)->x1) continue;

        PUSH(*out, *it);

        *min_y = MIN(*min_y, (*it)->y0);
        *max_y = MAX(*max_y, (*it)->y1);

        if((*it)->active) none_write = false;
    }

    *min_y = MAX(*min_y, min_y_0);
    *max_y = MIN(*max_y, max_y_0);

    if(none_write)
    {
        *min_y = 0;
        *max_y = 0;
    }
}

void CopyValidY(InstructionPtrVector * out, InstructionPtrVector const * in, coord_t y, coord_t * min_z, coord_t * max_z)
{
    out->end = out->begin; // Emptying without releasing memory

    const coord_t min_z_0 = *min_z;
    const coord_t max_z_0 = *max_z;

    *min_z =  INT_MAX;
    *max_z = -INT_MAX;

    bool none_write = true;

    for(Instruction ** it = in->begin; it != in->end; ++it)
    {
        if((*it)->y0 > y || y >= (*it)->y1) continue;

        PUSH(*out, *it);

        *min_z = MIN(*min_z, (*it)->z0);
        *max_z = MAX(*max_z, (*it)->z1);

        if((*it)->active) none_write = false;
    }

    *min_z = MAX(*min_z, min_z_0);
    *max_z = MIN(*max_z, max_z_0);

    if(none_write)
    {
        *min_z = 0;
        *max_z = 0;
    }
}

bool GetVoxelStatus(InstructionPtrVector const * vi_xyz, coord_t z)
{
    for(Instruction ** it = vi_xyz->end-1; it >= vi_xyz->begin; --it)
    {
        if((*it)->z0 <= z && z < (*it)->z1)
            return (*it)->active;
    }

    return 0;
}

solution_t SweepYZ(Instruction * instr)
{
    if(!instr->active) return 0;

    solution_t area = ((solution_t)(instr->y1 - instr->y0))
                    * ((solution_t)(instr->z1 - instr->z0));
    return area;
}

solution_t SweepZ(Instruction * instr)
{
    if(!instr->active) return 0;

    solution_t length = (instr->z1 - instr->z0);
    return length;
}

void InsertInactiveOverlap(SegmentVector * segments, coord_t start, coord_t finish)
{
    for(Segment * it = segments->begin; it != segments->end; ++it)
    {
        // Fully past segment
        // ---ooooo------- initial state
        // ----------xxx-- operation
        if(start >= it->finish) continue;

        // Fully before segment
        // --------ooooo--oo-o- initial state
        // ---xxx------------- operation
        if(finish <= it->start) return;

        // Fully within segment
        // ------ooooooooo----- initial state
        //----------xxxx------- operation
        //-------ooo----oo----- result
        if(start >= it->start && finish <= it->finish)
        {
            Segment new_segment = {finish, it->finish};

            it->finish = start;

            Segment * insert_position = it + 1;

            if(new_segment.start != new_segment.finish)
            {
                if(it->start == it->finish) {
                    *it = new_segment;
                } else {
                    INSERT(*segments, Segment, insert_position, new_segment);
                }
            }

            return;
        }

        // Right-intersect
        //  ------ooooo------- initial state
        //  ---------xxxxx---- operation
        //
        //  ------ooo--------- result
        //  -----------xxx---- remainder may intersect next segment
        if(start >= it->start && finish <= it->finish)
        {
            coord_t tmp = it->finish;
            it->finish = start;
            start = tmp;

            if(it->start == it->finish) {
                REMOVE(*segments, Segment, it);
            }

            if(start == finish) return;
            continue;
        }

        // Left-intersect
        //  ------ooooo------- initial state
        //  ----xxxxx--------- operation
        //
        //  ---------oo--------- result
        //  ----xx-------------- Remiander irrelevant
        if(start <= it->start && finish <= it->finish)
        {
            it->start = finish;

            if(it->start == it->finish) {
                REMOVE(*segments, Segment, it);
            }

            return;
        }
    }
}

void InsertActiveOverlap(SegmentVector * segments, coord_t start, coord_t finish)
{
    for(Segment * it = segments->begin; it != segments->end; ++it)
    {
        // Fully past segment
        // --ooooo------------- initial state
        // ----------ooooo----- operation
        if(start >= it->finish) continue;

        // Fully before segment
        // --------ooooo--oo-o- initial state
        // ---ooo------------- operation
        if(finish < it->start) {
            Segment new_segment = {start, finish};
            INSERT(*segments, Segment, it, new_segment);
            return;
        }


        // Fully within segment
        // ------ooooooooo----- initial state
        //----------oooo------- operation
        //-------ooooooooo----- result
        if(start >= it->start && finish <= it->finish)
        {
            return;
        }

        // Right-intersect
        //  ---ooooo-------- initial state
        //  ------ooooo----- operation
        //
        //  ----ooooooo----- result may intersect next segment!
        if(start >= it->start && finish <= it->finish)
        {
            it->finish = finish;
            // Repairing possible overlaps:
            // -----ooooooooooooooooo---- it
            //---------ooooooo---ooo----- next, next+1
            //------ooooooooooooooooo---- result
            for(Segment * next = it + 1; next != segments->end; ++next)
            {
                if(it->finish < next->start) break; // No intersection

                it->finish = MAX(it->finish, next->finish);

                REMOVE(*segments, Segment, next);
            }

            return;
        }

        // Left-intersect
        //  ------ooooo------- initial state
        //  ----ooooo--------- operation
        //
        //  ----ooooooo--------- result
        if(start <= it->start && finish <= it->finish)
        {
            it->start = start;
            return;
        }
    }

    // No intersections: append at end
    Segment new_segment = {start, finish};
    PUSH(*segments, new_segment);
}

solution_t ComputeOverlap(InstructionPtrVector * instructions)
{
    SegmentVector segments;
    NEW_VECTOR(segments);

    for(Instruction ** it = instructions->begin; it != instructions->end; ++it)
    {
        if((*it)->active) InsertActiveOverlap(&segments, (*it)->z0, (*it)->z1);
        else            InsertInactiveOverlap(&segments, (*it)->z0, (*it)->z1);
    }

    solution_t length = 0;
    for(Segment * it = segments.begin; it != segments.end; ++it)
    {
        length += it->finish - it->start;
    }

    return length;
}




solution_t Solve(const bool is_test, coord_t boot_region)
{
    InstructionVector instructions = ReadCubes(is_test);
    const size_t size = SIZE(instructions);

    coord_t begin_x = -boot_region, end_x = boot_region+1;
    InstructionPtrVector vi_x = CreatePtrVector(instructions, &begin_x, &end_x); // Valid instructions

    InstructionPtrVector vi_xy; // Valid instructions - x & y axes
    NEW_VECTOR(vi_xy);
    RESERVE(vi_xy, size);

    InstructionPtrVector vi_xyz; // Valid instructions - y, y & z axis
    NEW_VECTOR(vi_xyz);
    RESERVE(vi_xyz, size);

    solution_t lit_voxels = 0;
    for(coord_t x=begin_x; x < end_x; ++x)
    {
        // printf("%d\n", x); fflush(stdout);
        coord_t begin_y = -boot_region, end_y = boot_region+1;
        CopyValidX(&vi_xy, &vi_x, x, &begin_y, &end_y);

        if(SIZE(vi_xy) == 1) {
            lit_voxels += SweepYZ(vi_xy.begin[0]);
            continue;
        }

        for(coord_t y=begin_y; y < end_y; ++y)
        {
            // printf("%d %d\n", x, y); fflush(stdout);

            coord_t begin_z = -boot_region, end_z = boot_region+1;
            CopyValidY(&vi_xyz, &vi_xy, y, &begin_z, &end_z);

            if(SIZE(vi_xyz) == 1) {
                lit_voxels += SweepZ(vi_xyz.begin[0]);
                continue;
            }

            // const solution_t test = ComputeOverlap(&vi_xyz);
            solution_t correct = 0;

            for(coord_t z=begin_z; z <= end_z; ++z)
            {
                correct += GetVoxelStatus(&vi_xyz, z);

            }

            lit_voxels += correct;
            // printf("Correct=%lld, Test=%lld, Passed=%d\n", correct, test, correct==test);

        }
    }

    CLEAR(vi_x);
    CLEAR(vi_xy);
    CLEAR(vi_xyz);
    CLEAR(instructions);

    return lit_voxels;
}

solution_t SolvePart1(const bool is_test)
{
    return Solve(is_test, 50);
}

solution_t SolvePart2(const bool is_test)
{
    return Solve(is_test, INT_MAX);
}




DEFINE_TEST(1, 474140)
DEFINE_TEST(2, 2758514936282235)
