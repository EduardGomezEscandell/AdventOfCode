#include "day16.h"
#include "common/file_utils.h"
#include "common/vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

DEFINE_TEST(1, 31)
DEFINE_TEST(2, 1)

bool IsEnd(const char * it)
{
    return *it == '\0' || *it == '\n';
}

BitReader GetBitReader(const char * line)
{
    BitReader r;
    NEW_VECTOR(r.buffer);
    const size_t size = strlen(line);
    RESERVE(r.buffer, size);
    
    bool half_word = false;
    for(const char * it = line; !IsEnd(it) && !half_word; it = it+2)
    {
        half_word = IsEnd(it+1);

        word_t upper = InterpretByte(it[0]);
        word_t lower = half_word ? 0 : InterpretByte(it[1]);

        word_t word = upper << HEX_SIZE | lower;

        PUSH(r.buffer, word);
    }

    r.buffer_ptr = r.buffer.begin;
    r.mask = 1 << (WORD_SIZE-1);
    r.bitcount = 0;
    return r;
}

word_t InterpretByte(char c_repr)
{
    if(c_repr <= '9') return c_repr - '0';
    return 10 + c_repr - 'A';
}

int NextBit(BitReader * reader, word_t * bit)
{
    if(reader->mask == 0)
    {
        ++reader->buffer_ptr;
        if(reader->buffer_ptr == reader->buffer.end) return -1; // Line is over

        reader->mask = 1 << (WORD_SIZE-1);
    }

    *bit = (reader->mask & *reader->buffer_ptr) ? 1 : 0;
    reader->mask = reader->mask >> 1;
    ++reader->bitcount;

    return 0;
}

void ClearBitReader(BitReader * reader)
{
    CLEAR(reader->buffer);
    reader->buffer_ptr = NULL;
    reader->mask = 0;
    reader->bitcount = 0;
}

unsigned int ReadInt(BitReader * br, size_t n_bits)
{
    unsigned int val = 0;
    word_t bit = 0;
    for(size_t i=0; i<n_bits; ++i)
    {// This can be optimized to read multiple bits at once
        NextBit(br, &bit);
        val = (val << 1) + bit;
    }
    return val;
}

void ReadLiteral(BitReader * reader, Packet * packet)
{
    word_t flag = 1;
    packet->value = 0;

    while(flag == 1) {
        NextBit(reader, &flag);
        packet->value = (packet->value << HEX_SIZE) + ReadInt(reader, HEX_SIZE);
    }
}

void ReadSubPackets(BitReader * reader, Packet * packet)
{
    NEW_VECTOR(packet->subpackets);
    
    word_t ltype = 0;
    NextBit(reader, &ltype);

    if(ltype == 0)
    {
        const size_t n_bits = ReadInt(reader, 15);
        const size_t bit_target = reader->bitcount + n_bits;

        while(reader->bitcount < bit_target)
        {
            Packet next_packet = ReadPacket(reader);
            PUSH(packet->subpackets, next_packet);
        }

        return;
    }

    //ltype = 1

    const size_t n_packets = ReadInt(reader, 11);
    for(size_t i=0; i<n_packets; ++i)
    {
        Packet next_packet = ReadPacket(reader);
        PUSH(packet->subpackets, next_packet);
    }
}

Packet ReadPacket(BitReader * reader)
{
    Packet packet;

    packet.version = ReadInt(reader, 3);
    packet.type_id = ReadInt(reader, 3);

    if(packet.type_id == 4)
    {
        ReadLiteral(reader, &packet);
    } else {
        packet.value = 0;
        ReadSubPackets(reader, &packet);
    }

    return packet;
}

void PrintPacket(Packet * packet, size_t indentation)
{
    for(size_t i=0; i<indentation; ++i) printf(" ");
    
    printf("<Packet v=%d type=", packet->version);

    switch ( (enum PacketType) packet->type_id) {
        case ADD:  printf("ADD"); break;
        case MUL:  printf("MUL"); break;
        case MIN:  printf("MIN"); break;
        case MAX:  printf("MAX"); break;
        case LOAD: printf("LOAD"); break;
        case GT:   printf("GT"); break;
        case LT:   printf("LT"); break;
        case EQ:   printf("EQ"); break;
    }

    if(packet->type_id == 4)
    {
        printf(" data=%ld />\n", packet->value);
    }
    else
    {
        printf(" n=%ld>\n", SIZE(packet->subpackets));

        for(Packet * it=packet->subpackets.begin; it != packet->subpackets.end; ++it)
        {
            PrintPacket(it, indentation+4);
        }

        for(size_t i=0; i<indentation; ++i) printf(" ");
        printf("</Packet>\n");
    }
}

