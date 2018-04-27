#pragma once
#include "stdafx.h"
#include "Alphabet.h"
#include "Trie.h"

class AdaptiveAlphabet : public Alphabet
{
	Trie probModels;
	vector<int> encodedSequence;
	vector<double> weights;
	void CalculateGradient(int lastSeenWord);
	long int ExperimentalGetComulativeCount(long word);
public:
	AdaptiveAlphabet(unsigned long int numChars);
	~AdaptiveAlphabet();
	virtual ull GetComulativeCount(long i);
	virtual void Update(int lastSeenWord);
	virtual ull GetTotalCount();
};