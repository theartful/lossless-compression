#pragma once
#include "ProbabilityModel.h"

class UniformModel : public ProbabilityModel
{
public:
	/* Gets a probability between 0 and 4096. */
	virtual uint GetProbability(short bit);
	/* Gets a cumulative probability between 0 and 4096. */
	virtual uint GetCumulativeProbability(short bit);
	/* Update with the last seen bit. */
	virtual void Update(int lastBit);
};