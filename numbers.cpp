#include "numbers.hpp"
#include <math.h>

bool Numbers::checkHex( string str ) // Checks if the given string contains a hexa-decimal number or not.
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

bool Numbers::checkBinary( string str ) // Checks if the given string contains a binary number or not.
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

bool Numbers::checkInteger( string str ) // Checks if the given string contains a integer or not.
{
    for(int cnt = 0;cnt < str.length( );cnt++)
    {
        if (cnt == 0 && str[cnt] == '-') continue; else return false; // Check for negative integers.
        if (!((str[cnt] >= '0' && str[cnt] <= '9'))) return false; // Check for existence of any illegal token
    }

    return true;
}

int Numbers::getInteger( string str ) // Returns a integer out of a string.
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

int Numbers::getHex( string str ) // Returns a hexa-decimal in integer form from a string.
{
    int res_num = 0;

    if (!checkHex(str)) return 0; // Check if proper hexa-decimal.

    for(int cnt = 2;cnt < str.length( );cnt++)
    {
        res_num += pow(16,str.length( ) - cnt - 1) * ((str[cnt] >= 'a') ? (10 + (str[cnt] - 'a')) : (str[cnt] >= '0') ? (str[cnt] - '0') : 0); // Ternay operation for getting the proper number.
    }

    return res_num;
}

int Numbers::getBinary( string str) // Returns a binary in integer from from a string.
{
    int res_num = 0;

    if (!checkBinary(str)) return 0; // Check if proper binary.

    for(int cnt = 2;cnt < str.length( );cnt++)
    {
        res_num += pow(2,str.length( ) - (cnt - 1)) * (str[cnt] - '0'); // Ternay operation for getting the proper number.
    }

    return res_num;
}


string Numbers::toHex(int number) // Returns a string representation of integer in hex form.
{
	string ret_str;
	string num_str;

	if (number < 0) ret_str = "-0x"; else ret_str = "0x";

	while (number > 16)
	{
		int rem = number % 16;
		number = number / 16;

		if (rem > 9) num_str += (char) ('A' + rem - 10);
		else num_str += rem;
	}
	
	if (number > 9) num_str += (char) ('A' + number - 10);
	else num_str += number;

	num_str.reserve();
	
	return (ret_str += num_str);
}

string Numbers::toInteger(int num) // Returns a string representation of integer .
{
	string ret_str;

	if (num == 0) return "0";
	else if (num < 0) ret_str = "-";

	register int total_digits = 0;
	register int reverse = 0;

	while (num > 0)
	{
		reverse = reverse * 10 + (num % 10);
		num /= 10;
		total_digits++;
	}

	register int counter = 0;

	while (reverse > 0)
	{
		ret_str += '0' + (reverse % 10);
		reverse /= 10;
		counter++;
	}

	while (counter < total_digits)
	{
		ret_str += '0';
		counter++;
	}

	return ret_str;
}