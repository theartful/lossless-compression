#include "stdafx.h"
#include "AdaptiveAlphabet.h"
ull evidenceArray[256];
ull frequencies[256][MAX_DEPTH];
ull totalEvidence = 0;

void AdaptiveAlphabet::CalculateGradient(int lastSeenWord)
{
	double gradients[MAX_DEPTH];
	int maxIndex = 0;
	for (int j = 0; j < MAX_DEPTH - 1; j++)
	{
		gradients[j] = 0;
		for (int i = 0; i < 256; i++)
		{
			gradients[j] += frequencies[i][j];
		}
		gradients[j] /= totalEvidence;
		gradients[j] -= (frequencies[lastSeenWord][j] / evidenceArray[j]);
		weights[j] -= gradients[j];
	}
	totalCount = totalEvidence;
}

AdaptiveAlphabet::AdaptiveAlphabet(unsigned long int numChars) : Alphabet(numChars)
{
	this->totalCount = 256;
	for (int i = 0; i < MAX_DEPTH; i++)
	{
		weights.push_back(1.0/(MAX_DEPTH-1));
	}
}

AdaptiveAlphabet::~AdaptiveAlphabet()
{
	for (int i = 0; i < MAX_DEPTH; i++)
		cout << weights[i] << "\t";
	cout << "\n";
	//probModels.PrintProbabilities();
}

long int AdaptiveAlphabet::ExperimentalGetComulativeCount(long word)
{
	if (word < 0)
		return 0;
	ull tempTotalCount = 0;
	for (int k = 0; k < 256; k++)
	{
		evidenceArray[k] = 1;
		encodedSequence.push_back(k); // We'll find the evidence for the k-th character.
		for (int j = 0; j < MAX_DEPTH - 1; j++)
		{
			frequencies[k][j] = probModels.GetProbability(tempTotalCount, encodedSequence, j + 1);
			evidenceArray[k] += weights[j] * frequencies[k][j];
		}
		encodedSequence.pop_back();
		totalEvidence += evidenceArray[k];
	}
	ull cumulativeCount = 0;
	for (int j = 0; j <= word; j++)
	{
		cumulativeCount += evidenceArray[j];
	}
	totalCount = totalEvidence;
	return cumulativeCount;
}

ull AdaptiveAlphabet::GetComulativeCount(long i)
{
	return ExperimentalGetComulativeCount(i);
	if (i < 0)
		return 0;
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
		//cout << weights[j] << "\t";
		totalCount += tC * weights[j];
		cumulativeCount += tCumulativeCount * weights[j];
		norm += weights[j];
	};
	cumulativeCount = cumulativeCount / norm;// >> 1;
	totalCount = totalCount / norm;// >> 1;
	// cout << cumulativeCount << "\t";
	// cout << totalCount << "\n";
	encodedSequence.pop_back();
	return cumulativeCount;
}

void AdaptiveAlphabet::Update(int lastSeenWord)
{
	encodedSequence.push_back(lastSeenWord);
	probModels.VisitString(encodedSequence);
	CalculateGradient(lastSeenWord);
	/*
	totalCount = 0;
	ull norm = 0;
	ull totalProb = 0;
	ull cumProb[10];
	ull totCount[10];
	for (int j = 0; j < MAX_DEPTH - 1; j++)
	{
		cumProb[j] = probModels.GetProbability(totCount[j], encodedSequence, j+1);
		totalCount += totCount[j] * weights[j];
		totalProb += cumProb[j];
		norm += weights[j];
	};
	int maxIndex = 0;
	for (int i = 0; i < MAX_DEPTH - 1; i++)
	{
		weights[i] = weights[i] * 2 + cumProb[i] * 2;
		weights[i] = weights[i] >> 2;
	}
	//weights[maxIndex]++;
	totalCount = totalCount / norm;// >> 1;
	*/
}


ull AdaptiveAlphabet::GetTotalCount()
{
	return this->totalCount;
}