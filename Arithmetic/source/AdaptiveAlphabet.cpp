#include "stdafx.h"
#include "AdaptiveAlphabet.h"

AdaptiveAlphabet::AdaptiveAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	this->totalCount = 256;
}

AdaptiveAlphabet::~AdaptiveAlphabet()
{
	//probModels.PrintProbabilities();
}

long int AdaptiveAlphabet::GetComulativeCount(long i)
{
	encodedSequence.push_back(i);
	ull cumulativeCount = 0;
	ull tC = 0;
	totalCount = 0;
	ull tCumulativeCount = 0;
	ull norm = 0;
	for (int j = 1; j < 10; j++)
	{
		probModels.GetCumulativeProbability(tCumulativeCount, tC, encodedSequence, j);
		totalCount += tC * j * j;
		cumulativeCount += tCumulativeCount * j * j;
		norm += j*j;
	};
	cumulativeCount = cumulativeCount / norm;// >> 1;
	totalCount = totalCount / norm;// >> 1;
	//cout << cumulativeCount << "\t";
	//cout << totalCount << "\n";
	encodedSequence.pop_back();
	return cumulativeCount;
}

void AdaptiveAlphabet::Update(int lastSeenWord)
{
	encodedSequence.push_back(lastSeenWord);
	probModels.VisitString(encodedSequence);
	totalCount = 0;
	ull cumulativeCount;
	ull tCount = 0;
	ull norm = 0;
	for (int j = 1; j < 10; j++)
	{
		probModels.GetCumulativeProbability(cumulativeCount, tCount, encodedSequence, j);
		totalCount += tCount * j * j;
		norm += j*j;
	};
	totalCount = totalCount / norm;// >> 1;
}


ull AdaptiveAlphabet::GetTotalCount()
{
	return this->totalCount;
}