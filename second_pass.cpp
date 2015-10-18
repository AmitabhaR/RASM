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
	for (register int cnt = 0; cnt < register_flags_count; cnt++) if (register_flags[cnt].register_name == register_name) return &register_flags[cnt];

    return NULL;
}

/*
	getRegisterCount( ) :
		Takes a CPURegister handle and returns the register count to the caller.
		If not found , returns -1.
*/

static int getRegisterCount(CPURegister reg)
{
    for(register int cnt = 0;cnt < register_flags_count;cnt++) if (register_flags[cnt].register_name == reg.register_name) return cnt;

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

void Pass2::pass( string file , register list< list<string> > & token_tree_list, register list<Label> & label_list , register list<string> & error_list , register list<char> & code_list , register list<char> & data_list)
{
    register int cur_section = SECTION_CODE; // Current section.
    register int line_cntr = 1; // Line counter.

    for(register std::list< std::list<string> >::iterator cur_tree = token_tree_list.begin();cur_tree != token_tree_list.end();cur_tree++,line_cntr++)
    {
        register bool isSuccess = false; // Flag to check single instruction.
        register int tok_cntr = 1; // Token counter.

        for(register std::list<string>::iterator cur_token = (*cur_tree).begin( );cur_token != (*cur_tree).end( );cur_token++,tok_cntr++)
        {
            if (!isSuccess) // If this is the first instruction.
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

                if (cur_section == SECTION_CODE) // Check if code section.
                    for(register int cnt = 0;cnt < instruction_count;cnt++)
                    {
                        if (instruction_set[cnt].instruction_name == *cur_token) // If the instruction is found.
                        {
                            if (instruction_set[cnt].instruction_type == INSTRUCTION_NONE) // Check if no operand instruction.
                            {
								register char * bytes = getBytes(sizeof(int),0,instruction_set[cnt].instruction_opcode); // Convert the opcode to bytes.

                                for(register int c = 0;c < sizeof(int);c++) code_list.push_back(bytes[c]); // Write the bytes in the code table.
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER) // Check if register-only instruction.
                            {
                                register CPURegister * reg; // Pointer to store handle to register.

								tok_cntr++; // Increment our token counter.

                                if ((reg = findRegister(*(++cur_token)))) // Check if register is found.
                                {
                                    if (reg->register_size == 4) // Check if it's size is 4 bytes.
                                    {
										register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + getRegisterCount(*reg)); // Calculate the opcode and convert it into bytes.
                                        
										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the bytes in the code table.
                                    }
                                    else error_list.push_back(Assembler::getErrorMessage("Expected a 4 byte register!",file,line_cntr,tok_cntr)); // Print out error.
                                }
                                else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!",file,line_cntr,tok_cntr)); // Print out error.
                            }
							else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_ANY)
							{
								register CPURegister * reg; // Pointer to store handle to register.

								tok_cntr++; // Increment our token counter.

								if ((reg = findRegister(*(++cur_token)))) // Check if register is found.
								{
									register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + getRegisterCount(*reg)); // Calculate the opcode and convert it into bytes.

									for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the bytes in the code table.
								}
								else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!", file, line_cntr, tok_cntr)); // Print out error.
							}
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER) // Check if register-register instruction.
                            {
                                register CPURegister * reg_dest , * reg_src; // Pointer to store handle to register.
								 
								tok_cntr++; // Increment our token counter.

                                if ((reg_dest = findRegister(*(++cur_token)))) // Check if register is found.
                                {
									tok_cntr++; // Increment our token counter.

									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr)); // Print out error if not comma.

									tok_cntr++; // Increment our token counter.

									if ((reg_src = findRegister(*(++cur_token))))
									{
										if (reg_dest->register_size != reg_src->register_size) error_list.push_back(Assembler::getErrorMessage("Expected same size register in operation!", file, line_cntr, tok_cntr)); // Print out error.
										else if (reg_dest->register_name == reg_src->register_name) error_list.push_back(Assembler::getErrorMessage("Same registers in operation!", file, line_cntr, tok_cntr)); // Print out error.
										else
										{
											// Generate code.
											register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount( *reg_dest ) + 1)) + (register_flags_count - getRegisterCount( *reg_src ) + 1))); // Calculate the opcode and convert it into bytes.

											for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the opcode bytes.
											for (register int c = 0; c < sizeof(int); c++) code_list.push_back(0); // Write out some zeros.
										} 
									}
									else if (Numbers::checkBinary(*cur_token))
									{
										// Generate code.
										register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest)); // Calculate and covert opcode into bytes.
										register char * val_bytes = getBytes(sizeof(int), 0, Numbers::getBinary(*cur_token)); // Convert the value into bytes.

										if ((reg_dest->register_size == sizeof(char) && Numbers::getBinary(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getBinary(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getBinary(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr)); // Print out error.
										
										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the opcode bytes.
										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write the value bytes.
									}
									else if (Numbers::checkInteger(*cur_token))
									{
										// Generate code.
										register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest)); // Calculate and convert opcode into bytes.
										register char * val_bytes = getBytes(sizeof(int), 0, Numbers::getInteger(*cur_token)); // Conver the value into bytes.

										if ((reg_dest->register_size == sizeof(char) && Numbers::getInteger(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getInteger(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getInteger(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr)); // Print out error.

										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the opcode bytes.
										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write the value bytes.
									}
									else if (Numbers::checkHex(*cur_token))
									{
										// Generate code.
										register char * bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + register_flags_count * (register_flags_count - 3) + getRegisterCount(*reg_dest)); // Calculate and convert the opcode into bytes.
										register char * val_bytes = getBytes(sizeof(int), 0, Numbers::getHex(*cur_token)); // Convert the value into bytes.

										if ((reg_dest->register_size == sizeof(char) && Numbers::getHex(*cur_token) > 0xff) || (reg_dest->register_size == sizeof(short) && Numbers::getHex(*cur_token) > 0xffff) || (reg_dest->register_size == sizeof(int) && Numbers::getHex(*cur_token) > 0xffffffff)) error_list.push_back(Assembler::getErrorMessage("Register data overflow!", file, line_cntr, tok_cntr)); // Print out error.

										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(bytes[c]); // Write the opcode bytes.
										for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write the value bytes.
									}
									else error_list.push_back(Assembler::getErrorMessage("Unknown operand in line!", file, line_cntr, tok_cntr)); // Print out error.
                                }
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_MEMORY)
                            {
                                string memory_address = *(++cur_token); // Store handle to memory address.
                                register int address = 0;
                      
								tok_cntr++; // Increment our token counter.

                                if (Numbers::checkBinary( memory_address )) 
								{ 
									address = Numbers::getBinary( memory_address ); // Get the address in integer form. 
									goto write_instruction; // Jump to write the instruction.
								}
                                else if (Numbers::checkInteger( memory_address ))  
								{ 
									address = Numbers::getInteger( memory_address ); // Get the address in integer form. 
									goto write_instruction; 
								}
                                else if (Numbers::checkHex ( memory_address )) 
								{ 
									address = Numbers::getHex( memory_address );  // Get the address in integer form.
									goto write_instruction; 
								}
								else for (register std::list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++) 
										if (cur_label->label_name == memory_address) // Check if the label is found.
										{ 
												if (cur_label->isExtern) address = cur_label->resolve_id; // Replace the resolve id to address.
												else address = cur_label->label_address;  // Get the address of the label.
												goto write_instruction; 
										}

								error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr)); // Print out error.
								goto final;
