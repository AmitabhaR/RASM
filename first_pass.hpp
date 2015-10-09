#ifndef FIRST_PASS
#define FIRST_PASS

#include<iostream>
#include<list>
#include "assembler.hpp"

using namespace std;

#define INSTRUCTION_REGISTER_REGISTER 0xf
#define INSTRUCTION_REGISTER_REGISTER_INDEX 0xffe
#define INSTRUCTION_MEMORY_REGISTER 0xcff
#define INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX 0xeff
#define INSTRUCTION_REGISTER_MEMORY 0xe
#define INSTRUCTION_MEMORY 0xa
#define INSTRUCTION_REGISTER 0xb
#define INSTRUCTION_NONE 0x0

struct Instruction
{
    string instruction_name;
    int instruction_size; // In bytes.
    int instruction_opcode;
    int instruction_type;
};

struct CPURegister
{
    string register_name;
    int register_size; // In bytes.
};

class Pass1
{
public:
    static void pass( string , int , list< list<string> > & , list<Label> & , list<string> & , int & );
};

#endif
