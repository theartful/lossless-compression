#pragma once
#include "stdafx.h"

class Alphabet
{
public:
	unsigned long int numCharacters;
	long int totalCount;
	Alphabet(unsigned long int numChars);
	virtual long int GetComulativeCount(long i) = 0;
	virtual void Update(int lastSeenWord) = 0;
	long int GetEOFCharacter();
};