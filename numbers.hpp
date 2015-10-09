#ifndef NUMBERS_H

#define NUMBERS_H

#include<iostream>

using namespace std;

class Numbers
{
public:
	static bool checkHex( string ); // Checks if the given string contains a hexa-decimal number or not.
	static bool checkBinary( string ); // Checks if the given string contains a binary number or not.
	static bool checkInteger( string ); // Checks if the given string contains a integer or not.

	static int getInteger( string ); // Returns a integer out of a string.
	static int getHex( string ); // Returns a hexa-decimal in integer form from a string.
	static int getBinary( string ); // Returns a binary in integer from from a string.

	static string toHex( int ); // Returns a string representation of a number in hex format.
	static string toInteger(int); // Returns a string representation of integer.
};

#endif