write_instruction:
								register char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode); // Convert the instruction to bytes.
								register char * address_bytes = getBytes(sizeof(int), 0, address); // Convert the address to bytes.

								for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write the opcode to the code list.
								for (register int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]); // Write the address to the code list.
                            }
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_MEMORY)
                            {
								register CPURegister * reg; // Pointer to the register.

								tok_cntr++; // Increment the token counter.

								if ((reg = findRegister(*(++cur_token)))) // Check if the register is found.
								{
									tok_cntr++; // Increment the token counter.

									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr)); // Print out error.

									string memory_address = *(++cur_token);
									register int address = 0;
									
									tok_cntr++; // Increment the token counter.

									if (Numbers::checkBinary(memory_address)) 
									{ 
										address = Numbers::getBinary(memory_address); 
										goto proceed_write; 
									}
									else if (Numbers::checkInteger(memory_address)) 
									{ 
										address = Numbers::getInteger(memory_address); 
										goto proceed_write; 
									}
									else if (Numbers::checkHex(memory_address)) 
									{ 
										address = Numbers::getHex(memory_address); 
										goto proceed_write; 
									}
									else for (register std::list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++) 
											if (cur_label->label_name == memory_address) // Check if label is found.
											{ 
												if (cur_label->isExtern) address = cur_label->resolve_id; // Replace the address with resolve id.
												else address = cur_label->label_address; // Get the address of the label.
												goto proceed_write; 
											}

									error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr)); // Print out error.
									goto final;
