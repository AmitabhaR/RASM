#ifndef LINKER_H

#define LINKER_H

#include "assembler.hpp"
#include <list>

using namespace std;

class Linker
{
public:
	static list<string> link(list<AssemblerRet> & );
};

#endif