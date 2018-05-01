#include "stdafx.h"
#include "ArithmeticDecoder.h"
#include "ArithmeticCodingUtilities.h"

ArithmeticDecoder::ArithmeticDecoder(ProbabilityModel* model)
{
	this->model = model;
	maximumLog2Precision = MAX_CODING_PRECISION;
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
	static const ull MAX_SIZE = 1LL << (maximumLog2Precision - 1);
	ull fileSize = 0;
	for (ull i = 1; i < MAX_SIZE; i = i << 1)
	{
		if ((*input)[currIndex])
			setBit(fileSize, currIndex);
		currIndex++;
	}
	fileSize *= 8;
	/* Get initial (maxLog2Precision) bits of the tag.	*/
	for (int i = 0; i < maximumLog2Precision; i++, currIndex++)
	{
		if ((*input)[currIndex])
			setBit(tag, maximumLog2Precision - 1 - i);
	}
	for (ull i = 0; i < fileSize; i++)
	{
		int low;
		int high;
		int k = -1;
		bool tagInKInterval = false;
		do
		{
			k++;
			ull temp = ((tag - l + 1) * model->GetTotalCount() - 1) / (u - l + 1);
			GetLowAndHigh(k, model->GetProbability(1), low, high);
			ull kCumulativeCount = high;
			tagInKInterval = (temp < kCumulativeCount);
		} while (!tagInKInterval);
		output.push_back(k);
		ull currentIntervalLength = u - l + 1;
		GetLowAndHigh(k, model->GetProbability(1), low, high);
		ull lowIncrement = currentIntervalLength * low / model->GetTotalCount();
		ull highIncrement = currentIntervalLength * high / model->GetTotalCount();
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		assert(u > l);
		Rescale();
		// decoding for k finished, update alphabet with the result
		model->Update(k);
	}
	return output;
}