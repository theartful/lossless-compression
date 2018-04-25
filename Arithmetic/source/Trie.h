#include "stdafx.h"
#include <limits.h>
#define MAX_CHILDREN 256
#define MAX_DEPTH	10

class TrieNode
{
public:
	TrieNode(ull val, bool childless = false);
	TrieNode* children[MAX_CHILDREN];
	ull value;
};

class Trie
{
	TrieNode* start;
public:
	Trie();

	void VisitStringContext(vector<int> string, int contextLength, int currentPos);

	void VisitString(vector<int> string, int currentPos = -1);

	void PrintProbabilities();
};
