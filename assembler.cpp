#include "assembler.hpp"
#include "first_pass.hpp"
#include "second_pass.hpp"
#include "numbers.hpp"
#include <stdio.h>

/*
	getEscapeSequence( ) :
	 Returns a escape sequence by taking a base character.
*/

static int getEscapeSequence(char input_char)
{
	if (input_char == 'n') return '\n';
	else if (input_char == 't') return '\t';
	else if (input_char == 'v') return '\v';
	else if (input_char == 'b') return '\b';
	else if (input_char == 'r') return '\r';
	else if (input_char == 'f') return '\f';
	else if (input_char == 'a') return '\a';
	else if (input_char == '\\') return '\\';
	else if (input_char == '\'') return '\'';
	else if (input_char == '\"') return '\"';
	else if (input_char == '\0') return '\0';
	else if (input_char == '\ooo') return '\ooo';
	else return input_char;
}

/*
	extractTokens( ) : 
		Returns a list of tokens found in a given line.
*/

static list<string> extractTokens(int line_num , string file , string line , list<string> & error_list)
{
    string cur_token = ""; // Stores the current token.
    list<string> token_list; // List of tokens in the line.
    bool isString = false , isEscapeSeq = false , isChar = false; // Check if the token is a string.
    int cnt = 0; // Counter .

    for(;cnt < line.length( );cnt++)
    {
		/* 
			Parse throught all the characters and follow the flags.
		*/
        if (!isString && !isEscapeSeq && !isChar) // Non-string tokenizing.
        {
            if (line[cnt] == ' ' || line[cnt] == '\t' || line[cnt] == -1) // Check if white-space or tabs.
            {
                if (cur_token != "") // Check if there is a token.
                {
                    token_list.push_back(cur_token); // Add the current token to the list.
                    cur_token = ""; // Empty for new tokens.
                }
            }
            else if (line[cnt] == '\"') // Check if string identifier.
            {
                if (cur_token != "") token_list.push_back(cur_token); // Add the current token to the list.

                cur_token = "@"; // String identifier for quicker searching.
                isString = true; // Jump to the string extraction.
            }
			else if (line[cnt] == '\'') // Check if character identifier.
			{
				if (cur_token != "") token_list.push_back(cur_token); // Add the current token to the list.

				cur_token = ""; // Empty token.
				isChar = true; // Jump to the character extraction.
			}
            else if (line[cnt] == ',') // Check if comma identifier.
            {
                if (cur_token != "")
                {
                    token_list.push_back(cur_token); // Add the current token to the list.
                    cur_token = ""; 
                }

                token_list.push_back(string(",")); // Add a comma identifier to the list.
            }
            else if (line[cnt] == ';') // Check if a comment brace.
            {
                if (cur_token != "") token_list.push_back(cur_token); 

				return token_list; // Return our token list to the caller.
            }
            else cur_token += line[cnt]; // Add the current character to the token.
        }
        else // String and Character tokenizing.
        {
			if (isString)
			{
				if (line[cnt] == '\"') // Check if string identifier.
				{
					token_list.push_back(cur_token);
					cur_token = "";
					isString = false; // Jump back to non-string tokenizing.
				}
				else if (line[cnt] == '\\') // Check if escape sequences.
				{
					// Work with the escape sequence.
					isString = false; 
					isEscapeSeq = true;
				}
				else cur_token += line[cnt]; // Add character to current string.
			}
			else if (isChar)
			{
				if (line[cnt] == '\'') // Check if character literal ending.
				{
					if (isEscapeSeq) { token_list.push_back(cur_token); isEscapeSeq = false; goto char_end; } // Check if escape sequence character.
					else if (cur_token.length() > 1) { error_list.push_back(Assembler::getErrorMessage("Expected only a character literal in line!", file, line_num, cnt)); goto char_end; } // Check if more than one character in token.

					token_list.push_back(Numbers::toInteger(cur_token[0])); // Push the current token.
char_end:
					cur_token = ""; // Empty the string.
					isChar = false; // Set current character to false.
				}
				else if (line[cnt] == '\\') // Check if escape sequence literal.
				{
					if (++cnt >= line.length()) break; // Check if not end of string.

					cur_token = Numbers::toInteger(getEscapeSequence(line[cnt])); // Get the escape sequence value.
					isEscapeSeq = true; // Use this flag for the work.
				}
				else cur_token += line[cnt]; // Add the character to the token.
			}
			else if (isEscapeSeq) // Escape sequences.
			{
				/*
					Check all escape sequences and replace them in the string token.
				*/

				cur_token += (char) getEscapeSequence(line[cnt]);
				
				// Switch back to strings.
				isEscapeSeq = false;
				isString = true;
			}
        }
    }

	/*
		Check if any error exists at last.
	*/

    if (isString) error_list.push_back(Assembler::getErrorMessage("Expected end of string literal!",file,line_num,cnt)); // Check if the string literal was not ended.
	if (cur_token != "") token_list.push_back(cur_token); // Add the left over tokens.

    return token_list; // Return back the list.
}

