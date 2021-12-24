#include "day24.h"
#include "24/virtual_machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MONAD_DIGITS 14
#define MAX_MONAD (1e14-1)


solution_t Test1()
{
    VirtualMachine vm = ReadVirtualMachine(true);
    
    const value_t input1 = 13;
    vm.data[IO_SEGMENT_BEGIN] = input1; // Program converts this to binary (numbers up to 16)
    
    RunVM(&vm);

    // Reconstructing
    solution_t solution1 = 0;
    for(address_t i=REGISTER_SEGMENT_BEGIN; i<NREGISTERS; ++i) {
        solution1 = solution1 << 1;
        solution1 += 1 & (vm.data[i]);
    }

    // printf("Registers at HALT:\n");
    // for(address_t i=REGISTER_SEGMENT_BEGIN; i<NREGISTERS; ++i) {
    //     printf(" %15ld", vm->data[i]);
    // }
    // printf("\n");

    RebootVirtualMachine(&vm);

    const value_t input2 = 13;
    vm.data[IO_SEGMENT_BEGIN] = input2; // Program converts this to binary (numbers up to 16)
    
    RunVM(&vm);

    // Reconstructing
    solution_t solution2 = 0;
    for(address_t i=REGISTER_SEGMENT_BEGIN; i<NREGISTERS; ++i) {
        solution2 = solution2 << 1;
        solution2 += 1 & (vm.data[i]);
    }

    return (input1 == solution1) && (input2 == solution2);

}

bool ConvertToArray(solution_t num, solution_t digits[MONAD_DIGITS])
{
    for(size_t i=0; i < MONAD_DIGITS; ++i)
    {
        digits[i] = num % 10;
        if(digits[i] == 0) return true;
        num /= 10;
    }
    return 0;
}

void LoadOntoMemory(VirtualMachine * vm, solution_t data[MONAD_DIGITS])
{
    for(size_t i=0; i<MONAD_DIGITS; ++i)
    {
        vm->data[IO_SEGMENT_BEGIN + i] = data[i];
    }
}

solution_t SolvePart1(const bool is_test)
{
    if(is_test) return Test1();

    VirtualMachine vm = ReadVirtualMachine(false);

    for(solution_t i = MAX_MONAD; i > 0; --i)
    {
        solution_t digits[MONAD_DIGITS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        bool has_zeroes = ConvertToArray(i, digits);

        if(has_zeroes)
        {
            continue;
        }

        fflush(stdout);

        RebootVirtualMachine(&vm);
        LoadOntoMemory(&vm, digits);

        RunVM(&vm);

        solution_t output = vm.data[GetRegisterAddress('w')];

        if(output == 0)
        {
            ClearVirtualMachine(&vm);
            return i;
        }
    }

    return 0;
}


solution_t SolvePart2(const bool is_test)
{
    return is_test;
}

DEFINE_TEST(1, 1)
DEFINE_TEST(2, 1)
