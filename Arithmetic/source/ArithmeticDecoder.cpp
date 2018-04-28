#include "stdafx.h"
#include "ArithmeticDecoder.h"
#include "ArithmeticCodingUtilities.h"
#include <limits.h>
ArithmeticDecoder::ArithmeticDecoder(Alphabet * alpha)
{
	alphabet = alpha;
	maximumLog2Precision = MAX_PRECISION; //40; //(int)ceil(log2(alphabet->GetTotalCount())) + 2; //max((int)ceil(log2(alphabet->GetTotalCount())) + 2, MIN_PRECISION);
	maximumInteger = (1LL << maximumLog2Precision) - 1;
	msbMask = 1LL << (maximumLog2Precision - 1);
	msb2Mask = 1LL << (maximumLog2Precision - 2);
	l = 0;
	u = maximumInteger;
}

void ArithmeticDecoder::rescale()
{
	bool first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
	bool second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	while (first || second)
	{
		if (first)
		{
			l = (l << 1); //(l * 2) % maximumInteger;
			u = (u << 1) + 1;// (u * 2) % maximumInteger + 1;
			tag = (tag << 1) + (int)(*input)[currIndex]; //(tag * 2) % maximumInteger + (int)(*input)[currIndex];
			l = l & maximumInteger;
			u = u & maximumInteger;
			tag = tag & maximumInteger;
			currIndex++;
		}
		if (second)
		{
			l = (l << 1); //(l * 2) % maximumInteger;
			u = (u << 1) + 1;// (u * 2) % maximumInteger + 1;
			tag = (tag << 1) + (int)(*input)[currIndex]; //(tag * 2) % maximumInteger + (int)(*input)[currIndex];
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

void ArithmeticDecoder::writeBitsToVector(long k, vector<bool>& output)
{
	int numBitsPerWord = ceil(log2(alphabet->numCharacters));
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
	u = maximumInteger;
	tag = 0;
	vector<bool> output;
	static const uint MAX_SIZE = 1LL << (MAX_PRECISION - 1);
	ull sizeInBytes = 0;
	int j = 0;
	currIndex = 0;
	for (uint i = 1; i < MAX_SIZE; i = i << 1)
	{
		if ((*input)[currIndex])
			setBit(sizeInBytes, currIndex);
		currIndex++;
	}
	cout << "Decoding: size = " << sizeInBytes << "\n";
	/* Get initial (maxLog2Precision) bits of the tag.	*/
	for (int i = 0; i < maximumLog2Precision; i++, currIndex++)
	{
		if((*input)[currIndex])
			setBit(tag, maximumLog2Precision - 1 - i);
	}
	for(ull i = 0; i < sizeInBytes*8; i++)
	{
		long k = -1;
		bool tagInKInterval = false;
		ull kCumulativeCount;
		do
		{
			k++;
			ull temp = ((tag - l + 1) * alphabet->GetTotalCount() - 1) / (u - l + 1);
			kCumulativeCount = alphabet->GetComulativeCount(k);
			tagInKInterval = (temp < kCumulativeCount);
		} while (!tagInKInterval);
		if (k >= alphabet->numCharacters)
		{
			cout << "We should never be here.\n";
		}
		//cout << k << "\t";
		//cout << k << "\t" << "kCumulativeCount = " << kCumulativeCount << "\t";
		//cout << "totalCount = " << alphabet->GetTotalCount() << "\n";
		writeBitsToVector(k, output);
		ull currentIntervalLength = u - l + 1;
		ull lowIncrement = currentIntervalLength * alphabet->GetComulativeCount(k - 1); // / alphabet->GetTotalCount();
		ull highIncrement = currentIntervalLength * alphabet->GetComulativeCount(k); // alphabet->GetTotalCount();
		lowIncrement = lowIncrement / alphabet->GetTotalCount();
		highIncrement = highIncrement / alphabet->GetTotalCount();
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		// decoding for k finished, update alphabet with the result
		alphabet->Update(k);
		//cout << "u = " << u << "\n";
		//cout << "l = " << l << "\n";

		rescale();
	}
	return output;
}