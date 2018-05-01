#include "stdafx.h"
#include "StateMap.h"
#include "StateTable.h"

StateMap::StateMap() : previousState(0)
{
	for (int i = 0; i < 256; i++)
	{
		/* P(state) = 2^16 * c1/c where c1 = n1 + 1, c0 = n0 + 1, c = c0 + c1, where state = (n0, n1). */
		lookUpTable[i] = 65536 * (StateInfo(i, 3) + 1) / (StateInfo(i, 2) + StateInfo(i, 3) + 2);\
	}
}

int StateMap::GetPrediction(int state, int lastBit)
{
	assert(state >= 0 && state < 256);
	/* This error is on the order of +/- 0.5, which is represented by +/- 2^8 here */
	/* (since the prob. in the lookup table are on order 2^16). We take the ceiling. */
	int error = ((lastBit << 16) - lookUpTable[previousState] + 128) >> 8;
	lookUpTable[previousState] += error;
	previousState = state;
	return lookUpTable[state] >> 4;
}