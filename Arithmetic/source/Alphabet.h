#pragma once
#include "stdafx.h"

struct Alphabet
{
	unsigned long int numCharacters = 0;
	vector<unsigned long int> cumulativeCount;
	long int totalCount = 0;
	Alphabet(unsigned long int numChars) : numCharacters(numChars)
	{
		for (unsigned long int i = 0; i < numCharacters; i++)
			cumulativeCount.push_back(i + 1);
		totalCount = numChars;
	}
	long int getCumulativeCount(long i)
	{
		if (i < 0)
			return 0;
		return cumulativeCount[i];
	}
	long int GetEOFCharacter()
	{
		return numCharacters - 1;
	}
};