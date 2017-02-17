#include "num.h"


void mtf(std::string &str) { 
	std::list<unsigned char> al;
	std::vector<int> a;
	std::string result;
	a.resize(256);
	for (int i = 0; i < 256; i++)
			al.push_back((unsigned char)i);

	for (int i = 0; i < str.size(); i++) {
		int j = 0;
		for (std::list<unsigned char>::iterator it = al.begin(); it != al.end(); j++, it++) {
			if ((unsigned char)str[i] == *it) {
				al.remove((unsigned char)str[i]);
				al.push_front((unsigned char)str[i]);
				str[i] = j;
				break;
			}
		}
	}
}

void mtfDecode(std::string &str) {
	std::vector<unsigned char> al;
	unsigned char tmp;
	al.resize(256);
	for (int i = 0, j = 0; i < 256; i++, j++)
			al[j] = (unsigned char)i;
	for (int i = 0; i < str.size(); i++) {
		tmp = al[(unsigned char)str[i]];
		for (int j = (unsigned char)str[i]; j > 0; j--) 
			al[j] = al[j - 1];
		str[i] = tmp;
		al[0] = tmp;
	}
}
