#include "stdafx.h"
#include "ArithmeticDecoder.h"
#include "ArithmeticCodingUtilities.h"

ArithmeticDecoder::ArithmeticDecoder(Alphabet * alpha)
{
	alphabet = alpha;
	maximumLog2Precision = max((int)ceil(log2(alphabet->totalCount)) + 2, MIN_PRECISION);
	maximumInteger = 1LL << maximumLog2Precision;
	msbMask = 1LL << (maximumLog2Precision - 1);
	msb2Mask = 1LL << (maximumLog2Precision - 2);
	l = 0;
	u = maximumInteger - 1;
}

void ArithmeticDecoder::rescale()
{
	bool first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
	bool second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	while (first || second)
	{
		if (first)
		{
			l = (l * 2) % maximumInteger;
			u = (u * 2) % maximumInteger + 1;
			tag = (tag * 2) % maximumInteger + (int)(*input)[currIndex];
			int j = (int)maximumInteger;
			currIndex++;
		}
		if (second)
		{
			l = (l * 2) % maximumInteger;
			u = (u * 2) % maximumInteger + 1;
			tag = (tag * 2) % maximumInteger + (int)(*input)[currIndex];
			currIndex++;
			complementMaskedBits(l, this->maximumLog2Precision - 1);
			complementMaskedBits(u, this->maximumLog2Precision - 1);
			complementMaskedBits(tag, this->maximumLog2Precision - 1);
		}
		first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
		second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	}

}

void ArithmeticDecoder::writeBitsToVector(long k, vector<bool>& output)
{
	int numBitsPerWord = ceil(log2(alphabet->numCharacters - 1));
	int multiple = 1 << (numBitsPerWord - 1);
	int numDiv = numBitsPerWord;
	while (numDiv > 0)
	{
		output.push_back(k & multiple);
		multiple /= 2;
		numDiv--;
	}
}

vector<bool> ArithmeticDecoder::DecodeSequence(vector<bool>& sequenceToDecode)
{
	input = &sequenceToDecode;
	l = 0;
	u = maximumInteger - 1;
	tag = 0;
	vector<bool> output;

	/* Get initial (maxLog2Precision-1) bits of the tag.	*/
	for (currIndex = 0; currIndex < maximumLog2Precision; currIndex++)
	{
		if((*input)[currIndex])
			setBit(tag, maximumLog2Precision - 1 - currIndex);
	}
	while (currIndex < sequenceToDecode.size())
	{
		long k = -1;
		bool tagInKInterval = false;
		ull temp = ((tag - l + 1) * alphabet->totalCount - 1) / (u - l + 1);
		do
		{
			k++;
			ull kCumulativeCount = alphabet->GetComulativeCount(k);
			tagInKInterval = (temp < kCumulativeCount);
		} while (!tagInKInterval);
		if (k == alphabet->GetEOFCharacter())
		{
			break;
		}
		writeBitsToVector(k, output);
		// decoding for k finished, update alphabet with the result
		alphabet->Update(k);
		ull currentIntervalLength = u - l + 1;
		ull lowIncrement = currentIntervalLength * alphabet->GetComulativeCount(k - 1) / alphabet->totalCount;
		ull highIncrement = currentIntervalLength * alphabet->GetComulativeCount(k) / alphabet->totalCount;
		u = l + highIncrement - 1;
		l = l + lowIncrement;

		rescale();
	}
	return output;
}