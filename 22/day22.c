#include "day22.h"

#include "common/file_utils.h"
#include "common/vector.h"
#include "common/math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

coord_t GetStartX(Instruction const * it) { return it->x0;}
coord_t GetFinishX(Instruction const * it) { return it->x1;}

coord_t GetStartY(Instruction const * it) { return it->y0;}
coord_t GetFinishY(Instruction const * it) { return it->y1;}

coord_t GetStartZ(Instruction const * it) { return it->z0;}
coord_t GetFinishZ(Instruction const * it) { return it->z1;}

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
    printf("[%ld..%ld, %ld..%ld, %ld..%ld](%d)\n", instr->x0, instr->x1, instr->y0, instr->y1, instr->z0, instr->z1, instr->active);
}

InstructionPtrVector CreatePtrVector(InstructionVector reference, coord_t *min_x, coord_t *max_x)
{
    InstructionPtrVector out;
    NEW_VECTOR(out);
    RESERVE(out, SIZE(reference));

    const coord_t min_x_0 = *min_x;
    const coord_t max_x_0 = *max_x;

    *min_x =  LONG_MAX;
    *max_x = -LONG_MAX;

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

void CopyValidInstructions(
    InstructionPtrVector * out,
    InstructionPtrVector const * in,
    coord_t coord,
    coord_t * min_next,
    coord_t * max_next,
    Getter Start,
    Getter Finish,
    Getter StartNext,
    Getter FinishNext)
{
    out->end = out->begin; // Emptying without releasing memory

    const coord_t min_next_0 = *min_next;
    const coord_t max_next_0 = *max_next;

    *min_next =  LONG_MAX;
    *max_next = -LONG_MAX;

    bool none_write = true;

    for(Instruction ** it = in->begin; it != in->end; ++it)
    {
        if(Start(*it) > coord || coord >= Finish(*it)) continue;

        PUSH(*out, *it);

        *min_next = MIN(*min_next, StartNext(*it));
        *max_next = MAX(*max_next, FinishNext(*it));

        if((*it)->active) none_write = false;
    }

    *min_next = MAX(*min_next, min_next_0);
    *max_next = MIN(*max_next, max_next_0);

    if(none_write)
    {
        *min_next = 0;
        *max_next = 0;
    }
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

        // Fully covering segment
        // ------oooo----- initial state
        // -----xxxxxxxx-- operation
        // --------------- result
        // ----------xxx-- remainder
        if(start <= it->start && finish >= it->finish)
        {
            REMOVE(*segments, Segment, it);
            continue;
        }

        // Right-intersect
        //  ------ooooo------- initial state
        //  ---------xxxxx---- operation
        //
        //  ------ooo--------- result
        //  -----------xxx---- remainder may intersect next segment
        if(start >= it->start && start <= it->finish)
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
        if(start <= it->start && finish >= it->start)
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

        // Fully covering segment
        // ------oooo----- initial state
        // -----oooooooo-- operation
        // --------------- result
        // -----oooooooo-- remainder
        if(start <= it->start && finish >= it->finish)
        {
            REMOVE(*segments, Segment, it);

            // Repairing possible overlaps:
            //---------ooooooo---ooo----- it, next
            // -----ooooooooooooooooo---- operation
            //------ooooooooooooooooo---- result
            for(Segment * next = it + 1; next != segments->end; ++next)
            {
                if(it->finish < next->start) break; // No intersection

                it->finish = MAX(it->finish, next->finish);

                REMOVE(*segments, Segment, next);
            }


            return;
        }

        // Right-intersect
        //  ---ooooo-------- initial state
        //  ------ooooo----- operation
        //
        //  ----ooooooo----- result may intersect next segment!
        if(start >= it->start && finish >= it->finish)
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

SegmentVector ComputeRelevantSegment(
    InstructionPtrVector * instructions,
    Getter Start,
    Getter Finish)
{
    SegmentVector segments;
    NEW_VECTOR(segments);

    for(Instruction ** it = instructions->begin; it != instructions->end; ++it)
    {
        if((*it)->active) InsertActiveOverlap(&segments, Start(*it), Finish(*it));
        else            InsertInactiveOverlap(&segments, Start(*it), Finish(*it));
    }

    return segments;
}


solution_t ComputeTotalLength(SegmentVector * segments)
{
    solution_t length = 0;
    for(Segment * it = segments->begin; it != segments->end; ++it)
    {
        length += it->finish - it->start;
    }
    return length;
}




solution_t Solve(const bool is_test, coord_t boot_region)
{
    if(!is_test) return 0;

    // Trims
    InstructionVector trims;
    NEW_VECTOR(trims);

    Instruction left_trim =  {0,0,0,0,-LONG_MAX,     -boot_region, false};
    Instruction right_trim = {0,0,0,0, boot_region+1,    LONG_MAX,    false};

    PUSH(trims, left_trim);
    PUSH(trims, right_trim);

    // Instructions
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
        CopyValidInstructions(&vi_xy, &vi_x, x, &begin_y, &end_y, GetStartX, GetFinishX, GetStartY, GetFinishY);

        if(SIZE(vi_xy) == 1) {
            lit_voxels += SweepYZ(vi_xy.begin[0]);
            continue;
        }

        for(coord_t y=begin_y; y < end_y; ++y)
        {
            coord_t begin_z = -boot_region, end_z = boot_region+1;
            CopyValidInstructions(&vi_xyz, &vi_xy, y, &begin_z, &end_z, GetStartY, GetFinishY, GetStartZ, GetFinishZ);

            PUSH(vi_xyz, trims.begin);
            PUSH(vi_xyz, trims.begin+1);

            SegmentVector segments_Z = ComputeRelevantSegment(&vi_xyz, GetStartZ, GetFinishZ);
            lit_voxels += ComputeTotalLength(&segments_Z);
            CLEAR(segments_Z);
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
  return is_test;//Solve(is_test, LONG_MAX-1);
}


DEFINE_TEST(1, 474140)
DEFINE_TEST(2, 2758514936282235)
