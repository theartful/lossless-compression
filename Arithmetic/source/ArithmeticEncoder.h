#pragma once
#include "stdafx.h"
#include "Alphabet.h"

class ArithmeticEncoder
{
public:
	ArithmeticEncoder(Alphabet* alpha);
	vector<bool> EncodeSequence(vector<bool> inputSequence, bool printProgress = false);
private:
	Alphabet* alphabet;
	int numE3Scalings;
	int maximumLog2Precision;
	ull maximumInteger;
	ull l;
	ull u;
	vector<bool> output;
	void Rescale(); // scales [l, u) to be in [0, 1) instead of [0, 0.5) or [0.5, 1).
};
