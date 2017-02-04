#include "num.h"
#define NO_TERMINAL_SYMBOL 9

int compare(const void *a1, const void *b1) {
	int a = *(int *)a1, b = *(int *)b1, cmp;
	for (int i = 0; i < sourceStr.size(); i++)
		if (cmp = (int)sourceStr[(a + i) % sourceStr.size()] - (int)sourceStr[(b + i) % sourceStr.size()]) return cmp;
	return 0;
}



std::string bwTransform() {
	int *shiftsArray = new int[sourceStr.size()];
	for (int i = 0; i < sourceStr.size(); i++) shiftsArray[i] = i;
	std::qsort(shiftsArray, sourceStr.size(), sizeof(int), &compare);
	//sourceStr.clear();
	std::string result;
	for (int i = 0; i < sourceStr.size(); i++) {
		result += (shiftsArray[i] != 0
			? sourceStr[shiftsArray[i] - 1] 
			: (beginReverse = i, sourceStr[sourceStr.size() - 1]));
	}
	return result;
}

std::string bwtReverse(std::string &str) {
	std::vector<int> placeArray(256), id; //, countArray(256);
	std::string result, sortStr = str;
	result.resize(str.size());
	sort(sortStr.begin(), sortStr.end());
	//for (int i = 0; i < sortStr.size(); i++)
		//if (!placeArray[(unsigned char)sortStr[i]])
			//placeArray[(unsigned char)sortStr[i]] = i;
	std::map<unsigned char, int> mp;
	unsigned char c = sortStr[0];
	for (int i = 1; i < sortStr.size(); i++)
		if (!mp[sortStr[i]] && c != sortStr[i]) 
			mp[sortStr[i]] = i;
	for (int i = 0; i < str.size(); i++)
		id.push_back(mp[str[i]]++);

	int start = 0, sh = beginReverse;
	for (int i = 0; i < str.size(); i++) {
		result[sourceStr.size() - 1 - i] = str[sh];
		sh = id[sh];
	}
	return result;



}
