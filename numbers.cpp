#include "numbers.hpp"
#include <math.h>

/*
	Numbers::checkHex( ) :
		Checks if the given string contains a hexa-decimal number or not.
*/

bool Numbers::checkHex( string str )
{
	if (str.length() > 3)
	{
		if (str[0] == '-' && str[1] == '0' && str[2] == 'x')
		{
			for (int cnt = 2; cnt < str.length(); cnt++)
			{
				if (!((str[cnt] >= '0' && str[cnt] <= '9') || (str[cnt] >= 'a' && str[cnt] <= 'f') || (str[cnt] >= 'A' && str[cnt] <= 'F'))) return false; // Check for existence of any illegal token
			}

			return true;
		}
	}
	
	if (str.length( ) > 2) // Check if the length is greater than 2.
    {
        if (str[0] == '0' && str[1] == 'x') // Check if the string matches proper hexa-decimal marks.
        {
            for(int cnt = 2;cnt < str.length( );cnt++)
            {
                if (!((str[cnt] >= '0' && str[cnt] <= '9') || (str[cnt] >= 'a' && str[cnt] <= 'f') || (str[cnt] >= 'A' && str[cnt] <= 'F'))) return false; // Check for existence of any illegal token
            }

            return true;
        }
        else return false;
    }
    else return false;
}

/*
	Numbers::checkBinary( ) :
		Checks if the given string contains a binary number or not.
*/

bool Numbers::checkBinary( string str )
{
    if (str.length( ) > 2) // Check if the length is greater than 2.
    {
        if (str[0] == '0' && str[1] == 'b') // Check if the string matches proper hexa-decimal marks.
        {
            for(int cnt = 2;cnt < str.length( );cnt++)
            {
                if (!((str[cnt] >= '0' && str[cnt] <= '1'))) return false; // Check for existence of any illegal token
            }

            return true;
        }
        else return false;
    }
    else return false;
}

/*
	Numbers::checkInteger( ) :
		Checks if the given string contains a integer or not.
*/

bool Numbers::checkInteger( string str )
{
    for(int cnt = 0;cnt < str.length( );cnt++)
    {
        if (cnt == 0 && str[cnt] == '-') continue; else return false; // Check for negative integers.
        if (!((str[cnt] >= '0' && str[cnt] <= '9'))) return false; // Check for existence of any illegal token
    }

    return true;
}

/*
	Numbers::getInteger( ) :
		Returns a integer out of a string.
*/

int Numbers::getInteger( string str )
{
    int res_num = 0;
	bool isNegative = false;

    if (!checkInteger(str)) return 0; // Check if proper integer.

    for(int cnt = 0;cnt < str.length( );cnt++)
    {
        if (cnt == 0 && str[cnt] == '-') isNegative = true;
        res_num = res_num * 10 + (str[cnt] - '0');
    }

	return (isNegative) ? -res_num : res_num;
}

/*
	Numbers::getHex( ) :
		Returns a hexa-decimal in integer form from a string.
*/

int Numbers::getHex( string str )
{
    int res_num = 0;

    if (!checkHex(str)) return 0; // Check if proper hexa-decimal.

    for(int cnt = 2;cnt < str.length( );cnt++)
    {
        res_num += pow(16,str.length( ) - cnt - 1) * ((str[cnt] >= 'a') ? (10 + (str[cnt] - 'a')) : (str[cnt] >= '0') ? (str[cnt] - '0') : 0); // Ternay operation for getting the proper number.
    }

    return res_num;
}

/*
	Numbers::getBinary( ) :
		Returns a binary in integer from from a string.
*/

int Numbers::getBinary( string str)
{
    int res_num = 0;

    if (!checkBinary(str)) return 0; // Check if proper binary.

    for(int cnt = 2;cnt < str.length( );cnt++)
    {
        res_num += pow(2,str.length( ) - (cnt - 1)) * (str[cnt] - '0'); // Ternay operation for getting the proper number.
    }

    return res_num;
}

/*
	Numbers::toHex( ) :
		Returns a string representation of integer in hex form.
*/

string Numbers::toHex(int number) // Returns a string representation of integer in hex form.
{
	string ret_str; // Return string handle.
	string num_str; // Number string handle.

	if (number < 0) ret_str = "-0x"; else ret_str = "0x";

	while (number > 16) // Collect all the hexa-decimal digits.
	{
		int rem = number % 16; // Get the hexa-decimal digit.
		number = number / 16;

		if (rem > 9) num_str += (char) ('A' + rem - 10); // Add the letter hexa-decimal digit if more than 9.
		else num_str += toInteger(rem); // Add only the digit.
	}
	
	if (number > 9) num_str += (char) ('A' + number - 10); // Add the letter hexa-decimal digit if more than 9.
	else num_str += number;

	num_str.reserve(); // Reverse the digits.
	
	return (ret_str += num_str); // Return the final string.
}

/*
	Numbers::toInteger( ) :
		Returns a string representation of integer .	
*/

string Numbers::toInteger(int num)
{
	string ret_str; // Return string handle.

	if (num == 0) return "0"; // If zero then return directly.
	else if (num < 0) ret_str = "-"; // Negative check.

	register int total_digits = 0; // Total digits.
	register int reverse = 0; // Reversed digits excluding zeros in the end.

	while (num > 0)
	{
		reverse = reverse * 10 + (num % 10); // Reverse formula.
		num /= 10;
		total_digits++; // Increment digit counter.
	}

	register int counter = 0; // Base counter.

	while (reverse > 0)
	{
		ret_str += '0' + (reverse % 10); // Add the digits one by one.
		reverse /= 10; // Obtain the next digit.
		counter++; // Increment base counter.
	}

	while (counter < total_digits)
	{
		ret_str += '0'; // Add the remaining zeros.
		counter++; // Increment base counter.
	}

	return ret_str; // Return the final string.
}