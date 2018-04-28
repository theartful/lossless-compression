#include "stdafx.h"
#include "ArithmeticDecoder.h"
#include "ArithmeticCodingUtilities.h"
#include <limits.h>

ArithmeticDecoder::ArithmeticDecoder(Alphabet * alpha)
{
	alphabet = alpha;
	maximumLog2Precision = MAX_PRECISION;
	maximumInteger = (1LL << maximumLog2Precision) - 1;
	l = 0;
	u = maximumInteger;
	currIndex = 0;
}

void ArithmeticDecoder::Rescale()
{
	bool first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
	bool second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	while (first || second)
	{
		if (first)
		{
			l = (l << 1);
			u = (u << 1) + 1;
			tag = (tag << 1) + (int)(*input)[currIndex];
			l = l & maximumInteger;
			u = u & maximumInteger;
			tag = tag & maximumInteger;
			currIndex++;
		}
		if (second)
		{
			l = (l << 1);
			u = (u << 1) + 1;
			tag = (tag << 1) + (int)(*input)[currIndex];
			l = l & maximumInteger;
			u = u & maximumInteger;
			tag = tag & maximumInteger;
			currIndex++;
			complementMaskedBits(l, this->maximumLog2Precision - 1);
			complementMaskedBits(u, this->maximumLog2Precision - 1);
			complementMaskedBits(tag, this->maximumLog2Precision - 1);
		}
		first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
		second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	}
}

vector<bool> ArithmeticDecoder::DecodeSequence(vector<bool>& sequenceToDecode)
{
	vector<bool> output;
	input = &sequenceToDecode;
	l = 0;
	u = maximumInteger;
	tag = 0;
	currIndex = 0;

	/* Read the header. */
	static const uint MAX_SIZE = 1LL << (MAX_PRECISION - 1);
	ull fileSize = 0;
	for (uint i = 1; i < MAX_SIZE; i = i << 1)
	{
		if ((*input)[currIndex])
			setBit(fileSize, currIndex);
		currIndex++;
	}
	fileSize *= 8;
	/* Get initial (maxLog2Precision) bits of the tag.	*/
	for (int i = 0; i < maximumLog2Precision; i++, currIndex++)
	{
		if((*input)[currIndex])
			setBit(tag, maximumLog2Precision - 1 - i);
	}
	for(ull i = 0; i < fileSize; i++)
	{
		int k = -1;
		bool tagInKInterval = false;
		while (!tagInKInterval)
		{
			k++;
			ull temp = ((tag - l + 1) * alphabet->GetTotalCount() - 1) / (u - l + 1);
			ull kCumulativeCount = alphabet->GetComulativeCount(k);
			tagInKInterval = (temp < kCumulativeCount);
		}
		if (k > 1)
		{
			cout << "Error: k > 1.\n";
			throw 0;
		}
		output.push_back(k);
		ull currentIntervalLength = u - l + 1;
		ull lowIncrement = currentIntervalLength * alphabet->GetComulativeCount(k - 1) / alphabet->GetTotalCount();
		ull highIncrement = currentIntervalLength * alphabet->GetComulativeCount(k) / alphabet->GetTotalCount();
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		// decoding for k finished, update alphabet with the result
		alphabet->Update(k);
		Rescale();
	}
	return output;
}