/*
	readFileLines( ) :
		Recursive function to read all the tokens including 'includes'.
*/

static list< list<string> > readFileLines ( string file , list<string> & error_list )
{
    FILE * file_ptr = fopen( file.c_str( ) , "r" ); // Handle to file pointer.
    std::list< std::list<string> > token_tree_list; // List of tokens of every line.
    string cur_line; // Stores the current line.
    int line_cntr = 1; // Line counter.

	/*
		Read every line from the required file and extract 
		every tokens from it and store that in the list.
	*/

    while(!feof(file_ptr)) // Check not end-of-file
    {
        char cur_ch = fgetc(file_ptr); // Get the current token handle.

        if (cur_ch == '\n') // Check if end of line.
        {
            token_tree_list.push_back(extractTokens(line_cntr ,file , cur_line , error_list)); // Extract all tokens of this line and add to the token list.
            cur_line = ""; // Empty the current line.
            line_cntr++;  // Increment line counter. 
        }
        else cur_line += cur_ch; // Add character to current line.
    }

    if (cur_line != "") token_tree_list.push_back(extractTokens(line_cntr , file ,cur_line, error_list)); // Extract all tokens of this line and add to the token list.

    fclose(file_ptr); // Close the file handle.

	/*
		Parse all the include files and add them to the final list.
	*/

    line_cntr = 1; // Reset the line counter.

    for(std::list< std::list<string> >::iterator cur_tree = token_tree_list.begin();cur_tree != token_tree_list.end();cur_tree++,line_cntr++)
    {
		bool isSuccess = false; // Flag for single instruction in a line.
        int tok_cntr = 1; // Token counter .

		/*	
			Pass the all the tokens and check which of them is include.
		*/

        for(std::list<string>::iterator cur_token = (*cur_tree).begin( );cur_token != (*cur_tree).end( );cur_token++,tok_cntr++)
        {
			if (!isSuccess)
			{
				if (*cur_token == "include") // Check if include token.
				{
					if ((*(++cur_token))[0] == '@') // String identifier.
					{
						string inc_file = *cur_token; // Store a handle of the current token.

						token_tree_list.erase(cur_tree); // Remove the current line from the tree.

						std::list < list<string > > token_list = readFileLines(inc_file.substr(0, inc_file.length() - 1), error_list); // Call our recursive function to parse the include files.

						for (list < list<string> >::iterator cur_elem = token_list.begin(); cur_elem != token_list.end(); cur_elem++) token_tree_list.push_back(*cur_elem); // Add all the obtained tokens.

						isSuccess = true; // Setup our flag to true.
					}
					else
					{
						/*
							Print out error.
						*/
						error_list.push_back(Assembler::getErrorMessage("Expected a string literal!", file, line_cntr, tok_cntr));
					}
				}
			}
            else // Error only works if include statement is found.
            {
                /*
					Print out error.
                */
				error_list.push_back(Assembler::getErrorMessage("Only one instruction allow in one line!",file,line_cntr,tok_cntr));
                break;
            }
        }
    }

    return token_tree_list; // Return our token tree.
}

/*
	Assembler::getErrorMessage( ) :
		Used for returning a string error message from a few parameters.
*/

string Assembler::getErrorMessage( string message, string file_name, int line_count, int token_count)
{
	/*
		Prepare a string error message from a few parameters and send it back to the caller.
		Note : Integers are converted to string using another function Integer::toInteger( ).
	*/

    string error_message = "Error at line <";

    error_message += Numbers::toInteger(line_count); error_message += ","; error_message += Numbers::toInteger(token_count);
    error_message += "> : " + message + " in ";
    error_message += file_name;

    return error_message; // Return our message.
}

AssemblerRet Assembler::assemble(string hex_adr , string file_path)
{
	/*
		Call pass1 and pass2 for assembling the given file.
	*/

    AssemblerRet asm_ret; // Handle to be returned.
    
    std::list< std::list<string> > token_tree_list = readFileLines(file_path,asm_ret.error_list); // Get a final token tree of all tokens in the files including 'includes'.

    /* 
		Call pass1 . Collect labels and their respective sections and calculate their addresses. 
	*/
    Pass1::pass(file_path,Numbers::getHex(hex_adr),token_tree_list,asm_ret.label_list,asm_ret.error_list , asm_ret.end_address);
    
	/* 
		Call pass2 . Generates opcodes. 
	*/

    Pass2::pass(file_path,token_tree_list,asm_ret.label_list,asm_ret.error_list,asm_ret.gen_code,asm_ret.gen_data);

    return asm_ret; // Return our assembler return table.
}
