#include "stdafx.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticCodingUtilities.h"

ArithmeticEncoder::ArithmeticEncoder(Alphabet* alpha)
{
	alphabet = alpha;
	numE3Scalings = 0;
	maximumLog2Precision = ceil(log2(alphabet->totalCount)) + 2;
	maximumInteger = 1LL << maximumLog2Precision;
	msbMask = 1LL << (maximumLog2Precision-1);
	msb2Mask = 1LL << (maximumLog2Precision-2);
	l = 0;
	u = maximumInteger - 1;
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
			l = (l * 2) % maximumInteger;
			u = (u * 2) % maximumInteger + 1;
			while (numE3Scalings > 0)
			{
				output.push_back(!b);
				numE3Scalings--;
			}
		}
		if (second)
		{
			l = (l * 2) % maximumInteger;
			u = (u * 2) % maximumInteger + 1;
			complementMaskedBits(l, this->maximumLog2Precision - 1);
			complementMaskedBits(u, this->maximumLog2Precision - 1);
			numE3Scalings++;
		}
		first = (getMaskedBit(u, maximumLog2Precision - 1) == getMaskedBit(l, maximumLog2Precision - 1));
		second = (!getMaskedBit(u, maximumLog2Precision - 2) && getMaskedBit(l, maximumLog2Precision - 2));
	}
}

vector<bool> ArithmeticEncoder::EncodeSequence(vector<bool> inputSequence)
{
	output.clear();
	output.reserve(SIZE_MAX);
	l = 0;
	u = maximumInteger - 1;
	int progress = 0;
	size_t increment = 0;
	int practicalNumChars = alphabet->numCharacters - 1;
	int numBitsPerWord = ceil(log2(practicalNumChars));
	vector<int> sequenceToEncode;
	for (auto i = 0; i < inputSequence.size(); )
	{
		auto j = (inputSequence.size() - i < numBitsPerWord) ? inputSequence.size() - i : numBitsPerWord;
		int tempNum = 0;
		int multiple = 1 << (numBitsPerWord-1);
		for (; j > 0; j--)
		{
			tempNum += inputSequence[i] * multiple;
			i++;
			multiple /= 2;
		}
		sequenceToEncode.push_back(tempNum);
	}
	for (auto wordToEncode : sequenceToEncode)
	{
		//cout << "Encoding " << wordToEncode << "...\n";
		int unt = static_cast<int>(u);
		int lnt = static_cast<int>(l);
		ull temp = u - l + 1;
		ull lowIncrement = (temp * alphabet->getCumulativeCount(wordToEncode - 1)) / alphabet->totalCount;
		ull highIncrement = (temp * alphabet->getCumulativeCount(wordToEncode)) / alphabet->totalCount;
		int t = static_cast<int>(temp);
		int li = static_cast<int>(lowIncrement);
		int hi = static_cast<int>(highIncrement);
		u = l + highIncrement - 1;
		l = l + lowIncrement;
		rescale();
		if (progress >= increment)
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
		l = (l * 2) % maximumInteger;
		while (numE3Scalings > 0)
		{
			output.push_back(!currentBit);
			numE3Scalings--;
		}
	}
	return output;
}