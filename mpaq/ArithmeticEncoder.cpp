#include "stdafx.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticCodingUtilities.h"

ArithmeticEncoder::ArithmeticEncoder(ProbabilityModel * model)
{
	this->model = model;
	numE3Scalings = 0;
	maximumLog2Precision = MAX_CODING_PRECISION;
	maximumInteger = (1LL << maximumLog2Precision) - 1;
	l = 0;
	u = maximumInteger;
}

vector<bool> ArithmeticEncoder::EncodeSequence(vector<bool> inputSequence, bool printProgress)
{
	output.clear();
	l = 0;
	u = maximumInteger;

	/* Write header. */
	static const ull MAX_SIZE = 1 << (maximumLog2Precision - 1);
	ull sizeInBytes = ceil(inputSequence.size() / 8.0);
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
	for (int k : inputSequence)
	{
		int low;
		int high;
		GetLowAndHigh(k, model->GetProbability(1), low, high);
		ull currentIntervalLength = u - l + 1;
		ull lowIncrement = currentIntervalLength * low / model->GetTotalCount();
		ull highIncrement = currentIntervalLength * high / model->GetTotalCount();
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		assert(u > l);
		Rescale();
		model->Update(k); // encoding for wordToEncode finished, update probability model with the result.
	}
	ull tag = (u + l) >> 1;
	/* Write what's left of the tag. */
	for (int i = 0; i < maximumLog2Precision; i++)
	{
		bool currentBit = getMaskedBit(tag, this->maximumLog2Precision - 1);
		output.push_back(currentBit);
		tag = (tag << 1) & maximumInteger; //(l * 2) % maximumInteger;
		while (numE3Scalings > 0)
		{
			output.push_back(!currentBit);
			numE3Scalings--;
		}
	}
	return output;
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