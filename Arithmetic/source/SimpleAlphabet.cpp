#include "stdafx.h"
#include "SimpleAlphabet.h"

SimpleAlphabet::SimpleAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	for (unsigned long int i = 0; i < numCharacters; i++)
		cumulativeCount.push_back(i + 1);
	totalCount = numChars;
}

long int SimpleAlphabet::GetComulativeCount(long i)
{
	if (i < 0)
		return 0;
	return cumulativeCount[i];
}

void SimpleAlphabet::Update(int lastSeenWord)
{
	// Do nothing: we're not adaptive.
}
