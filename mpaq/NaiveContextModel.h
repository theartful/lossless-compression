#pragma once
#include "ProbabilityModel.h"
#include <unordered_map>
#include <utility>
using std::pair;
using std::unordered_map;
#include "StateMap.h"

struct ContextStateInfo
{
	uint numZeros;
	uint numOnes;
	ContextStateInfo()
	{
		numZeros = 0;
		numOnes = 0;
	}
	void Update(int bit)
	{
		assert(bit == 0 || bit == 1);
		if (bit == 0)
		{
			numZeros++;
		}
		else if (bit == 1)
		{
			numOnes++;
		}
		if (numZeros > 65536 || numOnes > 65536)
		{
			numZeros >>= 1;
			numOnes >>= 1;
		}
	}
	int GetProbability()
	{
		return 65536 * (numZeros+1) / (numZeros + numOnes + 2);
	}
};

class NaiveContextModel
{
public:
	unordered_map<ull, ContextStateInfo> contextMap;
	int GetProbability(ull context, int lastBit);
};