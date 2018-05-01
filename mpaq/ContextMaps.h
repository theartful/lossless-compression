#pragma once
#include "RunContextMap.h"
#include "NonstationaryContextMaps.h"

/* Combines a NonstationaryContextMap and a RunContextMap. */
class ContextMap
{
	RunContextMap runContextMap;
	NonstationaryContextMap nonstationaryContextMap;
	bool lastStateNonZero;
public:
	ContextMap(int mem, ContextInfo* contextInfo);
	void SetContext(uint context);
	/* Returns whether the last state was nonzero. */
	int GetPredictions(int& pred1, int& pred2);
	//int MixPrediction(MixerNN& mixer);
};

/* Combines a SmallNonstationaryContextMap and a RunContextMap. */
class SmallContextMap
{
	RunContextMap runContextMap;
	SmallNonstationaryContextMap nonstationaryContextMap;
	bool lastStateNonZero;
public:
	SmallContextMap(int mem, ContextInfo* contextInfo);

	void SetContext(uint context);

	/* Returns whether the last state was nonzero. */
	int GetPredictions(int& pred1, int& pred2);
	//int MixPrediction(MixerNN& mixer);
};