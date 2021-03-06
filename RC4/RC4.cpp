/********************************************************************************************
*										RC4.cpp 											*
*																							*
*	DESCRIPTION: RC4 Stream Cipher															*
*				 Input Parameters: RC4 <key> <infile> <outfile>								*
*																							*
*																							*
*	AUTHOR: Aaron Millikin											START DATE: 11/8/2017	*
*********************************************************************************************/
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

unsigned char s[256];
int index1;
int index2;

//Initialize s[256] to uchar 0-255
void intialize() {
	index1 = 0;
	index2 = 0;
	for (int i = 0; i < 256; i++) {
		s[i] = i;
	}
}

//Generate Key using intialized state and provided key
void keygen(unsigned char * key, int keyLength) {
	intialize();
	int j = 0;
	for (int i = 0; i < 256; i++) {
		j = (j + s[i] + key[i%keyLength]) % 256;
		swap(s[i], s[j]);
	}
}

//Generates and returns next pseudo random number
unsigned char prng(int keyLength) {
	index1 = (index1 + 1) % 256;
	index2 = (index2 + s[index1]) % 256;
	swap(s[index1], s[index2]);
	return s[(s[index1] + s[index2]) % 256];
}

/*Swaps state indexes
void swap(int sIndex1, int sIndex2) {
	unsigned char temp = s[sIndex1];
	s[sIndex1] = s[sIndex2];
	s[sIndex2] = temp;
}*/

bool validAsciiLength(string key) {
	return key.length() > 0 && key.length() <= 256;
}

bool validHexLength(string key) {
	return key.length() > 0 && key.length() <= 512;
}

bool validAsciiKey(string key) {
	if (validAsciiLength(key)) {
		return true;
	}
	else {
		cout << "Not a valid key. Must be less than 257 characters" << endl;
		return false;
	}
}

bool validHexKey(string key) {
	if (validHexLength(key)) {
		if (key.length() % 2 == 0)
			for (int i = 0; i < key.length(); i++) {
				if (!isxdigit(key[i])) {
					cout << "Not a valid hex key. Must contain only hex digits."<< endl;
					return false;
				}
			}
		else {
			cout << "Not a valid hex key. Must have even number of hex digits." << endl;
				return false;
		}
	}
	else {
		cout << "Not a valid hex key. Must be less than 514 hex digits (< 257 chars)" << endl;
		return false;
	}
}

// If key is wrapped in single quotes, 
//		Check if valid ASCII string
// Else 
//		Check if valid Hex string
// Returns: 0 if INVALID; 1 if ASCII key; 2 if Hex key
int getKeyType(string key) {
	if (key.substr(0, 1) == "'" && key.substr(key.length() - 1, 1) == "'") {
		return validAsciiKey(key.substr(0, key.length() - 2)) ? 1 : 0;
	}
	else {
		return validHexKey(key) ? 2 : 0;
	}
}

void prompt() {
	cout << "Welcome to Aaron's RC4 Stream Cipher." << endl;
	cout << "Accepted input: RC4 <key> <infile> <outfile>" << endl;
}
int main(int argc, char* argv[]) {
	clock_t startTime = clock();
	double secondsElapsed;
	ifstream inFile;
	ofstream outFile;
	unsigned char* key = NULL;
	string strKey;
	string keyByte;
	int keyType;
	int keyLength;
	char charIn;

	if (argc != 4) {
		cout << "Incorrect number of arguments supplied." << endl;
		prompt();
		return EXIT_FAILURE;
	}

	inFile.open(argv[2], ios::in | ios::binary);
	if (!inFile) {
		cout << "Can't open input file " << argv[2] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	outFile.open(argv[3], ios::out | ios::binary);
	if (!outFile) {
		cout << "Can't open output file " << argv[3] << endl;
		prompt();
		return EXIT_FAILURE;
	}

	strKey = argv[1];
	keyType = getKeyType(strKey);

	//Key Types:  0 if INVALID; 1 if ASCII key; 2 if Hex key
	if (keyType == 1) {
		strKey = strKey.substr(1, strKey.length()-2);
		keyLength = strKey.length();
		key = new unsigned char[keyLength];
		for (int i = 0; i < keyLength; i++) {
			key[i] = strKey[i];
		}
	}
	else if (keyType == 2) {
		keyLength = strKey.length() / 2;
		key = new unsigned char[keyLength];
		for (int i = 0; i < keyLength; i++) {
			key[i] = stoul(strKey.substr(2 * i,2), NULL, 16);
		}
	}
	else {
		prompt();
		return EXIT_FAILURE;
	}

	keygen(key, keyLength);

	//Clean up key, not needed after key schedule intilization
	delete[] key;
	key = NULL;

	//Read each byte from file
	//XOR next pseudo random number with byte in
	//Output result to outfile
	unsigned char prngRet;
	unsigned char xorResult;
	while (inFile.get(charIn)) {
		prngRet = (prng(keyLength));
		xorResult = charIn ^ prngRet;
		outFile << xorResult;
	}

	inFile.close();
	outFile.close();

	//Output time elapsed
	secondsElapsed = double(clock() - startTime) / CLOCKS_PER_SEC;
	cout << fixed << setprecision(3);
	cout << secondsElapsed << " Seconds Elapsed." << endl;

	return EXIT_SUCCESS;
}

