#include "zip.h"

int main(int argc,const char *argv[]) {
	if (argc < 2) {
		std::cout << "./mzip: I won't write compressed data to a terminal.\n./mzip: For help, type: `./mzip -h'." << std::endl;
		return 0;
	}
	else if (argc < 3) {
		if (!strcmp(argv[1], "-h")) help();
		else if (!strcmp(argv[1], "-")) {
			char ch;
			while ((ch = getchar()) != EOF) {
				sourceStr += ch;
			}
			bwTransform();
			mtf(sourceStr);
			huffEncode(sourceStr);
			std::cout << linerStr << std::endl;
		}
		else {
			inputFile = argv[1];
			zip();
		}
		return 0;
	}
	else {
		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				int lenFlag = strlen(argv[i]);
				if (lenFlag > 2) {
					for (int j = 0; j < lenFlag; j++) createFlag(argv[i][j]);
					break;
				}
				else createFlag(argv[i][1]);
			}
			else {
				inputFile = argv[i];
				break;
			}
		}
		if (!inputFile.size()) {
			help(); 
			return 0;
		}
		zip();
	}
	
	return 0;
}