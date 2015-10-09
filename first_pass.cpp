#include "first_pass.hpp"

/*
	Array of all registers for assembler including their sizes in bytes.
*/

CPURegister register_flags[30] = { // Register name , size.
     {"r1", 1}, // A 1'st byte.
     {"r2", 1}, // A 2'nd byte.
     {"r3", 1}, // A 3'rd byte.
     {"r4", 1}, // A 4'th byte.
     {"r5", 2}, // A 1'st word. (1 + 2)
     {"r6", 2}, // A 2'nd word. (3 + 4)
     {"rA", 4}, // A = 4 bytes.
     {"r7", 1}, // B 1'st byte.
     {"r8", 1}, // B 2'nd byte.
     {"r9", 1}, // B 3'rd byte.
     {"r10", 1}, // B 4'th byte.
     {"r11", 2}, // B 1'st word. (7 + 8)
     {"r12", 2}, // B 2'nd word. (9 + 10)
     {"rB", 4}, // B = 4 bytes.
     {"r13", 1}, // C 1'st byte.
     {"r14", 1}, // C 2'nd byte.
     {"r15", 1}, // C 3'rd byte.
     {"r16", 1}, // C 4'th byte.
     {"r17", 2}, // C 1'st word. (13 + 14)
     {"r18", 2}, // C 2'nd word. (15 + 16)
     {"rC", 4}, // C = 4 bytes.
     {"r19", 1}, // D 1'st byte.
     {"r20", 1}, // D 2'nd byte.
     {"r21", 1}, // D 3'rd byte.
     {"r22", 1}, // D 4'th byte.
     {"r23", 2}, // D 1'st word. (19 + 20)
     {"r24", 2}, // D 2'nd word. (21 + 22)
     {"rD", 4}, // D = 4 bytes.
	 {"rbp", 4}, // bp = 4 bytes.
	 {"rsp", 4} // sp = 4 bytes.
};

/*
	Array of instuctions supported by this assembler . 
	Also stores the size in bytes and the instruction flags.
*/

