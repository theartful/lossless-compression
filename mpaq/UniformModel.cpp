#include "stdafx.h"
#include "UniformModel.h"

uint UniformModel::GetProbability(short bit)
{
	if (bit < 0 || bit > 1)
		return 0;
	else
		return 2048;
}

uint UniformModel::GetCumulativeProbability(short bit)
{
	if (bit < 0)
		return 0;
	else if (bit == 0)
		return 2048;
	else
		return 4096;
}

void UniformModel::Update(int lastBit)
{
	/* We do nothing in the uniform model. */
}
