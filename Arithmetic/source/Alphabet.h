#pragma once
#include "stdafx.h"

class Alphabet
{
protected:
	ull totalCount;
public:
	unsigned long int numCharacters;
	Alphabet(unsigned long int numChars);
	virtual long int GetComulativeCount(long i) = 0;
	virtual void Update(int lastSeenWord) = 0;
	virtual ull GetTotalCount() = 0;
	long int GetEOFCharacter();
};