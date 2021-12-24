#ifndef DAY_24_H
#define DAY_24_H

#include "common/testing.h"
#include "common/vector.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

typedef int_least64_t value_t;
typedef uint_least16_t address_t;

/*
 *  Memory layout (16 bit addressing)
 *
 *  0000
 *   ..  Registers
 *  0003
 *  0004
 *  |     Reserved memory for input
 *  00FF
 *  0100
 *   |    Program memory
 *  FFFF
 */

#define REGISTER_SEGMENT_BEGIN 0
#define IO_SEGMENT_BEGIN 0x4
#define DATA_SEGMENT_BEGIN 0x30
#define MEMORY_CAP 0x100

enum OpCodes {
    INP, ADD, MUL, DIV, MOD, EQL, HALT, ERROR
};

typedef struct {
    int op;
    address_t args[2];
} Instruction;

TEMPLATE_VECTOR(Instruction) Program;

typedef struct {
    Program instructions;
    value_t data[MEMORY_CAP];
    address_t instruction_ptr;
    address_t io_data_ptr; 
    address_t data_ptr;
} VirtualMachine;



// Solving
typedef value_t solution_t; // Just in case we have to switch to long

solution_t SolvePart1(const bool is_test);
solution_t SolvePart2(const bool is_test);

DECLARE_TEST(1);
DECLARE_TEST(2);

#endif
