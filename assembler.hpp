#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include<iostream>
#include<list>

using namespace std;

#define OUTPUT_FORMAT_BIN 0xfff
#define OUTPUT_FORMAT_RAX 0xaaa
#define SECTION_CODE 0xff
#define SECTION_DATA 0xaa

class Label
{
public:
    int section_type;
    int label_address;
    int resolve_id;
    bool isExtern;
    bool isPublic;
    string label_name;
};

class AssemblerRet
{
public:
    list<Label> label_list;
    list<char> gen_code;
    list<char> gen_data;
    list<string> error_list;
	int end_address;
};

class Assembler
{
public:
    static AssemblerRet assemble(string , string);
    static string getErrorMessage( string , string , int , int );
};

#endif
