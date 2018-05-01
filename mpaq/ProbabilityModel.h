#pragma once
#include "stdafx.h"

class ProbabilityModel
{
public:
	/* Gets a probability between 0 and 4096. */
	virtual uint GetProbability(short bit) = 0;
	/* Gets a cumulative probability between 0 and 4096. */
	virtual uint GetCumulativeProbability(short bit) = 0;
	/* Update with the last seen bit. */
	virtual void Update(int lastBit) = 0;
	/* Returns the total count (always 4096 in this program). */
	virtual uint GetTotalCount() { return 4096; };
};