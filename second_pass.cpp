#include "second_pass.hpp"
#include "numbers.hpp"

extern Instruction instruction_set[]; // External reference to array of instructions.
extern CPURegister register_flags[]; // External reference to register flags.
extern int instruction_count; // External reference to instruction count.
extern int register_flags_count; // External reference to register count.

/*
	findRegister( ) :
		Takes a string register name and returns a handle to the required register.
		If not found , returns NULL.
*/

static CPURegister * findRegister(string register_name)
{
	for (int cnt = 0; cnt < register_flags_count; cnt++) if (register_flags[cnt].register_name == register_name) return &register_flags[cnt];

    return NULL;
}

/*
	getRegisterCount( ) :
		Takes a CPURegister handle and returns the register count to the caller.
		If not found , returns -1.
*/

static int getRegisterCount(CPURegister reg)
{
    for(int cnt = 0;cnt < register_flags_count;cnt++) if (register_flags[cnt].register_name == reg.register_name) return cnt;

    return -1;
}

/*
	getBytes( ) :
		Takes a value type , word and a dword and returns a byte buffer to the caller.
		If not found , returns NULL.
*/

static char * getBytes(register int t , register short num_s , register int num_i)
{
	register char * ret_ptr;

	if (t == sizeof(short)) // Check if word type.
	{
		ret_ptr = new char[2];

		memcpy(ret_ptr, &num_s, 2); // Copy the bytes to returning pointer.
	}
	else if (t = sizeof(int)) // Check if dword type.
	{
		ret_ptr = new char[4];
		
		memcpy(ret_ptr, &num_i, 4); // Copy the bytes to returning pointer.
	}

	return ret_ptr; // Return our pointer.
}

/*
	Pass2::pass( ) :
		Takes a few parameters and generates assembler codes and data section.
*/

void Pass2::pass( string file , int initial_addresss , register list< list<string> > & token_tree_list, register list<Label> & label_list , register list<string> & error_list , register list<char> & code_list , register list<char> & data_list)
{
    int cur_section = SECTION_CODE;
    int line_cntr = 1;

    for(std::list< std::list<string> >::iterator cur_tree = token_tree_list.begin();cur_tree != token_tree_list.end();cur_tree++,line_cntr++)
    {
        bool isSuccess = false;
        int tok_cntr = 1;

        for(std::list<string>::iterator cur_token = (*cur_tree).begin( );cur_token != (*cur_tree).end( );cur_token++,tok_cntr++)
        {
            if (!isSuccess)
            {
                goto loop_over_end;

                loop_over:

                    isSuccess = true;
                    continue;

                loop_over_end:

                if (*cur_token == ".data")
                {
                     cur_section = SECTION_DATA;
                     goto loop_over;
                }
                else if (*cur_token == ".text")
                {
                    cur_section = SECTION_CODE;
                    goto loop_over;
                }

                if (cur_section == SECTION_CODE)
                {
                    for(int cnt = 0;cnt < instruction_count;cnt++)
                    {
                        if (instruction_set[cnt].instruction_name == *cur_token)
                        {
                            if (instruction_set[cnt].instruction_type == INSTRUCTION_NONE)
                            {
								char * bytes = getBytes(sizeof(int),0,instruction_set[cnt].instruction_opcode);

                                for(int c = 0;c < sizeof(int);c++) code_list.push_back(bytes[c]);
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER)
                            {
                                CPURegister * reg;

                                if ((reg = findRegister(*(++cur_token))))
                                {
                                    if (reg->register_size == 4)
                                    {
										char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + getRegisterCount(*reg));
                                        
										for (int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]);
                                    }
                                    else error_list.push_back(Assembler::getErrorMessage("Expected a 4 byte register!",file,line_cntr,tok_cntr));
                                }
                                else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!",file,line_cntr,tok_cntr));
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER)
                            {
                                CPURegister * reg_dest , * reg_src;

                                if ((reg_dest = findRegister(*(++cur_token))))
                                {
									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr));

									if ((reg_src = findRegister(*(++cur_token))))
									{
										if (reg_dest->register_size != reg_src->register_size) error_list.push_back(Assembler::getErrorMessage("Expected same size register in operation!", file, line_cntr, tok_cntr));
										else if (reg_dest->register_name == reg_src->register_name) error_list.push_back(Assembler::getErrorMessage("Same registers in operation!", file, line_cntr, tok_cntr));
										else
										{
											// Generate code.
											char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount( *reg_dest ) + 1)) + (register_flags_count - getRegisterCount( *reg_src ) + 1)));

											for (int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]);
											for (int c = 0; c < sizeof(int); c++) code_list.push_back(0);
										}
									}
									else if (Numbers::checkBinary(*cur_token))
									{
										// Generate code.
										char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest));
										char * val_bytes = getBytes(sizeof(int), 0, Numbers::getBinary(*cur_token));

										if ((reg_dest->register_size == sizeof(char) && Numbers::getBinary(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getBinary(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getBinary(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr));
										
										for (int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]);
										for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
									}
									else if (Numbers::checkInteger(*cur_token))
									{
										// Generate code.
										char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest));
										char * val_bytes = getBytes(sizeof(int), 0, Numbers::getInteger(*cur_token));

										if ((reg_dest->register_size == sizeof(char) && Numbers::getInteger(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getInteger(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getInteger(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr));

										for (int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]);
										for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
									}
									else if (Numbers::checkHex(*cur_token))
									{
										// Generate code.
										char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest));
										char * val_bytes = getBytes(sizeof(int), 0, Numbers::getHex(*cur_token));

										if ((reg_dest->register_size == sizeof(char) && Numbers::getHex(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getHex(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getHex(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr));

										for (int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]);
										for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
									}
									else error_list.push_back(Assembler::getErrorMessage("Unknown operand in line!", file, line_cntr, tok_cntr));
                                }
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_MEMORY)
                            {
                                string memory_address = *(++cur_token);
                                int address = 0;
                      
                                if (Numbers::checkBinary( memory_address )) { address = Numbers::getBinary( memory_address ); goto write_instruction; }
                                else if (Numbers::checkInteger( memory_address )) { address = Numbers::getInteger( memory_address ); goto write_instruction; }
                                else if (Numbers::checkHex ( memory_address )) { address = Numbers::getHex( memory_address ); goto write_instruction; }
								else for (std::list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++) if (cur_label->label_name == memory_address) { if (cur_label->isExtern) address = cur_label->resolve_id; else address = cur_label->label_address; goto write_instruction; }

								error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr));
								goto final;
