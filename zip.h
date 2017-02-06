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
	else if (f == '1') flags |= F;
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
	std::fstream fin(file);
	if (!fin.is_open()) {
		std::cout << "mzip: Can't open input file " << file << ": No such file or directory." << std::endl;
		return ;
	}
	fin >> sizeOfUncompressed;
	fin.seekg (0, std::ios::end);
    sizeOfCompressed = fin.tellg();
    printf("%19lld %19lld %5.1f%c %s\n", sizeOfCompressed, sizeOfUncompressed, 1 - double(sizeOfUncompressed / sizeOfCompressed), '%', unzipedName.c_str());
    fin.close();
    return ;
}

void recursive_step(std::string dir, std::function<void(std::string)> func) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
 
    if ( ( dp = opendir( dir.c_str() ) ) == NULL ) {
    	std::cout << "mzip: " << dir <<  " cann't open." << std::endl;
        return;
    }
 
    chdir( dir.c_str() ); // перемещаемся в указанную директорию
 
    while ( ( entry = readdir( dp ) ) != NULL ) { // цикл по всем файлам указанной директории
        lstat( entry->d_name, &statbuf ); // считываем информацию о файле в структуру
 		std::string tmp;
        if ( S_ISDIR( statbuf.st_mode ) ) { 
            if ( strcmp( ".", entry->d_name ) == 0 || strcmp( "..", entry->d_name ) == 0 ) continue; 
            //std::cout << "dir " << entry->d_name << std::endl;
            recursive_step(tmp = entry->d_name, func);
        }
        else{
        	//std::cout << "file " << entry->d_name << std::endl;
            func(tmp = entry->d_name);
        }
    }
 
    closedir( dp );
}

void encode(std::string filename) {
	if (filename.size()) inputFile = filename;
	std::cout << inputFile << std::endl;
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
	std::ofstream outFile(outputFile, std::ios::binary | std::ios::out);
	sourceStr.resize(sizeOfBlock);
	while (!inFile.eof()){
		inFile.read((char*)&sourceStr[0], sourceStr.size());
		std::streamsize readCount = inFile.gcount();
		if (readCount == 0) break;
		if (readCount < sizeOfBlock) sourceStr.resize(int(readCount));
		sourceStr = bwTransform();
		mtf(sourceStr);
		huffEncode(sourceStr);
		sizeOfCompressed = (unsigned long long)(3 * sizeof(long long) + 258 * sizeof(int) + sizeOfResult * sizeof(char));
		if (flags & C) {
			std::cout << numOfPi << sizeOfCompressed << sizeOfResult << beginReverse;
			for (int i = 0; i < 256; i++) std::cout<< linerTable[i];
			for (int i = 0; i < linerStr.size(); i++) std::cout << linerStr[i];
			std::cout << numOfPi << std::endl;
		}
		else {
			outFile.write((char *)&numOfPi, sizeof(numOfPi));
			outFile.write((char *)&sizeOfCompressed, sizeof(sizeOfCompressed));
			outFile.write((char *)&sizeOfResult, sizeof(sizeOfResult));
			outFile.write((char *)&beginReverse, sizeof(beginReverse));
			outFile.write((char *)linerTable, 256 * sizeof(linerTable[0]));
			outFile.write((const char *)linerStr.c_str(), linerStr.size() * sizeof(char));
			outFile.write((char *)&numOfPi, sizeof(numOfPi));
		}
	}
	outFile.write((char *)&numOfPi, sizeof(numOfPi));
	inFile.close();
	outFile.close();
}

void decode(std::string filename) {
	if (filename.size()) inputFile = filename;
	std::string suff = inputFile.substr(inputFile.size() - 5, inputFile.size() - 2);
	if (!strcmp(suff.c_str(), ".mzip")) {
		std::cout << "mzip: " << inputFile << ": unknown suffix -- ignored" << std::endl;
		return;
	}
	return;
	std::ifstream inFile(inputFile, std::ios::binary | std::ios::in);
	if (flags & C) outputFile = "";
	else {
		outputFile = inputFile;
		outputFile.pop_back(); //p
		outputFile.pop_back(); //i
		outputFile.pop_back(); //z
		outputFile.pop_back(); //m
		outputFile.pop_back(); //.
	}
	std::ofstream outFile(outputFile, std::ios::binary | std::ios::out);
	sourceStr.resize(sizeOfBlock);
	unsigned long long PI1 = 0, PI2 = 0;
	inFile.read((char *)&PI1, sizeof(PI1));
	std::streamsize readCount = inFile.gcount();
	inFile.seekg(readCount - 8,std::ios::beg);
	inFile.read((char *)&PI2, sizeof(PI2));
	if (PI1 != numOfPi || PI2 != numOfPi) {
		std::cout << "mzip: " << inputFile << ": not in mzip format" << std::endl;
		return;
	}
	return;
	while (!inFile.eof()){
		inFile.read((char*)&sourceStr[0], sourceStr.size());
		std::streamsize readCount = inFile.gcount();
		if (readCount == 0) break;
		if (readCount < sizeOfBlock) sourceStr.resize(int(readCount));

	}
}


int zip() {
	if (flags == 0 || flags & F) {
		if (flags) sizeOfBlock = 100000;
		//std::cout << sizeof(const char) << std::endl;
		encode("");
	}

	if (flags & D) {
		decode("");
	}

	struct stat statbuf;
	lstat( inputFile.c_str(), &statbuf );
	if (!(S_ISDIR(statbuf.st_mode) || S_ISREG(statbuf.st_mode))) {
		std::cout << "mzip: Can't open input file " << inputFile << ": No such file or directory." << std::endl;
		return 0;
	}
	if (flags & L) {
		std::cout << "         compressed        uncompressed  ratio uncompressed_name" << std::endl;
		recursive_step(inputFile.c_str(), &list);
		list(inputFile);
		return 0;
	}
	if (flags & R && !(flags & D)) {
		recursive_step(inputFile.c_str(), &encode);
	}
	if (flags & R && flags & D) {
		recursive_step(inputFile.c_str(), &decode);
	}
	return 0;
}


#endif