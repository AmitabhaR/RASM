#include <stdio.h>
#include "generator.hpp"


void CodeGenerator::generate_asm_output(register int output_type , string output_file , register list<AssemblerRet> & asm_files)
{ 
	register FILE * fptr = NULL;

	// Basic binary format implementation .
	if (output_type == OUTPUT_FORMAT_BIN)
	{
		fptr = fopen(output_file.c_str(), "w");

		for (register list<AssemblerRet>::iterator cur_asm_file = asm_files.begin(); cur_asm_file != asm_files.end(); cur_asm_file++)
		{
			// Write .text or code section.
			for (register list<char>::iterator cur_code = cur_asm_file->gen_code.begin(); cur_code != cur_asm_file->gen_code.end(); cur_code++) fputc(*cur_code, fptr);
			// Write .data or data section.
			for (register list<char>::iterator cur_data = cur_asm_file->gen_data.begin(); cur_data != cur_asm_file->gen_data.end(); cur_data++) fputc(*cur_data, fptr);
		}

		fclose(fptr);
	}
	// Add any other output format here.
}