proceed_write:
									register char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + getRegisterCount(*reg)); // Calculate and convert opcode into bytes.
									register char * address_bytes = getBytes(sizeof(int), 0, address); // Convert the address into bytes.

									for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write out the opcode to code list.
									for (register int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]); // Write out the address to code list.
								}
								else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!", file, line_cntr, tok_cntr)); // Print out error.
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
                            else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER_INDEX || instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_REGISTER_OPPOSITE_INDEX) // Check if index type instruction.
                            {
								register CPURegister * reg_dest , * reg_src ; // Pointer to store register handle.

								tok_cntr++; // Increment the counter.

								if ((reg_dest = findRegister(*(++cur_token)))) // Check if register is found.
								{
									tok_cntr++; // Increment the counter.

									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr)); // Print out error.

									tok_cntr++; // Increment the counter.

									if ((reg_src = findRegister(*(++cur_token)))) // Check if register is found.
									{
										if (reg_src->register_size == 4) // Check if four bytes register.
										{
											tok_cntr++; // Increment the counter.

											if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected another seperator in line!", file, line_cntr, tok_cntr)); // Print out error.

											tok_cntr++; // Increment the counter.

											register char * instruction_bytes, *val_bytes; // Pointers to store byte buffers.

											if (Numbers::checkBinary(*(++cur_token))) // Check if binary data. 
											{
												// Generate code.
												instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1))); // Calculate and convert the opcode into bytes.
												val_bytes = getBytes(sizeof(int), 0, Numbers::getBinary(*cur_token)); // Convert the value into bytes.

												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write out the opcode bytes.
												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write out the value bytes.
											}
											else if (Numbers::checkInteger(*cur_token))
											{
												// Generate code.
												instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1))); // Calculate and convert opcode into bytes.
												val_bytes = getBytes(sizeof(int), 0, Numbers::getInteger(*cur_token)); // Convert the value into bytes.

												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write out the opcode bytes.
												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write out the value bytes.
											}
											else if (Numbers::checkHex(*cur_token))
											{
												// Generate code.
												instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + ((register_flags_count * (getRegisterCount(*reg_dest) + 1)) + (register_flags_count - getRegisterCount(*reg_src) + 1))); // Calculate and convert opcode into bytes.
												val_bytes = getBytes(sizeof(int), 0, Numbers::getHex(*cur_token)); // Convert the value into bytes.

												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write out the opcode bytes.
												for (register int c = 0; c < sizeof(int); c++) code_list.push_back(val_bytes[c]); // Write out the value bytes.
											}
											else error_list.push_back(Assembler::getErrorMessage("Expected a index value!", file, line_cntr, tok_cntr)); // Print out error.
										}
										else error_list.push_back(Assembler::getErrorMessage("Expected a 4 bytes base register!", file, line_cntr, tok_cntr)); // Print out error.
									}
									else error_list.push_back(Assembler::getErrorMessage("Expected a base register!", file, line_cntr, tok_cntr)); // Print out error.
								}
								else error_list.push_back(Assembler::getErrorMessage("Unknown register in this line!", file, line_cntr, tok_cntr)); // Print out error.
                            }
							else if (instruction_set[cnt].instruction_type == INSTRUCTION_REGISTER_MEMORY | INSTRUCTION_MEMORY_REGISTER) // Check if register-memory or memory-register type instruction.
							{
								tok_cntr++; // Increment the counter.

								register CPURegister * reg;
								string memory_address = *(++cur_token);
								register int address = 0;
								register bool isRM = false;

								if ((reg = findRegister(memory_address))) // Check if register is found.
								{
									tok_cntr++; // Increment the counter.

									if (*(++cur_token) != ",") error_list.push_back(Assembler::getErrorMessage("Expected seperator in line!", file, line_cntr, tok_cntr)); // Print out error.
									
									tok_cntr++; // Increment the counter.

									memory_address = *(++cur_token); // Obtain the memory address or next operand.

									isRM = true; // Flag becomes true.
								}
								
								if (Numbers::checkBinary(memory_address)) // Check if binary address.
								{
									address = Numbers::getBinary(memory_address); // Convert it to integer form.
									
								    if (!isRM)	if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!",file,line_cntr,tok_cntr)); // Print out error.

									goto print_write;
								}
								else if (Numbers::checkInteger(memory_address)) // Check if integer address.
								{
									address = Numbers::getInteger(memory_address); // Convert it to integer form.

									if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr)); // Print out error.

									goto print_write;
								}
								else if (Numbers::checkHex(memory_address)) // Check if hex address.
								{
									address = Numbers::getHex(memory_address); // Convert it to integer form.

									if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr)); // Print out error.

									goto print_write;
								}
								else for (list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
										if (cur_label->label_name == *cur_token) // Is the label found.
										{
											if (cur_label->isExtern) address = cur_label->resolve_id; // Replace the address to resolve id.
											else address = cur_label->label_address; // Set the address of the label.

											if (!isRM) if (!(reg = findRegister(*(++cur_token)))) error_list.push_back(Assembler::getErrorMessage("Expected a register in the line!", file, line_cntr, tok_cntr)); // Print out error.

											goto print_write;
										}

								error_list.push_back(Assembler::getErrorMessage("Unknown label given " + memory_address + " at line", file, line_cntr, tok_cntr)); // Print out error.
								goto final;

print_write:
								register char * instruction_bytes = getBytes(sizeof(int), 0, instruction_set[cnt].instruction_opcode + isRM * register_flags_count + getRegisterCount(*reg)); // Calculate and convert opcode into bytes.
								register char * address_bytes = getBytes(sizeof(int), 0, address); // Convert the address into bytes.

								for (register int c = 0; c < sizeof(int); c++) code_list.push_back(instruction_bytes[c]); // Write out the opcode bytes.
								for (register int c = 0; c < sizeof(int); c++) code_list.push_back(address_bytes[c]); // Write out the address bytes.
							}
