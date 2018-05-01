#pragma once
#include "stdafx.h"
#include "HashMap.h"
#include "StateMap.h"
#include "ContextInfo.h"

/* A context map which doesn't actually store the last n0,n1 counts but */
/* instead uses a state table. Uses a single byte per context.			*/
class NonstationaryContextMap
{
	uint context;	// context hash.
	uchar* contextState;	// pointer to context state.
	HashMap<16>	bitHistory; // hash table mapping context to bit history.
	StateMap stateMap;
	ContextInfo* contextInfo;
public:
	NonstationaryContextMap(int mem, ContextInfo* contextInfo);
	int UpdateState(); // updates state and gets the next state.
	void SetContext(uint contextHash);
	int GetPrediction();
	bool IsStateGreaterThanZero();
	//int MixPrediction(MixerNN& mixer); // updates state, adds prediction to MixerNN, returns state > 0.
};

/* Same as above, except that the context is not hashed and is kept directly. */
class SmallNonstationaryContextMap
{
	uint context;	// the current context.
	uchar* contextState;	  // pointer to current context state.
	vector<uchar> bitHistory; // mapping of context to context states.
	StateMap stateMap;
	ContextInfo* contextInfo;
public:
	SmallNonstationaryContextMap(int mem, ContextInfo* contextInfo);
	int UpdateState(); // updates state and gets the next state.
	void SetContext(uint context);
	int GetPrediction();
	bool IsStateGreaterThanZero();
	//int MixPrediction(MixerNN& mixer);
};