Instruction instruction_set[] = {
    { "ldr" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "ldm" , 8 , 0x00000000 , INSTRUCTION_MEMORY_REGISTER | INSTRUCTION_REGISTER_MEMORY },
    { "add" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER},
    { "addi" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "addio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "sub" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER},
    { "subi" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "subio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "mul" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER},
    { "muli" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX},
    { "mulio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX},
    { "div" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER},
    { "divi" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX},
    { "divio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "jmp" , 4 , 0x00000000 , INSTRUCTION_REGISTER },
    { "call" ,8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "fret" , 4 , 0x00000000 , INSTRUCTION_NONE },
    { "cmp" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER},
    { "cmpi" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX},
    { "cmpio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "cmpm" , 8 , 0x00000000 , INSTRUCTION_REGISTER_MEMORY},
    { "lgdt" , 8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "lidt" , 8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "lpg" , 8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "shl" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "shli" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "shlio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "shr" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "shri" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "shrio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "out" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "in" , 4 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "nop" , 4 , 0x00000000 , INSTRUCTION_NONE },
    { "syscall" , 4 , 0x00000000 , INSTRUCTION_NONE },
    { "and" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "andi" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "andio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "or" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "ori" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "orio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "xor" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "xori" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "xorio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "not" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER },
    { "noti" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_INDEX },
    { "notio" , 8 , 0x00000000 , INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX },
    { "jequ" , 8 , 0x0000000 , INSTRUCTION_MEMORY },
    { "jnequ" , 8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "jg" , 8 , 0x00000000 , INSTRUCTION_MEMORY },
    { "jl" , 8 , 0x00000000 , INSTRUCTION_MEMORY }
};

int instruction_count = sizeof(instruction_set) / sizeof(Instruction); // Public variable for number of instructions.
int register_flags_count = sizeof(register_flags) / sizeof(CPURegister); // Public variable for number of registers. 
static int resolve_ids = -0xf; // Static variable for counting resolve ids for extern symbols.

/*
	Pass1::pass( ):
		Looks for labels in the token tree and calculates their address .
		Also calculates end addresses for a assembler file.
*/

void Pass1::pass(string file , int initial_address , list< list<string> > & token_tree_list , list<Label> & label_list , list<string> & error_list  , int & end_address)
{
    int cur_section = SECTION_CODE; // Flag for current section of code.
    int line_cntr = 1; // Line counter.
    int address_counter = initial_address; // Address counter.
    list<string> resolving_symbols; // Symbol to be resolved list.

	/*
		Calculate opcodes for every instructions.
	*/

	for (int cnt = 0; cnt < instruction_count;) instruction_set[cnt].instruction_opcode = 0x1000 + (++cnt * 0xffff); // Maximum register register possibilites.

	/*
		Pass through every token and calculate label addresses according to sections.
	*/

	for (std::list< std::list<string> >::iterator cur_tree = token_tree_list.begin(); cur_tree != token_tree_list.end(); cur_tree++, line_cntr++)
	{
		bool isSuccess = false; // Flag for single instruction in line.
		int tok_cntr = 1; // Token counter.

		for (std::list<string>::iterator cur_token = (*cur_tree).begin(); cur_token != (*cur_tree).end(); cur_token++, tok_cntr++)
		{
			if (!isSuccess)
			{
				goto loop_over_end; // Jump to loop over end.

			loop_over:

				isSuccess = true; // Set the flag to true.
				continue; // Skip to next iteration.

			loop_over_end:

				if (*cur_token == ".data") // Check if data section .
				{
					cur_section = SECTION_DATA; // Set section flag to data.
					goto loop_over; // Jump to a common jump.
				}
				else if (*cur_token == ".text") // Check if code section.
				{
					cur_section = SECTION_CODE; // Set section flag to code.
					goto loop_over;
				}
				else if (*cur_token == ".global" || *cur_token == ".public") // Check if public label declaration.
				{
					string label_name = *(++cur_token); // Store label name.
					bool flag = false; // Flag to find the label in the symbol table.

					/*
						Pass through every symbol in the symbol table.
					*/

					for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
					{
						if (cur_label->label_name == label_name) // Check if the label exists on the symbol table.
						{
							cur_label->isPublic = true; // Make the existing label public.
							flag = true; // Flag becomes true.
							break; 
						}
					}

					if (!flag) // If the label dosen't exist in the symbol table.
					{
						label_list.push_back({ cur_section, 0x000, 0, false, true, label_name }); // Create a new entry for the label in the symbol table.   
						resolving_symbols.push_back(label_name); // Create a resolving request for this label.
						goto loop_over; 
					}
				}
				else if (*cur_token == ".extern") // Check if extern label declaration.
				{
					string label_name = *(++cur_token); // Store label name.
					bool flag = false; // Flag to find the label in the symbol table.

					/*
						Pass through every symbol in the symbol table.
					*/

					for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
					{ 
						if (cur_label->label_name == label_name) // Check if the label exists on the symbol table.
						{
							cur_label->isExtern = true;	// Make the existing label extern.
							cur_label->resolve_id = resolve_ids; // Provide that it's resolve id.
							resolve_ids--; // Decrement the resolve id.
							flag = true; // Flag becomes true.
							break;
						}
					}

					if (!flag) 
					{ 
						label_list.push_back({ cur_section, 0x000, resolve_ids, true, false, label_name }); // Create a new entry for the label in the symbol table. 
						resolve_ids--;	// Decrement the resolve id.
						goto loop_over;
					}
				}
				else if ((*cur_token)[(*cur_token).length() - 1] == ':') // Check if label declaration.
				{
					string label_name = (*cur_token).substr(0, (*cur_token).length() - 1); // Store label name.
					bool flag = true; // Flag for finding label in symbol table.

					/*
						Pass through every symbol in the symbol table.
					*/

					for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
					{
						if (cur_label->label_name == label_name) // Check if label exists on symbol table.
						{
							if (cur_label->isPublic) // Check if the entry is public.
							{
								cur_label->section_type = cur_section; // Set the current section.
								cur_label->label_address = address_counter; // Set the value of the address counter.

								/*
									Find and remove the entry from the resolving list.
								*/

								for (list<string>::iterator cur_resolving = resolving_symbols.begin(); cur_resolving != resolving_symbols.end(); cur_resolving++) if (label_name == *cur_resolving) { resolving_symbols.erase(cur_resolving); break; } 

								goto loop_over;
							}
							else if (cur_label->isExtern) { cur_label->isExtern = false; goto loop_over; } // Check if the entry is extern then set it false.
							else
							{
								/*
									Print out error.
								*/

								error_list.push_back(Assembler::getErrorMessage(label_name + " already defined!", file, line_cntr, ++tok_cntr));
								flag = false; // Set flag to false.
								break;
							}
						}
						
					}

					if (flag) // If the label is not defined.
					{
						label_list.push_back({ cur_section, address_counter, 0, false, false, label_name }); // Create a new entry of the label in the symbol table.

						goto loop_over;
					}
				}
				else if (*cur_token == "db") // Check if byte defination.
				{
					address_counter++; // Increment address counter by sizeof a byte in bytes.
					goto loop_over;
				}
				else if (*cur_token == "dw") // Check if word defination.
				{
					address_counter += sizeof(short); // Increment address counter by sizeof a word in bytes. 
					goto loop_over;
				}
				else if (*cur_token == "dd") // Check if dword defination.
				{
					address_counter += sizeof(int); // Increment address counter by sizeof a dword in bytes.
					goto loop_over;
				}
					
				bool flag = false; // Flag to check if the token is a valid instruction.

				/*
					Pass through all the instructions and check if the current token matches
					with any one of them.
				*/

				for (int cntr = 0; cntr < instruction_count; cntr++)
				{
					if (instruction_set[cntr].instruction_name == *cur_token) // Check if current token is this instruction.
					{
						address_counter += instruction_set[cntr].instruction_size; // Increment the address counter by the bytes occupied by the instruction.
						flag = true; // Flag is now true.
						break;
					}
				}

				if (!flag) error_list.push_back(Assembler::getErrorMessage(*cur_token + " Unknow instruction!", file, line_cntr, tok_cntr)); // Print out error for unknown instruction.

				goto loop_over;
			}
		}
	}

	end_address = address_counter; // Setup the end address of the current asm file.

	/*
		Checks if any resolving is left . If any then print that 
		it is undefined in the current file.
	*/

	if (resolving_symbols.size() > 0) for(list<string>::iterator cur_label = resolving_symbols.begin( );cur_label != resolving_symbols.end( );cur_label++) error_list.push_back(Assembler::getErrorMessage("Undefined symbol " + *cur_label,file,0,0));
}