final:

                            break; // Jump to next line.
                        }
                    }
                
					register char * data_ptr; // Data buffer.
					register int data_value = 0; // Data value.

					if (*cur_token == "db")
					{
						tok_cntr++; // Increment the counter.

						if (Numbers::checkBinary(*(++cur_token))) // Check if binary type.
						{
							data_value = Numbers::getBinary(*cur_token); // Convert it to integer form.

							if (data_value <= 0xff) // Check the data bounds.
								if (cur_section == SECTION_DATA) data_list.push_back(data_value); else code_list.push_back(data_value); // Add the data to data list or code list.
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr)); // Print out error.
							goto final_data; // Jump to the end part.
						}
						else if (Numbers::checkInteger(*cur_token)) // Check if integer type.
						{
							data_value = Numbers::getInteger(*cur_token); // Convert it to integer form.

							if (data_value <= 0xff)
								if (cur_section == SECTION_DATA) data_list.push_back(data_value); else code_list.push_back(data_value); // Add the data to data list or code list.
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));
							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token)) // Check if hex type.
						{
							data_value = Numbers::getHex(*cur_token);

							if (data_value <= 0xff)
								if (cur_section == SECTION_DATA) data_list.push_back(data_value); else code_list.push_back(data_value); // Add the data to data list or code list.
							else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr));
							goto final_data;
						}
						else if ((*cur_token)[0] == '@')  // Check if string defination.
						{
							for (int cnt = 1; cnt < cur_token->length(); cnt++) if (cur_section == SECTION_DATA) data_list.push_back((*cur_token)[cnt]); else code_list.push_back((*cur_token)[cnt]);  // Push all characters to data section or code section.

							if (cur_section == SECTION_DATA) data_list.push_back(NULL); else code_list.push_back(NULL); // Push a null-byte for string.

							goto final_data;
						}
						else for (register list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token) // Check if label is found.
							{
								if (cur_label->label_address <= 0xff) // Check if address is a byte.
									if (!cur_label->isExtern) if (cur_section == SECTION_DATA) data_list.push_back(cur_label->label_address); else code_list.push_back(cur_label->label_address); // Store the address in the data list.
										else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr)); // Print out error.
								else error_list.push_back(Assembler::getErrorMessage("Byte data overflow!", file, line_cntr, tok_cntr)); // Print out error.

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr)); // Print out error.
					}
					else if (*cur_token == "dw")
					{
						tok_cntr++; // Increment the counter.

						if (Numbers::checkBinary(*(++cur_token))) // Check if binary form.
						{
							data_value = Numbers::getBinary(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value); // Convert the data into bytes.

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]); // Store the bytes in the data list.
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else if (Numbers::checkInteger(*cur_token)) // Check if integer form.
						{
							data_value = Numbers::getInteger(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value); 

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token))
						{
							data_value = Numbers::getHex(*cur_token);
							data_ptr = getBytes(sizeof(short), 0, data_value);

							if (data_value <= 0xffff)
								for (int c = 0; c < sizeof(short); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else for (register list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token) // Check if label is found.
							{
								if (cur_label->label_address <= 0xffff) // Check the data bound.
								{
									if (!cur_label->isExtern) // Check if not extern.
									{ 
										data_ptr = getBytes(sizeof(short), 0, cur_label->label_address); // Convert address into bytes.

										for (int c = 0; c < sizeof(short); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]); // Store the address bytes in the data table.
									}
									else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr)); // Print out error.
								}
								else error_list.push_back(Assembler::getErrorMessage("Word data overflow!", file, line_cntr, tok_cntr)); // Print out error.

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr)); // Print out error.
					}
					else if (*cur_token == "dd")
					{
						tok_cntr++; // Increment the counter.

						if (Numbers::checkBinary(*(++cur_token))) // Check if binary form.
						{
							data_value = Numbers::getBinary(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else if (Numbers::checkInteger(*cur_token)) // Check if integer form.
						{
							data_value = Numbers::getInteger(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else if (Numbers::checkHex(*cur_token)) // Check if hex form.
						{
							data_value = Numbers::getHex(*cur_token);
							data_ptr = getBytes(sizeof(int), 0, data_value);

							if (data_value <= 0xffffffff)
								for (int c = 0; c < sizeof(int); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]);
							else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr)); // Print out error.

							goto final_data;
						}
						else for (register list<Label>::iterator cur_label = label_list.begin(); cur_label != label_list.end(); cur_label++)
							if (cur_label->label_name == *cur_token) // Check if label is found.
							{
								if (cur_label->label_address <= 0xffffffff) // Check data bounds.
								{
									if (!cur_label->isExtern) // Check if extern label.
									{
										data_ptr = getBytes(sizeof(int), 0, cur_label->label_address); // Convert address into bytes.

										for (int c = 0; c < sizeof(int); c++) if (cur_section == SECTION_DATA) data_list.push_back(data_ptr[c]); else code_list.push_back(data_ptr[c]); // Store the bytes in the data list.
									}
									else error_list.push_back(Assembler::getErrorMessage("Extern labels cannot be used!", file, line_cntr, tok_cntr)); // Print out error.
								}
								else error_list.push_back(Assembler::getErrorMessage("DWord data overflow!", file, line_cntr, tok_cntr)); // Print out error.

								goto final_data;
							}

						error_list.push_back(Assembler::getErrorMessage("Unknown label given " + *cur_token + " at line", file, line_cntr, tok_cntr)); // Print out error.
					}
				final_data:;
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
