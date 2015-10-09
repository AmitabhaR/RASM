/* RASM
   Supported Architecture - RPU
   Output Formats - Binary (bin)
                    Rax    (rax)
*/

#include <iostream>
#include <string>
#include <list>
#include <stdio.h>
#include "numbers.hpp"
#include "assembler.hpp"
#include "linker.hpp"
#include "generator.hpp"

using namespace std;

/*
	main( ) :
		Entry point of the program.
*/

int main(int argc , char* args[])
{
    if (argc < 2)
    {
        cout << "Error : No options passed!" << endl;
        return 0;
    }

    string out_file_name; // Stores output file name.
    int out_format = 0; // Stores output format.
    string out_hex_adr; // Stores a string representation of hex address .
    std::list<string> file_list; // List of files to be assembled.
	bool isInputFiles = false; // Flag for obtaining input files from command list.

    for(int cnt = 1;cnt < argc;cnt++)
    {
        string cur_str = args[cnt];

        if (cur_str == "-o") // Output File Name.
        {
            if (++cnt >= argc)
            {
                cout << "Error : Expected output file name!" << endl;
                return 0;
            }

            out_file_name = args[cnt];
        }
        else if (cur_str == "-f") // Output format.
        {
            if (++cnt >= argc)
            {
                cout << "Error : Expected output format!" << endl;
                return 0;
            }

			cur_str = args[cnt];

            if (cur_str == "bin" || cur_str == "rax") out_format = (cur_str == "bin" ) ? OUTPUT_FORMAT_BIN : OUTPUT_FORMAT_RAX;
            else
            {
                cout << "Error : Expected valid output format . Given : " << args[cnt] << endl;
                return 0;
            }
        }
        else if (cur_str == "-org") // Initial address of the program [Note : Paging is then disabled].
        {
			if (++cnt >= argc)
			{
				cout << "Error : Expected a hexa-decimal initial address!" << endl;
				return 0;
			}

			cur_str = args[cnt];

            if (Numbers::checkHex(cur_str))
            {
                out_hex_adr = cur_str;
            }
            else
            {
                cout << "Error : Expected proper hexa-decimal initial addresss . Given : " << args[cnt] << endl;
                return 0;
            }
        }
        else if (cur_str == "-i") // The files to be passed.
        {
			if (++cnt >= argc)
			{
				cout << "Error : Expected input asm files!" << endl;
				return 0;
			}

			cur_str = args[cnt];

            FILE * file = fopen(cur_str.c_str( ),"r"); // Get a handle to the file.
			isInputFiles = true;

            if (file) file_list.push_back( cur_str );
            else
            {
                cout << "Error : Expected proper file path . Given : " << args[cnt] << endl;
                return 0;
            }
        }
		else if (isInputFiles)
		{
			FILE * file = fopen(cur_str.c_str(), "r"); // Get a handle to the file.

			if (file) file_list.push_back(cur_str);
			else
			{
				cout << "Error : Expected proper file path . Given : " << args[cnt] << endl;
				return 0;
			}
		}
        else
        {
            cout << "Error : Unknown option passed " << cur_str << endl;
			return 0;
        }
    }

    std::list<AssemblerRet> asm_ret_list; // Table of symbols of each individual files.
    bool isSuccess = true; // Flag for assembling success.

    // Call assembler for pass1 and pass2.
    for(std::list<string>::iterator cur_file = file_list.begin( );cur_file != file_list.end( );cur_file++)
    {
        AssemblerRet asm_ret = Assembler::assemble(out_hex_adr,*cur_file); // Get a handle to the assembler return.

		out_hex_adr = Numbers::toHex(asm_ret.end_address + 1); // Jump to the next location after this file.

        if (asm_ret.error_list.size( ) > 0) // Check for any errors.
        {
            // Show errors.
			cout << "Error list <" << *cur_file << "> :-" << endl;
			
			for(std::list<string>::iterator cur_error = asm_ret.error_list.begin();cur_error != asm_ret.error_list.end();cur_error++) cout << *cur_error << endl; // Print out every error.

            isSuccess = false;
        }
		else asm_ret_list.push_back(asm_ret); // Add to the symbol table.
    }

    if (isSuccess)
    {
        // Loop throught all the labels and link them.
        // Generate the final bin or rax file.
		list<string> linker_errors = Linker::link(asm_ret_list); // Linker error returns.

		if (linker_errors.size() > 0) // Check if any linking errors.
		{
			cout << "Error list <Linker> :- " << endl;

			for (list<string>::iterator cur_error = linker_errors.begin(); cur_error != linker_errors.end(); cur_error++) cout << *cur_error << endl; // Print out every error.

			getchar(); // Halt.

			return 0x0;
		}
		else CodeGenerator::generate_asm_output(out_format, out_file_name, asm_ret_list); // Call our code generator for generating the final file.

		getchar(); // Halt.

        return 0x1;
    }


	getchar();

    return 0;
}