write_instruction:
								char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode);
								char * address_bytes = getBytes(sizeof(int), 0, address);

								for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
								for (int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]);
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_MEMORY)
                            {
								CPURegister * reg;

								if ((reg = findRegister(*(++cur_token))))
								{
									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr));

									string memory_address = *(++cur_token);
									int address = 0;
									
									if (Numbers::checkBinary(memory_address)) { address = Numbers::getBinary(memory_address); goto proceed_write; }
									else if (Numbers::checkInteger(memory_address)) { address = Numbers::getInteger(memory_address); goto proceed_write; }
									else if (Numbers::checkHex(memory_address)) { address = Numbers::getHex(memory_address); goto proceed_write; }
									else for (std::list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++) if (cur_label->label_name == memory_address) { if (cur_label->isExtern) address = cur_label->resolve_id; else address = cur_label->label_address; goto proceed_write; }

									error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr));
									goto final;
proceed_write:
									char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode);
									char * address_bytes = getBytes(sizeof(int), 0, address);

									for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
									for (int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]);
								}
								else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!", file, line_cntr, tok_cntr));
                            }
							/*else if (instruction_set[cnt].instruction_type == INSTRUCTION_MEMORY_REGISTER)
							{
								string memory_address = *(++cur_token);
								CPURegister * reg = findRegister(*(++cur_token));
								int address = 0;

								if (Numbers::checkBinary(memory_address)) { address = Numbers::getBinary(memory_address); if (!reg) error_list.push_back(Assembler::getErrorMessage("Expected a register in line!", file, line_cntr, tok_cntr)); goto print_out; }
								else if (Numbers::checkInteger(memory_address)) { address = Numbers::getInteger(memory_address); if (!reg) error_list.push_back(Assembler::getErrorMessage("Expected a register in line!", file, line_cntr, tok_cntr)); goto print_out; }
								else if (Numbers::checkHex(memory_address)) { address = Numbers::getHex(memory_address); if (!reg) error_list.push_back(Assembler::getErrorMessage("Expected a register in line!", file, line_cntr, tok_cntr)); goto print_out; }
								else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++) if (cur_label->label_name == memory_address) { address = cur_label->label_address; if (!reg) error_list.push_back(Assembler::getErrorMessage("Expected a register in line!", file, line_cntr, tok_cntr)); goto print_out; }

								error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr));
								goto final;
print_out:
								code_list.push_back((instruction_set[cnt].instruction_opcode + getRegisterCount(*reg)) << 8);
								code_list.push_back((instruction_set[cnt].instruction_opcode + getRegisterCount(*reg)) & 0xff);
								code_list.push_back((address >> 24));
								code_list.push_back((address >> 16) & 0xff);
								code_list.push_back((address >> 8) & 0xff);
								code_list.push_back(address & 0xff);
							} */
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER_INDEX || instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX)
                            {
								CPURegister * reg_dest , * reg_src ;

								if ((reg_dest = findRegister(*(++cur_token))))
								{
									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr));

									if ((reg_src = findRegister(*(++cur_token))))
									{
										if (reg_src->register_size == 4)
										{
											if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected another seperator in line!", file, line_cntr, tok_cntr));

											if (Numbers::checkBinary(*(++cur_token)))
											{
												// Generate code.
												char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1)));
												char * val_bytes = getBytes(sizeof(int), 0, Numbers::getBinary(*cur_token));

												for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
												for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
											}
											else if (Numbers::checkInteger(*cur_token))
											{
												// Generate code.
												char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1)));
												char * val_bytes = getBytes(sizeof(int), 0, Numbers::getInteger(*cur_token));

												for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
												for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
											}
											else if (Numbers::checkHex(*cur_token))
											{
												// Generate code.
												char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1)));
												char * val_bytes = getBytes(sizeof(int), 0, Numbers::getHex(*cur_token));

												for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
												for (int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]);
											}
											else error_list.push_back(Assembler::getErrorMessage("Expected a index value!", file, line_cntr, tok_cntr));
										}
										else error_list.push_back(Assembler::getErrorMessage("Expected a 4 bytes base register!", file, line_cntr, tok_cntr));
									}
									else error_list.push_back(Assembler::getErrorMessage("Expected a base register!", file, line_cntr, tok_cntr));
								}
								else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!", file, line_cntr, tok_cntr));
                            }
							else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_MEMORY | INSTRUCTION_MEMORY_REGISTER)
							{
								CPURegister * reg;
								string memory_address = *(++cur_token);
								int address = 0;
								bool isRM = false;

								if ((reg = findRegister(memory_address)))
								{
									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr));

									memory_address = *(++cur_token);

									isRM = true;
								}
								
								if (Numbers::checkBinary(memory_address))
								{
									address = Numbers::getBinary(memory_address);
									
								    if (!isRM)	if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!",file,line_cntr,tok_cntr));

									goto print_write;
								}
								else if (Numbers::checkInteger(memory_address))
								{
									address = Numbers::getInteger(memory_address);

									if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr));

									goto print_write;
								}
								else if (Numbers::checkHex(memory_address))
								{
									address = Numbers::getHex(memory_address);

									if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr));

									goto print_write;
								}
								else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
										if (cur_label->label_name == *cur_token)
										{
											if (cur_label->isExtern) address = cur_label->resolve_id; else address = cur_label->label_address;

											if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr));

											goto print_write;
										}

								error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr));
								goto final;

