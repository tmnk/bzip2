#ifndef _NUM_H
#define _NUM_H
#include <fstream>

enum{
	C = 1, D = 2, L = 4, T = 8, R = 16, O = 32, N = 64 
};

std::string sourceStr, inputFile, outputFile, linerStr;
unsigned int beginReverse = 0, flags = 0, linerTable[256] = {0}, sizeOfResult = 0, sizeOfBlock = 50000, flagOfCompress = 0, MAX_BLOCK = 120000;
unsigned long long sizeOfCompressed = 0, sizeOfUncompressed = 0, sizeOfOutput = 0, currBlockSize = 0;
double  numOfPi = 0x314159265359, doubleTmp = 0;
char *line;
#endif