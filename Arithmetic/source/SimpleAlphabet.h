#pragma once
#include "Alphabet.h"

// Super simple alphabet: assumes equal probability for all contexts, doesn't adapt to anything.
class SimpleAlphabet : public Alphabet
{
public:
	vector<unsigned long int> cumulativeCount;
	SimpleAlphabet(unsigned long int numChars);
	virtual long int GetComulativeCount(long i);
	virtual ull SimpleAlphabet::GetTotalCount();
	virtual void Update(int lastSeenWord);
	void SetTotalCount(int tC);
};