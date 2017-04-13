#include "num.h"

enum {
	GoToPar = 256, Lson = -3, Rson = -2
};

class Node{
public:
	unsigned short int k;
	int d;
	Node *left, *right, *parent;
	Node() { k = 256; left = right = parent = NULL; };
	Node (unsigned char a, int b) : k(a), d(b), left(NULL), right(NULL)  {};
	Node (Node *l, Node *r) : left(l), right(r), k(256) {};
	Node (Node *p) : k(256), parent(p), left(NULL), right(NULL)  {};
};

Node *root;
int indexOfResult = 0;
char tc;
std::string code;
std::map<unsigned char, std::string > table;
struct Compare {
	bool operator() (Node *a, Node *b) {
		return a->d < b->d;
	}
};

Node * buildTree(std::string &str) {
	std::list<Node *> l;
	for (int i = 0; i < 256; i++) 
		if (linerTable[i]) l.push_back(new Node(i, linerTable[i]));
	
	while (l.size() != 1) {
		l.sort(Compare());
		Node *n = new Node;
		n->left = l.front();
		l.pop_front();
		n->right = l.front();
		l.pop_front();
		n->d = n->left->d + n->right->d;
		l.push_front(n); 
	}
	return l.front();
}
void buildTable(Node *node){
	if (node->left) {
		code.push_back('0');
		buildTable(node->left);
	}
	if (node->right) {
		code.push_back('1');
		buildTable(node->right);
	}
	if (node->k != GoToPar) {
		table[(unsigned char)node->k] = code;
	}
	if (code.size()) code.pop_back();
}


unsigned char recoveryChar(Node *tree, std::string &str) {
	while(1) {
		if (!tree->left && !tree->right) {
			if (tree == root) indexOfResult++;
			return (unsigned char)tree->k;
		}
		tc = str[int(indexOfResult / 8)];
		tc =  tc & (1 << (7 - indexOfResult + int(indexOfResult / 8) * 8));
		if (tc != 0) {
			tree = tree->right;
			indexOfResult++;
			continue;
		}
		if (tc == 0) {
			tree = tree->left;
			indexOfResult++;
			continue;
		}
	}
	return (unsigned char)tree->k;
}

std::string huffDecode(std::string &string) {
	root = buildTree(string);
	std::string result;
	const char *str = string.c_str();
	indexOfResult = 0;
	currBlockSize = 0;
	while ((unsigned)indexOfResult < sizeOfResult) {
		result += recoveryChar(root, string);
		currBlockSize++;
	}
	//if (!result.size()) return 
	return result;
}



void huffEncode(std::string &str) {
	for (int i = 0; i < 256; i++) linerTable[i] = 0;
	for (unsigned int i = 0; i < str.size(); i++) linerTable[(unsigned char)str[i]]++;
	root = buildTree(str);
	buildTable(root);
	linerStr.clear();
	std::string buff;
	unsigned char ch = 0;
	int j = 0;
	for (unsigned int i = 0; i < str.size(); ) {
		if (buff.size() < 8) buff += table[str[i++]];
		if (buff.size() < 8) continue;
		ch = 0;
		for (int k = 7; k >= 0; k--) {
			ch |= (buff[k] == '0' ? 0 : 1) << (7 - k);
		}
		linerStr += ch;
		j += 8;
		buff = buff.substr(8, buff.size() - 8);
	}
	if (buff.size() > 0) {
		ch = 0;
		for (unsigned int k = 0 ; k < buff.size(); k++) {
			ch |= (buff[k] == '0' ? 0 : 1) << (7  - k);
			j++;
		}
		linerStr += ch;
	}
	sizeOfResult = j;
	if (sizeOfResult == 0) sizeOfResult = linerTable[(unsigned char)str[0]];
}