void ClearPacket(Packet * packet)
{
    if(packet->type_id == LOAD) return;
    
    for(Packet * it=packet->subpackets.begin; it != packet->subpackets.end; ++it)
    {
        ClearPacket(it);
    }
    CLEAR(packet->subpackets);
}

size_t AccumulatePacketVersions(Packet * packet)
{
    if(packet->type_id == LOAD)
    {
        return packet->version;
    }
    else
    {
        size_t version = packet->version;
        for(Packet * it=packet->subpackets.begin; it != packet->subpackets.end; ++it)
        {
            version += AccumulatePacketVersions(it);
        }
        return version;
    }
}

typedef void(*AccumulativeOperation)(long *, long);
typedef bool(*LogicalOperation)(long, long);

void Op_Sum(long * x, long y)
{
    *x += y;
}

void Op_Mul(long * x, long y)
{
    *x *= y;
}

void Op_Min(long * x, long y)
{
    *x = (*x<y) ? *x : y;
}

void Op_Max(long * x, long y)
{
    *x = (*x>y) ? *x : y;
}

bool Op_Gt(long x, long y)
{
    return x > y;
}

bool Op_Lt(long x, long y)
{
    return x < y;
}

bool Op_Eq(long x, long y)
{
    return x == y;
}

long int Operate(Packet * packet)
{
    
    enum PacketType type_id = packet->type_id;
    
    AccumulativeOperation Acc_op = NULL;
    LogicalOperation Log_op = NULL;
    long int acc = 0;

    switch(type_id)
    {
        case LOAD: return packet->value;
        case ADD: Acc_op = Op_Sum; acc = 0; break;
        case MUL: Acc_op = Op_Mul; acc = 1; break; 
        case MIN: Acc_op = Op_Min; acc = LONG_MAX; break;
        case MAX: Acc_op = Op_Max; acc = LONG_MIN; break;
        case GT: Log_op = Op_Gt; break;
        case LT: Log_op = Op_Lt; break;
        case EQ: Log_op = Op_Eq; break;
    }

    if(Acc_op != NULL)
    {
        for(Packet * it=packet->subpackets.begin; it != packet->subpackets.end; ++it)
        {
            Acc_op(&acc, Operate(it));
        }
        return acc;
    }

    if(Log_op != NULL)
    {
        long int lhs = Operate(packet->subpackets.begin);
        long int rhs = Operate(packet->subpackets.begin + 1);
        return Log_op(lhs, rhs);
    }

    fprintf(stderr, "Unreachable code in %s:%d\n", __FILE__, __LINE__);
    return 0;
    
}


solution_t SolvePart1(const bool is_test)
{
    FILE * file = GetFile(is_test, 16);

    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    if((read = getline(&line, &len, file)) == -1)
    {
        fprintf(stderr, "Failed to open file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    BitReader reader = GetBitReader(line);
    free(line);
    fclose(file);
    
    Packet root = ReadPacket(&reader);
    ClearBitReader(&reader);

    // PrintPacket(&root, 0);
    size_t ids = AccumulatePacketVersions(&root);
    ClearPacket(&root);

    return ids;
}

solution_t SolvePart2(const bool is_test)
{
    FILE * file = GetFile(is_test, 16);

    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    if((read = getline(&line, &len, file)) == -1)
    {
        fprintf(stderr, "Failed to open file");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if(is_test) read = getline(&line, &len, file);

    BitReader reader = GetBitReader(line);
    free(line);
    fclose(file);
    
    Packet root = ReadPacket(&reader);
    ClearBitReader(&reader);
    // PrintPacket(&root, 0);

    long result = Operate(&root);
    ClearPacket(&root);

    return result;
}
