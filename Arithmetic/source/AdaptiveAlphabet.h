#pragma once
#include "stdafx.h"
#include "Alphabet.h"
#include "Trie.h"

class AdaptiveAlphabet : public Alphabet
{
	Trie probModels;
	vector<int> encodedSequence;
	vector<int> weights;
public:
	AdaptiveAlphabet(unsigned long int numChars);
	~AdaptiveAlphabet();
	virtual long int GetComulativeCount(long i);
	virtual void Update(int lastSeenWord);
	virtual ull GetTotalCount();
};