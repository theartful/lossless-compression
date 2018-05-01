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
		(start->children)[i] = new TrieNode(i + 1);
	}
}

void Trie::VisitStringContext(vector<int>& string, int contextLength, int currentPos)
{
	int effectiveStringSize = currentPos + 1;
	if (effectiveStringSize < contextLength)
		return;
	int startIndex = (effectiveStringSize - contextLength > 0) ? string.size() - contextLength : 0;
	TrieNode* currentNode = start;
	TrieNode* parent = start;
	int word;
	for (int i = startIndex; i < effectiveStringSize; i++)
	{
		word = string[i];
		parent = currentNode;
		TrieNode* nextNode = currentNode->children[word];
		if (nextNode == nullptr)
		{
			ull diff = 0;
			ull proposedProb = 0;
			for (int k = 0; k <= word; k++)
			{
				if (currentNode->children[k] != nullptr)
				{
					diff = 0;
					proposedProb = currentNode->children[k]->value;
				}
				else
					diff++;
			}
			currentNode->children[word] = new TrieNode(proposedProb + diff); // We have never visited this node before.
		}
		currentNode = currentNode->children[word];
	}
	for (int i = word; i < MAX_CHILDREN; i++)
	{
		if (parent->children[i] != nullptr)
			parent->children[i]->value++;
	}
}


void Trie::VisitString(vector<int>& string, int currentPos)
{
	if (currentPos = -1)
		currentPos = string.size() - 1; // i.e. the end of the string
	for (int i = 1; i <= MAX_DEPTH; i++)
	{
		VisitStringContext(string, i, currentPos);
	}
}

void Trie::GetCumulativeProbability(ull & currCount, ull & totalCount, vector<int>& string, int contextLength, int currentPos)
{
	currCount = 0;
	totalCount = 0;
	if (currentPos == -1)
		currentPos = string.size() - 1;
	int effectiveSize = currentPos + 1;
	int startIndex = (effectiveSize - contextLength > 0) ? effectiveSize - contextLength : 0; 
	TrieNode* currentNode = start;
	TrieNode* parentNode = currentNode;
	int word;
	int j = startIndex;
	for (; j < effectiveSize; j++)
	{
		parentNode = currentNode;
		word = string[j];
		if (word < 0)
		{
			break;
		}
		TrieNode* nextNode = currentNode->children[word];
		if (nextNode == nullptr)
		{
			int i = 0;
			int maxNotNull = -1;
			int posAddition = 0;
			for (; i <= word; i++)
			{
				if (currentNode->children[i] != nullptr)
				{
					posAddition = 0;
					maxNotNull = i;
				}
				else
				{
					posAddition++;
				}
			}
			currCount = (maxNotNull != -1) ? currentNode->children[maxNotNull]->value + posAddition : posAddition;
			break;
		}
		else
		{
			currentNode = nextNode;
			currCount = currentNode->value;
		}
	}
	int maxNotNull = -1;
	int diff = 0;
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		if (parentNode->children[i] != nullptr)
		{
			maxNotNull = i;
			diff = 0;
		}
		else
			diff++;
	}
	totalCount = (maxNotNull != -1) ? parentNode->children[maxNotNull]->value + diff : diff;
}


void Trie::PrintProbabilities()
{
	TrieNode* currentNode = start;
	for (int i = 1; i < MAX_CHILDREN; i++)
	{
		TrieNode* currentChild = currentNode->children[i];
		cout << "P(" << i << ") = " << currentChild->value - currentNode->children[i-1]->value << "\n";
	}
	cout << "\n";
}

ull Trie::GetProbability(ull& tC, vector<int>& string, int contextLength, int currentPos)
{
	ull currCount = 0;
	if (currentPos == -1)
		currentPos = string.size() - 1;
	int effectiveSize = currentPos + 1;
	int startIndex = (effectiveSize - contextLength > 0) ? effectiveSize - contextLength : 0;
	TrieNode* currentNode = start;
	TrieNode* parentNode = currentNode;
	int word;
	int j = startIndex;
	for (; j < effectiveSize; j++)
	{
		parentNode = currentNode;
		word = string[j];
		if (word < 0)
		{
			break;
		}
		TrieNode* nextNode = currentNode->children[word];
		if (nextNode == nullptr)
		{
			int i = 0;
			int maxNotNull = -1;
			int posAddition = 0;
			for (; i <= word; i++)
			{
				if (currentNode->children[i] != nullptr)
				{
					posAddition = 0;
					maxNotNull = i;
				}
				else
				{
					posAddition++;
				}
			}
			currCount = (maxNotNull != -1) ? currentNode->children[maxNotNull]->value + posAddition : posAddition;
			break;
		}
		else
		{
			currentNode = nextNode;
			currCount = currentNode->value;
		}
	}
	int maxNotNull = -1;
	int diff = 0;
	int maxBeforeWord = -1;
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		if (parentNode->children[i] != nullptr)
		{
			if (i < word)
				maxBeforeWord = i;
			maxNotNull = i;
			diff = 0;
		}
		else
			diff++;
	}
	tC = (maxNotNull != -1) ? parentNode->children[maxNotNull]->value + diff : diff;
	ull prevCount = (maxBeforeWord != -1) ? parentNode->children[maxBeforeWord]->value + word - maxBeforeWord - 1 : word - 1;
	return currCount - prevCount;
}