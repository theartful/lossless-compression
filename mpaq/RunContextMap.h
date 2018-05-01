#pragma once
#include "stdafx.h"
#include "HashMap.h"
#include "Buffer.h"
#include "ContextInfo.h"

/* A run context model maps a context into the next byte and a repeat count up to maxRepeatCount. */
class RunContextMap
{
	HashMap<3> bitHistory;
	uchar* contextState;
	ContextInfo* contextInfo;
public:
	RunContextMap(int maxRepeatCount, ContextInfo* contextInfo);
	void SetContext(uint context);
	int UpdateState();
	int GetPrediction();
	bool IsStateGreaterThanZero();
	//int MixPrediction(MixerNN& mixer); // updates state, adds prediction to MixerNN, returns state > 0.
};
