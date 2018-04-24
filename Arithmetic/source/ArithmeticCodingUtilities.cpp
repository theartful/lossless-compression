#include "stdafx.h"
#include "ArithmeticCodingUtilities.h"


void complementMaskedBits(ull& x, short bitNumber)
{
	auto mask = 1LL << bitNumber;
	if (x & mask)
		x = x & ~mask;
	else
		x = x | mask;
	//mpz_combit(x.backend().data(), bitNumber);
}

bool getMaskedBit(ull& x, short bitNumber)
{
	return (1LL << bitNumber) & x;
	//return mpz_tstbit(x.backend().data(), bitNumber);
}

void setBit(ull& x, short bitNumber)
{
	x = x | (1LL << bitNumber);
	//mpz_setbit(x.backend().data(), bitNumber);
}