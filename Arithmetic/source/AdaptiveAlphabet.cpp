#include "stdafx.h"
#include "AdaptiveAlphabet.h"

AdaptiveAlphabet::AdaptiveAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
}

long int AdaptiveAlphabet::GetComulativeCount(long i)
{
	ull cumulativeCount;
	probModels.GetCumulativeProbability(cumulativeCount, totalCount, encodedSequence, 1);
	return cumulativeCount;
}

void AdaptiveAlphabet::Update(int lastSeenWord)
{
	encodedSequence.push_back(lastSeenWord);
	probModels.VisitString(encodedSequence);
}


ull AdaptiveAlphabet::GetTotalCount()
{
	return this->totalCount;
}