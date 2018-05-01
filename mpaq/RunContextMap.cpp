#include "stdafx.h"
#include "RunContextMap.h"
#include "MathUtilities.h"

RunContextMap::RunContextMap(int maxRepeatCount, ContextInfo* contextInfo) : bitHistory(maxRepeatCount / 4)
{
	contextState = bitHistory[0] + 1;
	this->contextInfo = contextInfo;
}

void RunContextMap::SetContext(uint context)
{
	if (contextState[0] == 0 || contextState[1] != (contextInfo->buffer)(1))
	{
		contextState[0] = 1;
		contextState[1] = contextInfo->buffer(1);
	}
	else if (contextState[0] < 255)
	{
		contextState[0]++;
	}
	contextState = bitHistory[context] + 1;
}

int RunContextMap::UpdateState()
{
	if ((((contextState[1] + 256) >> 8) - contextInfo->bitPosition) == contextInfo->lastByte)
	{
		/* To-do: understand the math in here. */
		return ((contextState[1] >> 7 - contextInfo->bitPosition & 1) * 2 - 1) * ILog(contextState[0] + 1) * 8;
	}
	else
	{
		return 0;
	}
}

int RunContextMap::GetPrediction()
{
	return UpdateState();
}

bool RunContextMap::IsStateGreaterThanZero()
{
	return contextState[0] > 0;
}
