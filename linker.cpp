#include "linker.hpp"
#include "first_pass.hpp"

extern Instruction instruction_set[]; // External reference to instruction set.
extern int instruction_count; // External reference to instruction count.

/*
	findInstructionWithOpcode( ) :
		Takes a opcode and returns a instruction handle to the caller.
*/

static Instruction * findInstructionWithOpcode(register int opcode)
{
	for (register int cnt = 0; cnt < instruction_count; cnt++)
		if (cnt + 1 < instruction_count)
			if ((instruction_set[cnt].instruction_opcode >= opcode) && (opcode < instruction_set[cnt + 1].instruction_opcode)) return &instruction_set[cnt];
			else;
		else if (instruction_set[cnt].instruction_opcode >= opcode) return &instruction_set[cnt];

	return NULL;
}

/*
	bytesToInt( ) :
		Takes a byte buffer and converts that to a integer and returns to the caller.
*/

static int bytesToInt(register char * bytes)
{
	register int ret_int = 0;

	for (register int c = 0; c < sizeof(int); c++) ((char *)&ret_int)[c] = bytes[c];

	return ret_int;
}

/*
	findLabelInFiles( ) :
		Takes a label name and finds its entry in symbol tables of all other files and if found,
		returns a handle to the label otherwise returns NULL.
*/

static list<Label>::iterator * findLabelInFiles(string label_name , register list<AssemblerRet> & asm_list , register list<string> & error_list)
{
	register int count = 0; // Counter for labels. 
	register list<Label>::iterator * ret_iter = NULL;

	for (register list<AssemblerRet>::iterator cur_obj = asm_list.begin(); cur_obj != asm_list.end(); cur_obj++)
	{
		for (register list<Label>::iterator cur_label = cur_obj->label_list.begin(); cur_label != cur_obj->label_list.end(); cur_label++)
		{
			if (cur_label->label_name == label_name && !cur_label->isExtern && cur_label->isPublic) // If the label is found with some conditions.
			{
				if (count < 1)
				{
					ret_iter = new list<Label>::iterator; // Allocate a new handle for the iterator.
					*ret_iter = cur_label; // Set the value of it with the handle of this label.
				}

				count++; // Increment the counter.
			}
		}
	}

	if (count > 1) 
	{ 
		/*
			Print out error.
		*/

		error_list.push_back(Assembler::getErrorMessage("Multiple symbols found of " + label_name, "Linking", 0, 0)); 
		
		return (list<Label>::iterator *) -1; 
	}
	else return ret_iter;
}

/*
	Linker::link( ) :
		Takes a list of asm generated files and links external references
		and returns a error list.
*/

list<string> Linker::link(register list<AssemblerRet> & asm_list)
{
	list<string> error_list; // Error list.

	for (register list<AssemblerRet>::iterator cur_obj = asm_list.begin(); cur_obj != asm_list.end(); cur_obj++)
	{
		for (register list<Label>::iterator cur_label = cur_obj->label_list.begin(); cur_label != cur_obj->label_list.end(); cur_label++) // Go through all the labels.
		{
			if (cur_label->isExtern) // Check if the label is extern.
			{
				register list<Label>::iterator * label_iter = findLabelInFiles(cur_label->label_name, asm_list, error_list); // Find a pointer to the label in another file.

				if (!label_iter) // Is the label not found.
				{ 
					error_list.push_back(Assembler::getErrorMessage("Unresolved symbol " + cur_label->label_name, "Linking", 0, 0)); 
					continue; 
				}
				else if ((list<Label>::iterator *) label_iter < 0) continue; // There are multiple references for this label found.

				cur_label->label_address = (*label_iter)->label_address; // Store the reference address.
				
				/*
					Go through all the generated codes and replace all the resolving ids
					with the actual address.
				*/

				for (register list<char>::iterator cur_code = cur_obj->gen_code.begin(); cur_code != cur_obj->gen_code.end(); cur_code++)
				{
					register int cur_instruction_opcode = bytesToInt(new char[sizeof(int)]{ *cur_code, *(++cur_code), *(++cur_code), *(++cur_code) }); // Current opcode.
					register Instruction * cur_instruction = findInstructionWithOpcode(cur_instruction_opcode); // Handle to current instruction.
					register list<char>::iterator memory_begin; // Memory address pointer begin.

					//if (!cur_instruction){ for (register int c = 0; c < 3; c++) ++cur_code; continue; } // Skip this instruction.
					if (!cur_instruction) continue; // Skip this instruction.
					if ((cur_instruction->instruction_type == INSTRUCTION_MEMORY) || (cur_instruction->instruction_type == INSTRUCTION_REGISTER_MEMORY) || (cur_instruction->instruction_type == (INSTRUCTION_MEMORY_REGISTER | INSTRUCTION_REGISTER_MEMORY)))
					{
						memory_begin = ++cur_code; // Setup the begin address.
						register int memory_id = bytesToInt(new char[sizeof(int)]{ *cur_code, *(++cur_code), *(++cur_code), *(++cur_code) }); // Get the resolve id.

						if (cur_label->resolve_id == memory_id) // Check if the resolve id matches.
						{
							/*
								Replace the id with the address in the address pointer.
							*/

							*memory_begin = ((char *)&cur_label->label_address)[0];
							*(++memory_begin) = ((char *)&cur_label->label_address)[1];
							*(++memory_begin) = ((char *)&cur_label->label_address)[2];
							*(++memory_begin) = ((char *)&cur_label->label_address)[3];
						}
					}
				}
			}
		}
	}

	return error_list; // Return the list.
}