#include "stdafx.h"
#include "AdaptiveAlphabet.h"

AdaptiveAlphabet::AdaptiveAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	this->totalCount = 256;
}

long int AdaptiveAlphabet::GetComulativeCount(long i)
{
	ull cumulativeCount;
	encodedSequence.push_back(i);
	ull tC;
	probModels.GetCumulativeProbability(cumulativeCount, tC, encodedSequence, 1);
	encodedSequence.pop_back();
	return cumulativeCount;
}

void AdaptiveAlphabet::Update(int lastSeenWord)
{
	encodedSequence.push_back(lastSeenWord);
	probModels.VisitString(encodedSequence);
	ull cumulativeCount;
	probModels.GetCumulativeProbability(cumulativeCount, totalCount, encodedSequence, 1);
}


ull AdaptiveAlphabet::GetTotalCount()
{
	return this->totalCount;
}