#ifndef _ZIP_H
#define _ZIP_H

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <list>
#include <vector>
#include <functional>
#include <cstdio>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include "bwt.h"
#include "haff.h"
#include "mtf.h"
#include "num.h"

std::string bwTransform();
void mtf(std::string &str);
void huffEncode(std::string &str);
void help();

void createFlag(char f) {
	if (f == 'c') flags |= C;
	else if (f == 'd') flags |= D;
	else if (f == 'l') flags |= L;
	else if (f == 't') flags |= T;
	else if (f == 'r') flags |= R;
	else if (f == '1') flags |= O;
	else if (f == '9') flags |= N;
	else help();
}

void help() {std::cout << "usage: ./mzip [flags and input files in any order]\n\n   -h     print this message\n   -l     list compressed file\n   -d     force decompression\n   -f     overwrite existing output files\n   -c     output to standard out\n   -1     fast\n   -9     best\n" << std::endl;}
std::string zipped(std::string &s) {
	if (s.size() > 5 && s.substr(s.size() - 5, 5) == ".mzip") return s.substr(0, s.size() - 5);
	return "";
}

void list(std::string file) {
	std::string unzipedName;
	if ((unzipedName = zipped(file)) == "") {
		std::cout << "mzip: " << file << " not in mzip format." << std::endl;
		return ;
	}
	std::ifstream fin(file, std::ios::binary | std::ios::in);
	if (!fin.is_open()) {
		std::cout << "mzip: Can't open input file " << file << ": No such file or directory." << std::endl;
		return ;
	}
	double PI = 0;
	fin.read((char *)&PI, sizeof(PI));
	fin.read((char *)&sizeOfOutput, sizeof(sizeOfOutput));
	fin.read((char *)&sizeOfUncompressed, sizeof(sizeOfUncompressed));
	if (PI != numOfPi) {
		std::cout << "mzip: " << file << ": not in mzip format" << std::endl;
		fin.close();
		return;
	}
    printf("%19lld %19lld %5.1f%c %s\n", sizeOfOutput, sizeOfUncompressed, double(double(sizeOfOutput) / sizeOfUncompressed) * 100, '%', unzipedName.c_str());
    fin.close();
    return ;
}

void recursive_step(std::string dir, std::function<void(std::string)> func) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if ((dp = opendir(dir.c_str())) == NULL) {
    	std::cout << "mzip: " << dir <<  " cann't open." << std::endl;
        return;
    }
    chdir(dir.c_str());
    while ((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
 		std::string tmp;
        if (S_ISDIR(statbuf.st_mode)){ 
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) continue;
            recursive_step(tmp = entry->d_name, func);
        }
        else{
            func(tmp = entry->d_name);
        }
    }
    closedir(dp);
}

void encode(std::string filename) {
	if (filename.size()) inputFile = filename;
	struct stat statbuf;
	lstat( inputFile.c_str(), &statbuf );
	if (S_ISDIR(statbuf.st_mode)) {
		std::cout << "mzip: Can't open input file " << inputFile << ": Use flag -r." << std::endl;
		return;
	}
	if (inputFile.size() > 5) {
		std::string suff = inputFile.substr(inputFile.size() - 5, inputFile.size() - 2);
		if (!strcmp(suff.c_str(), ".mzip")) {
			std::cout << "mzip: " << inputFile << " already has .mzip suffix -- unchanged" << std::endl;
			return;
		}
	}
	std::ifstream inFile(inputFile, std::ios::binary | std::ios::in);

	if (flags & C) outputFile = "";
	else outputFile = inputFile + ".mzip";
	std::fstream outFile(outputFile, std::ios::binary | std::ios::out);
	sourceStr.resize(sizeOfBlock);
	unsigned int lineSize = 0;
	if (!(flags & C)) {
		outFile.write((char *)&doubleTmp, sizeof(doubleTmp));
		sizeOfOutput = sizeof(doubleTmp) + sizeof(sizeOfOutput) + sizeof(sizeOfUncompressed);
		std::streamsize readCount = inFile.gcount();
		sizeOfUncompressed = 0;
		outFile.write((char *)&sizeOfOutput, sizeof(sizeOfOutput));
		outFile.write((char *)&sizeOfUncompressed, sizeof(sizeOfUncompressed));
	}
	sizeOfUncompressed = 0;
	while (!inFile.eof()){		
		inFile.read((char*)&sourceStr[0], sourceStr.size());
		std::streamsize readCount = inFile.gcount();
		if (readCount == 0) break;
		if (readCount < sizeOfBlock) sourceStr.resize(int(readCount));
		sizeOfUncompressed += int(readCount);
		std::string tmm = sourceStr;
		sourceStr = bwTransform();
		mtf(sourceStr);
		huffEncode(sourceStr);
		sizeOfCompressed = (unsigned long long)(3 * sizeof(long long) + 258 * sizeof(int) + sizeOfResult * sizeof(char));
		if (flags & C) {
			std::cout << numOfPi << sizeOfCompressed << sizeOfResult << beginReverse;
			for (int i = 0; i < 256; i++) std::cout<< linerTable[i];
			for (unsigned int i = 0; i < linerStr.size(); i++) std::cout << linerStr[i];
			std::cout << numOfPi << std::endl;
		}
		else {
			lineSize = linerStr.size() + 1;
			outFile.write((char *)&lineSize, sizeof(lineSize));
			outFile.write((char *)&sizeOfCompressed, sizeof(sizeOfCompressed));
			outFile.write((char *)&sizeOfResult, sizeof(sizeOfResult));
			outFile.write((char *)&beginReverse, sizeof(beginReverse));
			outFile.write((char *)linerTable, 256 * sizeof(linerTable[0]));
			outFile.write((const char *)linerStr.c_str(), linerStr.size() * sizeof(char));
			sizeOfOutput += sizeof(lineSize) + sizeof(sizeOfCompressed) + sizeof(sizeOfResult) + sizeof(beginReverse) + 256 * sizeof(linerTable[0]) + linerStr.size() * sizeof(char);
			// std::cout << lineSize << ", " << sizeOfCompressed << ", " << sizeOfResult << ", " << beginReverse << ", " << sizeOfOutput << "\n";
		}
	}
	if (!(flags & C)) {
		outFile.seekp(0, std::ios::beg);
		outFile.write((char *)&numOfPi, sizeof(numOfPi));
		outFile.write((char *)&sizeOfOutput, sizeof(sizeOfOutput));
		outFile.write((char *)&sizeOfUncompressed, sizeof(sizeOfUncompressed));
	}
	inFile.close();
	outFile.close();
}

