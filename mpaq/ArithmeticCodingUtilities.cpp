#include "stdafx.h"
#include "ArithmeticCodingUtilities.h"

void GetLowAndHigh(int k, int p1, int & low, int & high)
{
	assert(k == 0 || k == 1);
	int p0 = 4096 - p1;
	if (p1 == 0)
	{
		p1++;
		p0--;
	}
	if (p0 == 0)
	{
		p0++;
		p1--;
	}
	switch (k)
	{
	case 0:
		low = 0;
		high = p0;
		break;
	case 1:
		low = p0;
		high = p0 + p1;
		break;
	default:
		break;
	}
}

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