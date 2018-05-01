#include "stdafx.h"
#include "NaiveContextModel.h"

int NaiveContextModel::GetProbability(ull context, int lastBit)
{
	if (contextMap.find(context) == contextMap.end())
	{
		ContextStateInfo* csi = new ContextStateInfo();
		contextMap[context] = *csi;
	}
	contextMap[context].Update(lastBit);
	return contextMap[context].GetProbability() >> 4;
}
