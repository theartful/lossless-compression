#include "stdafx.h"
#include "ContextMaps.h"

ContextMap::ContextMap(int mem, ContextInfo* contextInfo) : runContextMap(mem / 4, contextInfo), nonstationaryContextMap(mem, contextInfo)
{

}

void ContextMap::SetContext(uint context)
{
	runContextMap.SetContext(context);
	nonstationaryContextMap.SetContext(context);
}

int ContextMap::GetPredictions(int& pred1, int& pred2)
{
	pred1 = runContextMap.GetPrediction();
	pred2 = nonstationaryContextMap.GetPrediction();
	return runContextMap.IsStateGreaterThanZero();
}

SmallContextMap::SmallContextMap(int mem, ContextInfo* contextInfo) : runContextMap(mem / 128, contextInfo), nonstationaryContextMap(mem, contextInfo)
{

}

void SmallContextMap::SetContext(uint context)
{
	runContextMap.SetContext(context);
	nonstationaryContextMap.SetContext(context);
}

int SmallContextMap::GetPredictions(int & pred1, int & pred2)
{
	pred1 = runContextMap.GetPrediction();
	pred2 = nonstationaryContextMap.GetPrediction();
	return runContextMap.IsStateGreaterThanZero();
}