#include "virtual_machine.h"

#include "common/file_utils.h"

address_t GetRegisterAddress(char repr)
{
    switch (repr) {
        case 'w' : return REGISTER_SEGMENT_BEGIN + 0;
        case 'x' : return REGISTER_SEGMENT_BEGIN + 1;
        case 'y' : return REGISTER_SEGMENT_BEGIN + 2;
        case 'z' : return REGISTER_SEGMENT_BEGIN + 3;
    }

    fprintf(stderr, "Unrecognized register representation: %c\n (%s:%d)", repr, __FILE__, __LINE__);
    exit(EXIT_FAILURE);
}


char const * StringRepresentation(int op)
{
    static char const * names[] = {"inp", "add", "mul", "div", "mod", "eql", "hlt", "err"};
    return names[op];
}

void PrintInstruction(Instruction const * instr)
{
    printf("%s", StringRepresentation(instr->op));
    if(instr->op == HALT) return; // No args
    printf(" %4x", instr->args[0]);
    printf(" %4x", instr->args[1]);
}

bool ReadArgument(char const * arg_begin, address_t * adress, value_t * value)
{
    if(*arg_begin >= 'a' && *arg_begin <= 'z')
    {
        *adress = GetRegisterAddress(*arg_begin);
        return false;
    }

    *value = (address_t) atoll(arg_begin);
    return true;
}

void ReadInstruction(char const * line, VirtualMachine * vm)
{
    Instruction instr;
    instr.op = ERROR;
    for(int op = 0; op <= EQL; ++op)
    {
        if(strncmp(line, StringRepresentation(op), 3) == 0)
        {
            instr.op = op;
            break;
        }
    }

    if(instr.op == ERROR)
    {
        fprintf(stderr, "Failed to parse operation (%s:%d):\n%s", __FILE__, __LINE__, line);
        exit(EXIT_FAILURE);
    }

    // Argument 1
    for(size_t arg = 0; arg < 2; ++arg)
    {
        line = strchr(line, ' ') + 1;
        value_t value = 0;
        address_t ptr = 0;
        bool is_literal = ReadArgument(line, &ptr, &value);
        
        if(is_literal)
        {
            vm->data[vm->data_ptr] = value;
            instr.args[arg] = vm->data_ptr;
            ++vm->data_ptr;
        }
        else
        {
            instr.args[arg] = ptr;
        }

        if(instr.op == INP)
        {
            instr.args[1] = vm->io_data_ptr;
            ++vm->io_data_ptr;
            break;
        }
    }

    PUSH(vm->instructions, instr);
}

VirtualMachine ReadVirtualMachine(const bool is_test)
{
    FILE * file = GetFile(is_test, 24);

    VirtualMachine vm;
    NEW_VECTOR(vm.instructions);

    for(address_t ptr=0; ptr != MEMORY_CAP; ++ptr)
    {
        vm.data[ptr] = 0;
    }
    vm.instruction_ptr = 0;
    vm.io_data_ptr = IO_SEGMENT_BEGIN;
    vm.data_ptr = DATA_SEGMENT_BEGIN;

    size_t len = 0;
    char * line = NULL;
    while(getline(&line, &len, file) != -1)
    {
        ReadInstruction(line, &vm);
    }

    Instruction halt;
    halt.op = HALT;
    PUSH(vm.instructions, halt);

    free(line);
    fclose(file);

    return vm;
}

void PrintVirtualMachine(VirtualMachine * vm)
{
    printf("Instruction pointer: %04x (", vm->instruction_ptr);
    PrintInstruction(&vm->instructions.begin[vm->instruction_ptr]);
    printf(")\n\n");

    printf("Instruction memory:\n");
    size_t i=0;
    for(Instruction const * it = vm->instructions.begin; it != vm->instructions.end; ++it)
    {
        printf("%4ld | ", i++);
        printf("%s", StringRepresentation(it->op));
        
        if(it->op == HALT)
        {
            printf("              ; End of program\n");
            continue; // No args
        }
        
        printf(" %4x", it->args[0]);
        printf(" %4x", it->args[1]);

        printf("    ; %s ", StringRepresentation(it->op));
        if(/*it->args[0] >= REGISTER_SEGMENT_BEGIN &&*/ it->args[0] < (REGISTER_SEGMENT_BEGIN+NREGISTERS))
            printf("     register %c", 'w' + (char) it->args[0]);
        else
            printf(" %14ld", vm->data[it->args[0]]);

        if(/*it->args[1] >= REGISTER_SEGMENT_BEGIN &&*/ it->args[1] < (REGISTER_SEGMENT_BEGIN+NREGISTERS))
            printf("     register %c", 'w' + (char) it->args[1]);
        else
            printf(" %14ld", vm->data[it->args[1]]);
        printf("\n");
    }

    printf("\n\nData memory:\n");


    for(address_t i=0; i != MEMORY_CAP; i+= 8u)
    {
        printf("%04x |", i);
        for(address_t offset = 0; offset < 8; ++offset)
        {
            printf(" %15ld", vm->data[i + offset]);
        }
        printf("\n");
    }
}


void Op_INP(value_t * op1, value_t op2)
{
    *op1 = op2;
}

void Op_ADD(value_t * op1, value_t op2)
{
    *op1 += op2;
}

void Op_MUL(value_t * op1, value_t op2)
{
    *op1 *= op2;
}

void Op_DIV(value_t * op1, value_t op2)
{
    *op1 /= op2;
}

void Op_MOD(value_t * op1, value_t op2)
{
    *op1 %= op2;
}

void Op_EQL(value_t * op1, value_t op2)
{
    *op1 = (*op1 == op2);
}

void RunVM(VirtualMachine *vm)
{
    Instruction * instr = &vm->instructions.begin[vm->instruction_ptr];

    while(instr->op != HALT)
    {
        instr = &vm->instructions.begin[vm->instruction_ptr];
        switch (instr->op) {
            case INP: Op_INP(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
            case ADD: Op_ADD(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
            case MUL: Op_MUL(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
            case DIV: Op_DIV(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
            case MOD: Op_MOD(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
            case EQL: Op_EQL(&vm->data[instr->args[0]], vm->data[instr->args[1]]); break;
        }
        ++vm->instruction_ptr;
    }
}

void RebootVirtualMachine(VirtualMachine * vm)
{
    vm->instruction_ptr = 0;

    // Clearing registers
    for(address_t i=REGISTER_SEGMENT_BEGIN; i<NREGISTERS; ++i)
    {
        vm->data[i] = 0;
    }
}

void ClearVirtualMachine(VirtualMachine * vm)
{
    CLEAR(vm->instructions);
}
