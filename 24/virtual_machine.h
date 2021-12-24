#ifndef DAY_24_VM_H
#define DAY_24_VM_H

#include "common/vector.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

#define NREGISTERS 4
#define NARGUMENTS 2

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



address_t GetRegisterAddress(char repr);
char const * StringRepresentation(int op);

bool ReadArgument(char const * arg_begin, address_t * adress, value_t * value);
void ReadInstruction(char const * line, VirtualMachine * vm);
VirtualMachine ReadVirtualMachine(const bool is_test);

void PrintInstruction(Instruction const * instr);
void PrintVirtualMachine(VirtualMachine * vm);

void RebootVirtualMachine(VirtualMachine * vm);
void ClearVirtualMachine(VirtualMachine * vm);

void Op_INP(value_t * op1, value_t op2);
void Op_ADD(value_t * op1, value_t op2);
void Op_MUL(value_t * op1, value_t op2);
void Op_DIV(value_t * op1, value_t op2);
void Op_MOD(value_t * op1, value_t op2);
void Op_EQL(value_t * op1, value_t op2);

void RunVM(VirtualMachine *vm);

#endif
