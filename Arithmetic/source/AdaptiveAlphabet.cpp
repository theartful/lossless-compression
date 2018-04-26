#include "stdafx.h"
#include "AdaptiveAlphabet.h"

AdaptiveAlphabet::AdaptiveAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	this->totalCount = 256;
	for (int i = 0; i < 9; i++)
	{
		weights.push_back(1);
	}
}

AdaptiveAlphabet::~AdaptiveAlphabet()
{
	for (int i = 0; i < 9; i++)
		cout << weights[i] << "\t";
	cout << "\n";
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
	ull cumulativeCounts[MAX_DEPTH];
	ull totalCounts[MAX_DEPTH];
	for (int j = 0; j < MAX_DEPTH - 1; j++)
	{
		probModels.GetCumulativeProbability(tCumulativeCount, tC, encodedSequence, j+1);
		totalCount += tC * weights[j];
		cumulativeCount += tCumulativeCount * weights[j];
		norm += weights[j];
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
	ull norm = 0;
	ull cumProb[10];
	ull totCount[10];
	for (int j = 0; j < MAX_DEPTH - 1; j++)
	{
		probModels.GetCumulativeProbability(cumProb[j], totCount[j], encodedSequence, j + 1);
		totalCount += totCount[j] * weights[j];
		//cumulativeCount += cumProb[j] * weights[j];
		norm += weights[j];
	};
	int maxIndex = 0;
	for (int i = 1; i < MAX_DEPTH - 1; i++)
	{
		if (cumProb[i] * totCount[maxIndex] > cumProb[maxIndex] * totCount[i])
			maxIndex = i;
	}
	weights[maxIndex]++;
	totalCount = totalCount / norm;// >> 1;
}


ull AdaptiveAlphabet::GetTotalCount()
{
	return this->totalCount;
}