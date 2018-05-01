#include "stdafx.h"
#include "NonstationaryContextMaps.h"
#include "StateTable.h"

int NonstationaryContextMap::GetPrediction()
{
	int state = UpdateState();
	return stateMap.GetPrediction(state, contextInfo->lastBit);
}
bool NonstationaryContextMap::IsStateGreaterThanZero()
{
	return *contextState > 0;
}
void NonstationaryContextMap::SetContext(uint contextHash)
{
	this->context = contextHash;
}
NonstationaryContextMap::NonstationaryContextMap(int mem, ContextInfo* contextInfo) : context(0), bitHistory(mem / 16)
{
	assert(contextInfo != nullptr);
	contextState = bitHistory[0] + 1;
	this->contextInfo = contextInfo;
}

int NonstationaryContextMap::UpdateState()
{
	*contextState = StateInfo(*contextState, contextInfo->lastBit);
	if (contextInfo->bitPosition & 3)
	{
		contextState = contextState + ((contextInfo->lastBit + 1) << ((contextInfo->bitPosition & 3) - 1));
	}
	else
	{
		contextState = bitHistory[context ^ contextInfo->lastByte * 9] + 1;
	}
	return *contextState;
}


SmallNonstationaryContextMap::SmallNonstationaryContextMap(int mem, ContextInfo* contextInfo) : context(0), bitHistory((mem+1)/2)
{
	assert(contextInfo != nullptr);
	contextState = &bitHistory[0];
	this->contextInfo = contextInfo;
}

int SmallNonstationaryContextMap::UpdateState()
{
	*contextState = StateInfo(*contextState, contextInfo->lastBit);
	contextState = &bitHistory[context + contextInfo->lastByte];
	return *contextState;
}

void SmallNonstationaryContextMap::SetContext(uint context)
{
	this->context = context * 256 & bitHistory.size() - 256;
}

int SmallNonstationaryContextMap::GetPrediction()
{
	int state = UpdateState();
	return stateMap.GetPrediction(state, contextInfo->lastBit);
}

bool SmallNonstationaryContextMap::IsStateGreaterThanZero()
{
	return *contextState > 0;
}