print_write:
								char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + isRM * register_flags_count + getRegisterCount(*reg));
								char * address_bytes = getBytes(sizeof(int), 0, address);

								for (int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]);
								for (int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]);
							}
final:

                            break;
                        }
                    }
                }
                else
                {
					char * data_ptr;
					int data_value = 0;

					if (*cur_token == "db")
					{
						if (Numbers::checkBinary(*(++cur_token)))
						{
							data_value = Numbers::getBinary(*cur_token);

							if (data_value <= 0xff)
								data_list.push_back(data_value);
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));
							goto final_data;
						}
						else if (Numbers::checkInteger(*cur_token))
						{
							data_value = Numbers::getInteger(*cur_token);

							if (data_value <= 0xff)
								data_list.push_back(data_value);
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));
							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token))
						{
							data_value = Numbers::getHex(*cur_token);

							if (data_value <= 0xff)
								data_list.push_back(data_value);
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));
							goto final_data;
						}
						else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token)
							{
								if (cur_label->label_address <= 0xff)
									if (!cur_label->isExtern) data_list.push_back(cur_label->label_address);
										else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr));
								else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr));
					}
					else if (*cur_token == "dw")
					{
						if (Numbers::checkBinary(*(++cur_token)))
						{
							data_value = Numbers::getBinary(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value);

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else if (Numbers::checkInteger(*cur_token))
						{
							data_value = Numbers::getInteger(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value); 

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token))
						{
							data_value = Numbers::getHex(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value);

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token)
							{
								if (cur_label->label_address <= 0xffff)
								{
									if (!cur_label->isExtern)
									{
										data_ptr = getBytes(sizeof(short), 0, cur_label->label_address);

										for (int c = 0; c < sizeof(short); c++) data_list.push_back(data_ptr[c]);
									}
									else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr));
								}
								else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr));

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr));
					}
					else if (*cur_token == "dd")
					{
						if (Numbers::checkBinary(*(++cur_token)))
						{
							data_value = Numbers::getBinary(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else if (Numbers::checkInteger(*cur_token))
						{
							data_value = Numbers::getInteger(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token))
						{
							data_value = Numbers::getHex(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) data_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr));

							goto final_data;
						}
						else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token)
							{
								if (cur_label->label_address <= 0xffffffff)
								{
									if (!cur_label->isExtern)
									{
										data_ptr = getBytes(sizeof(int), 0, cur_label->label_address);

										for (int c = 0; c < sizeof(int); c++) data_list.push_back(data_ptr[c]);
									}
									else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr));
								}
								else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr));

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr));
					}
				final_data:;
                }
            }
			else
			{
				/*
					Print out error.
				*/

				error_list.push_back(Assembler::getErrorMessage("Only one instruction allow in one line!", file, line_cntr, tok_cntr));
				break;
			}
        }
    }
}
