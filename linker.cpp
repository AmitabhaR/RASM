#include "linker.hpp"
#include "first_pass.hpp"

extern Instruction instruction_set[];
extern int instruction_count;

inline Instruction * findInstructionWithOpcode(int opcode)
{
	for (int cnt = 0; cnt < instruction_count; cnt++) if (instruction_set[cnt].instruction_opcode == opcode) return &instruction_set[cnt];

	return NULL;
}

inline int bytesToInt(register char * bytes)
{
	int ret_int = 0;

	for (int c = 0; c < sizeof(int); c++) ((char *)&ret_int)[c] = bytes[c];

	return ret_int;
}

static list<Label>::iterator * findLabelInFiles(string label_name , register list<AssemblerRet> & asm_list , list<string> & error_list)
{
	int count = 0;
	register list<Label>::iterator * ret_iter = NULL;

	for (register list<AssemblerRet>::iterator cur_obj = asm_list.begin(); cur_obj != asm_list.end(); cur_obj++)
	{
		for (register list<Label>::iterator cur_label = cur_obj->label_list.begin(); cur_label != cur_obj->label_list.end(); cur_label++)
		{
			if (cur_label->label_name == label_name && !cur_label->isExtern && cur_label->isPublic)
			{
				if (count < 1)
				{
					ret_iter = new list<Label>::iterator;
					*ret_iter = cur_label;
				}

				count++;
			}
		}
	}

	if (count > 1) { error_list.push_back(Assembler::getErrorMessage("Multiple symbols found of " + label_name, "Linking", 0, 0)); return NULL; }
	else return ret_iter;
}

list<string> Linker::link(register list<AssemblerRet> & asm_list)
{
	list<string> error_list;

	for (register list<AssemblerRet>::iterator cur_obj = asm_list.begin(); cur_obj != asm_list.end(); cur_obj++)
	{
		for (register list<Label>::iterator cur_label = cur_obj->label_list.begin(); cur_label != cur_obj->label_list.end(); cur_label++)
		{
			if (cur_label->isExtern)
			{
				register list<Label>::iterator * label_iter = findLabelInFiles(cur_label->label_name, asm_list, error_list);

				if (!label_iter) { error_list.push_back(Assembler::getErrorMessage("Unresolved symbol " + cur_label->label_name, "Linking", 0, 0)); continue; }

				cur_label->label_address = (*label_iter)->label_address;
				// Make rest of workings here.

				for (register list<char>::iterator cur_code = cur_obj->gen_code.begin(); cur_code != cur_obj->gen_code.end(); cur_code++)
				{
					int cur_instruction_opcode = bytesToInt(new char[sizeof(int)]{ *cur_code, *(++cur_code), *(++cur_code), *(++cur_code) });
					register Instruction * cur_instruction = findInstructionWithOpcode(cur_instruction_opcode);
					register list<char>::iterator memory_begin = ++cur_code;

					if (!cur_instruction){ for (register int c = 0; c < 3; c++) ++cur_code; continue; }
					if (cur_instruction->instruction_type == INSTRUCTION_MEMORY || cur_instruction->instruction_type == INSTRUCTION_REGISTER_MEMORY || cur_instruction->instruction_type == INSTRUCTION_MEMORY_REGISTER | INSTRUCTION_REGISTER_MEMORY)
					{
						int memory_id = bytesToInt(new char[sizeof(int)]{ *cur_code, *(++cur_code), *(++cur_code), *(++cur_code) });

						if (cur_label->resolve_id == memory_id)
						{
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

	return error_list;
}