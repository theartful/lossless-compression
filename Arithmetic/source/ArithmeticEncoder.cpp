#include "stdafx.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticCodingUtilities.h"
#include <limits.h>

ArithmeticEncoder::ArithmeticEncoder(Alphabet* alpha)
{
	alphabet = alpha;
	numE3Scalings = 0;
	maximumLog2Precision = MAX_PRECISION;
	maximumInteger = (1LL << maximumLog2Precision) - 1;
	l = 0;
	u = maximumInteger;
}

void ArithmeticEncoder::Rescale()
{
	bool first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
	bool second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	while (first || second)
	{
		if (first)
		{
			bool b = getMaskedBit(u, this->maximumLog2Precision - 1);
			output.push_back(b);
			l = l << 1;
			u = (u << 1) + 1;
			l = l & maximumInteger;
			u = u & maximumInteger;
			while (numE3Scalings > 0)
			{
				output.push_back(!b);
				numE3Scalings--;
			}
		}
		if (second)
		{
			l = l << 1;
			u = (u << 1) + 1;
			l = l & maximumInteger;
			u = u & maximumInteger;
			complementMaskedBits(l, this->maximumLog2Precision - 1);
			complementMaskedBits(u, this->maximumLog2Precision - 1);
			numE3Scalings++;
		}
		first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
		second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	}
}

vector<bool> ArithmeticEncoder::EncodeSequence(vector<bool> inputSequence, bool printProgress)
{
	output.clear();
	l = 0;
	u = maximumInteger;

	/* Write header. */
	static const uint MAX_SIZE = 1LL << (MAX_PRECISION - 1);
	uint sizeInBytes = ceil(inputSequence.size() / 8.0);
	if (sizeInBytes > MAX_SIZE)
	{
		cout << "Error: can not compress files larger than " << MAX_SIZE / 1024 << " megabytes.\n";
		return output;
	}
	for (uint i = 1; i < MAX_SIZE; i = i << 1)
	{
		output.push_back(sizeInBytes & i);
	}
	/* Compress the file. */
	for (auto k : inputSequence)
	{
		ull currentIntervalLength = u - l + 1;
		ull lowIncrement = currentIntervalLength * alphabet->GetComulativeCount(k - 1) / alphabet->GetTotalCount();
		ull highIncrement = currentIntervalLength * alphabet->GetComulativeCount(k) / alphabet->GetTotalCount();
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		if (u <= l)
		{
			cout << "Error: u<=l: u = " << u << " l = " << l << "\n";
			throw 0;
		}
		Rescale();

		// encoding for wordToEncode finished, update alphabet with the result
		alphabet->Update(k);
	}

	/* Write what's left of the tag. */
	for (int i = 0; i < maximumLog2Precision; i++)
	{
		bool currentBit = getMaskedBit(l, this->maximumLog2Precision - 1);
		output.push_back(currentBit);
		l = (l << 1) & maximumInteger; //(l * 2) % maximumInteger;
		while (numE3Scalings > 0)
		{
			output.push_back(!currentBit);
			numE3Scalings--;
		}
	}
	return output;
}