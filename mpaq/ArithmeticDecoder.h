#pragma once
#include "stdafx.h"
#include "ProbabilityModel.h"

class ArithmeticDecoder
{
public:
	ArithmeticDecoder(ProbabilityModel* model);
	vector<bool> ArithmeticDecoder::DecodeSequence(vector<bool>& sequenceToDecode);
private:
	ProbabilityModel* model;
	int maximumLog2Precision;
	ull maximumInteger;
	ull l;
	ull u;
	ull tag;
	int currIndex;
	vector<bool>* input;
	void Rescale(); // scales [l, u) to be in [0, 1) instead of [0, 0.5) or [0.5, 1) or [0.25, 0.75).
};
