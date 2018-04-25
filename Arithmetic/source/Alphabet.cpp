#include "stdafx.h"
#include "Alphabet.h"

Alphabet::Alphabet(unsigned long int numChars) : numCharacters(numChars)
{
	totalCount = numChars;
}

long int Alphabet::GetEOFCharacter()
{
	return numCharacters - 1;
}