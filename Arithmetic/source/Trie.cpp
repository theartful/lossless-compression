#include "stdafx.h"
#include "Trie.h"

TrieNode::TrieNode(ull val, bool childless) : value(val)
{
	if (!childless)
	{
		for (int i = 0; i < MAX_CHILDREN; i++)
			children[i] = nullptr;
	}
}

Trie::Trie()
{
	start = new TrieNode(MAX_CHILDREN);
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		(start->children)[i] = new TrieNode(1);
	}
}

void Trie::VisitStringContext(vector<int> string, int contextLength, int currentPos)
{
	int effectiveStringSize = currentPos + 1;
	if (effectiveStringSize < contextLength)
		return;
	int startIndex = (effectiveStringSize - contextLength > 0) ? string.size() - contextLength : 0;
	TrieNode* currentNode = start;
	for (int i = startIndex; i < effectiveStringSize; i++)
	{
		auto word = string[i];
		TrieNode* nextNode = currentNode->children[word];
		if (nextNode == nullptr)
		{
			currentNode->children[word] = new TrieNode(0); // We have never visited this node before.
		}
		currentNode = currentNode->children[word];
	}
	currentNode->value++; // we increment the current node's value at this context
}


void Trie::VisitString(vector<int> string, int currentPos)
{
	if (currentPos = -1)
		currentPos = string.size() - 1; // i.e. the end of the string
	for (int i = 1; i <= MAX_DEPTH; i++)
	{
		VisitStringContext(string, i, currentPos);
	}
}

void Trie::PrintProbabilities()
{
	TrieNode* currentNode = start;
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		TrieNode* currentChild = currentNode->children[i];
		if (currentChild->value <= 1)
			continue;
		cout << "P(" << (char)i << ") = " << currentChild->value - 1 << "\n";
		for (int j = 0; j < MAX_CHILDREN; j++)
		{
			if (currentChild->children[j] == nullptr)
				continue;
			cout << "\tP(" << (char)j << " | " << (char)i << ") = " << currentChild->children[j]->value << "\n";
			TrieNode* secondChild = currentChild->children[j];
			for (int k = 0; k < MAX_CHILDREN; k++)
			{
				if (secondChild->children[k] == nullptr)
					continue;
				cout << "\t\tP(" << (char)k << " | " << (char)i << (char)j << ") = " << secondChild->children[k]->value << "\n";
			}
		}
	}
	cout << "\n";
}