#pragma once
#include "stdafx.h"
#include "ProbabilityModel.h"

class ArithmeticEncoder
{
public:
	ArithmeticEncoder(ProbabilityModel* model);
	vector<bool> EncodeSequence(vector<bool> inputSequence, bool printProgress = false);
private:
	ProbabilityModel* model;
	int numE3Scalings;
	int maximumLog2Precision;
	ull maximumInteger;
	ull l;
	ull u;
	vector<bool> output;
	void Rescale(); // scales [l, u) to be in [0, 1) instead of [0, 0.5) or [0.5, 1) or [0.25, 0.75).
};
