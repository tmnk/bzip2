#include "num.h"


class LNode{
public:
	unsigned char val;
	LNode *next, *prev	;
	LNode(unsigned char v, LNode *p, LNode *n) : val(v), prev(p), next(n) {};
	LNode() {
		val = 0;
		prev = next = NULL;
	}
};

LNode *node;

class List{
public:
	LNode *last, *first;
	LNode **arr;
	int lsize;
	List() {
		last = first = NULL;
		lsize = 0;
		arr = new LNode*[256];
		for (int i = 0; i < 256; i++) arr[i] = NULL;
	};
	int size() {return lsize;};
	void push_back(unsigned char v) {
		if (!last) {
			last = first = new LNode(v, NULL, NULL);
			lsize = 1;
			arr[v] = last;
		}
		else {
			last->next = new LNode(v, last, NULL);
			lsize++;
			last = last->next;
			arr[v] = last;
		}
	};

	void swap_to_front(unsigned char v) {
		if (first == arr[v]) return;
		if (last == arr[v]) {
			arr[v]->prev->next = NULL;
			last = arr[v]->prev;
			arr[v]->prev = NULL;
			arr[v]->next = first;
			first->prev = arr[v];
			first = arr[v];
		}
		else {
			node = arr[v];
			if (node->prev->next != node) {
				std::cout << '\n';
			}
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->prev = NULL;	
			node->next = first;
			first->prev = node;
			first = node;
		}
	};

};
void mtf(std::string &str) { 
	//std::list<unsigned char> al;
	List al;
	std::string result;
	for (int i = 0; i < 256; i++)
			al.push_back((unsigned char)i);

	for (int i = 0; i < str.size(); i++) {
		int j = 0;
		for (LNode *it = al.first; it != NULL; j++, it = it->next) {
			if ((unsigned char)str[i] == it->val) {
				al.swap_to_front((unsigned char)str[i]);
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