void decode(std::string filename) {
	if (filename.size()) inputFile = filename;
	struct stat statbuf;
	lstat( inputFile.c_str(), &statbuf );
	if (S_ISDIR(statbuf.st_mode)) {
		std::cout << "mzip: Can't open input file " << inputFile << ": Use flag -r." << std::endl;
		return;
	}
	if (inputFile.size() > 5) {
		std::string suff = inputFile.substr(inputFile.size() - 5, inputFile.size() - 1);
		//suff += '\0';
		if (strcmp(suff.c_str(), ".mzip")) {
			std::cout << "mzip: " << inputFile << ": unknown suffix -- ignored" << std::endl;
			return;
		}
	}
	else {
		std::cout << "mzip: " << inputFile << ": unknown suffix -- ignored" << std::endl;
		return;
	}
	std::ifstream inFile(inputFile, std::ios::binary | std::ios::in);
	if (flags & C) outputFile = "";
	else {
		outputFile = inputFile;
		outputFile.pop_back(); //p
		outputFile.pop_back(); //i
		outputFile.pop_back(); //z
		outputFile.pop_back(); //m
		outputFile.pop_back(); //.
		sourceStr.resize(sizeOfBlock);
	}
	std::ofstream outFile(outputFile, std::ios::binary | std::ios::out);
	double PI = 0;
	unsigned long long sc = 0; //!!
	inFile.read((char *)&PI, sizeof(PI));
	inFile.read((char *)&sizeOfOutput, sizeof(sizeOfOutput));
	inFile.read((char *)&sizeOfUncompressed, sizeof(sizeOfUncompressed));
	sc += sizeof(PI) + sizeof(sizeOfOutput) + sizeof(sizeOfUncompressed);
	if (PI != numOfPi) {
		std::cout << "mzip: " << inputFile << ": not in mzip format" << std::endl;
		inFile.close();
		return;
	}
	unsigned int outBlock = 0;
	while (sc < sizeOfOutput){
		inFile.read((char *)&flagOfCompress, sizeof(flagOfCompress));
		if (flagOfCompress) {
			linerStr.resize(--flagOfCompress);
			line = new char[flagOfCompress];
			inFile.read((char *)&sizeOfCompressed, sizeof(sizeOfCompressed));
			inFile.read((char *)&sizeOfResult, sizeof(sizeOfResult));
			inFile.read((char *)&beginReverse, sizeof(beginReverse));
			inFile.read((char *)linerTable, 256 * sizeof(linerTable[0]));
			sc += sizeof(flagOfCompress) +  sizeof(sizeOfCompressed) + sizeof(sizeOfResult) + sizeof(beginReverse) + 256 * sizeof(linerTable[0]) + flagOfCompress * sizeof(char);
			// std::cout << flagOfCompress << ", " << sizeOfCompressed << ", " << sizeOfResult << ", " << beginReverse << ", " << sc << "\n";
			inFile.read(line, flagOfCompress * sizeof(char));
			for (unsigned int i = 0; i < flagOfCompress; i++) linerStr[i] = line[i];
			sourceStr = huffDecode(linerStr);
			mtfDecode(sourceStr);
			sourceStr = bwtReverse(sourceStr);
		}
		if (flags & C) std::cout << sourceStr << std::endl;
		else outFile.write(&sourceStr[0], currBlockSize);
	}
	inFile.close();
	outFile.close();
}


int zip() {
	if (flags & L) {
		std::cout << "         compressed        uncompressed  ratio uncompressed_name" << std::endl;
		if (flags & R) recursive_step(inputFile.c_str(), &list);
		else list(inputFile);
		return 0;
	}
	if (flags == 0 || flags & O || flags & N) {
		if (flags & N) sizeOfBlock = MAX_BLOCK;
		if (flags & R) {
			if (flags & N) sizeOfBlock = MAX_BLOCK;
			recursive_step(inputFile.c_str(), &encode);
		}
		else encode("");
		return 0;
	}



	if (flags & D) {
		if (flags & R) recursive_step(inputFile.c_str(), &decode);
		decode("");
		return 0;
	}

	struct stat statbuf;
	lstat( inputFile.c_str(), &statbuf );
	if (!(S_ISDIR(statbuf.st_mode) || S_ISREG(statbuf.st_mode))) {
		std::cout << "mzip: Can't open input file " << inputFile << ": No such file or directory." << std::endl;
		return 0;
	}
	return 0;
}


#endif