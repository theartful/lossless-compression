#pragma once
#include "Alphabet.h"

class ArithmeticDecoder
{
public:
	ArithmeticDecoder(Alphabet* alpha);
	vector<bool> ArithmeticDecoder::DecodeSequence(vector<bool>& sequenceToDecode);
private:
	Alphabet* alphabet;
	int maximumLog2Precision;
	ull maximumInteger;
	ull l;
	ull u;
	ull tag;
	int currIndex;
	vector<bool>* input;
	void Rescale(); // scales [l, u) to be in [0, 1) instead of [0, 0.5) or [0.5, 1).
};
