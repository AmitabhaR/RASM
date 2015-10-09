#ifndef _GENERATOR_H

#define _GENERATOR_H

#include "assembler.hpp"

class CodeGenerator
{
public:
	static void generate_asm_output( int , string, list<AssemblerRet> & );
};

#endif