#include "stdafx.h"
#include "SimpleAlphabet.h"

SimpleAlphabet::SimpleAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	for (unsigned long int i = 0; i < numCharacters; i++)
		cumulativeCount.push_back(i + 1);
	totalCount = numChars;
}

ull SimpleAlphabet::GetComulativeCount(long i)
{
	if (i < 0)
		return 0;
	return cumulativeCount[i];
}

ull SimpleAlphabet::GetTotalCount()
{
	return this->totalCount;
}

void SimpleAlphabet::Update(int lastSeenWord)
{
	// Do nothing: we're not adaptive.
}

void SimpleAlphabet::SetTotalCount(int tC)
{
	this->totalCount = tC;
}
