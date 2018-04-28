#include "stdafx.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticCodingUtilities.h"
#include <limits.h>

ArithmeticEncoder::ArithmeticEncoder(Alphabet* alpha)
{
	alphabet = alpha;
	numE3Scalings = 0;
	maximumLog2Precision = MAX_PRECISION; //40; //(int)ceil(log2(alphabet->GetTotalCount())) + 2; //max((int)ceil(log2(alphabet->GetTotalCount())) + 2, MIN_PRECISION);
	maximumInteger = (1LL << maximumLog2Precision) - 1;
	msbMask = 1LL << (maximumLog2Precision-1);
	msb2Mask = 1LL << (maximumLog2Precision-2);
	l = 0;
	u = maximumInteger;
}

void ArithmeticEncoder::rescale()
{
	bool first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
	bool second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	while (first || second)
	{
		if (first)
		{
			bool b = getMaskedBit(u, this->maximumLog2Precision - 1);
			output.push_back(b);
			l = l << 1; //(l * 2) % maximumInteger;
			u = (u << 1) + 1; //(u * 2) % maximumInteger + 1;
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
			l = l << 1; //(l * 2) % maximumInteger;
			u = (u << 1) + 1; //(u * 2) % maximumInteger + 1;
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
	int progress = 0;
	size_t increment = 0;
	int practicalNumChars = alphabet->numCharacters - 1;
	int numBitsPerWord = ceil(log2(practicalNumChars));
	vector<int> sequenceToEncode;
	for (int i = 0; i < inputSequence.size(); i++)
	{
		sequenceToEncode.push_back(inputSequence[i]);
	}
	/* Write header. */
	static const uint MAX_SIZE = 1LL << (MAX_PRECISION - 1);
	uint sizeInBytes = ceil(sequenceToEncode.size() / 8.0);
	if (sizeInBytes > MAX_SIZE)
	{
		cout << "Error: can not compress files larger than " << MAX_SIZE / 1024 << " megabytes.\n";
		return output;
	}
	for (uint i = 1; i < MAX_SIZE; i = i << 1)
	{
		output.push_back(sizeInBytes & i);
	}
	//sequenceToEncode.push_back(alphabet->GetEOFCharacter());
	for (auto wordToEncode : sequenceToEncode)
	{
		//cout << wordToEncode << "\t";
		ull temp = u - l + 1;
		ull lowIncrement = (temp * alphabet->GetComulativeCount(wordToEncode - 1));
		ull highIncrement = (temp * alphabet->GetComulativeCount(wordToEncode));
		ull tCount = alphabet->GetTotalCount();
		lowIncrement /= tCount;
		highIncrement /= tCount;

		u = l + highIncrement - 1;
		l = l + lowIncrement;
		if (u <= l)
		{
			cout << "NO.\t";
			cout << u << "\t" << l << "\n";
			throw 0;
		}
		rescale();
		// encoding for wordToEncode finished, update alphabet with the result
		alphabet->Update(wordToEncode);
		if (progress >= increment && printProgress)
		{
			cout << "Encoding progress = " << progress*100.0f / sequenceToEncode.size() << "% \n";
			increment += sequenceToEncode.size() / 10;
		}
		progress++;
	}
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