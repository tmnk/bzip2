#ifndef _NUM_H
#define _NUM_H


enum{
	C = 1, D = 2, L = 4, T = 8, R = 16, F = 32 
};

std::string sourceStr, inputFile, outputFile, linerStr;
unsigned int beginReverse = 0, flags = 0, sizeOfBlock = 900000, linerTable[256] = {0}, sizeOfResult = 0;
unsigned long long numOfPi = 0x314159265359, sizeOfCompressed = 0, sizeOfUncompressed = 0;
#endif