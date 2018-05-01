#pragma once
#include "stdafx.h"
#include "StateTable.h"

/* A statemap maps one of the 256 states of a nonstationary context map to a probability.	   */
/* After each prediction, the mapping is updated with the error to improve future predictions. */
class StateMap
{
private:
	int previousState;
protected:
	ushort lookUpTable[256]; // lookUpTable[state] = Probability(State) * 2^16
public:
	StateMap();
	/* Get the probability (0 to 2^12) and update the map's predictions with the last bit. */
	int GetPrediction(int state, int lastBit);
};
