#pragma once
#include "stdafx.h"
#include "Alphabet.h"
#include "Trie.h"

/* All probability models operate on the bit level! */
class ProbabilityModel
{
public:
	/* Returns probability of word. */
	virtual uint GetProbability(uchar word) = 0;
	/* Returns the cumulative probability of word. */
	virtual uint GetCumulativeProbability(uchar word) = 0;
	/* Updates the probability model with the last word seen. */
	virtual void UpdateModel(uchar word) = 0;
};

class UniformModel : public ProbabilityModel
{
public:
	virtual uint GetProbability(uchar word)
	{
		return MAX_PROBABILITY >> 1;
	}
	virtual void UpdateModel(uchar word)
	{
		/* We do nothing! */
	}
	virtual uint GetCumulativeProbability(uchar word)
	{
		if (word < 0)
			return 0;
		return (word+1)*GetProbability(word);
	}
};

class ZeroOrderModel : public ProbabilityModel
{
	uint frequencies[3];
public:
	ZeroOrderModel()
	{
		frequencies[0] = 1;
		frequencies[1] = 1;
		frequencies[2] = 2;
	}
	virtual uint GetProbability(uchar word)
	{
		return frequencies[word] * MAX_PROBABILITY / frequencies[2];
	}
	virtual void UpdateModel(uchar word)
	{
		frequencies[word]++;
		frequencies[2]++;
		if (frequencies[2] > MAX_PROBABILITY)
		{
			frequencies[0] = frequencies[0] >> 1;
			frequencies[1] = frequencies[1] >> 1;
			while (frequencies[1] <= frequencies[0])
				frequencies[1]++;
			frequencies[2] = frequencies[0] + frequencies[1];
		}
	}
	virtual uint GetCumulativeProbability(uchar word)
	{
		uint frequency = 0;
		for (int i = 0; i <= word; i++)
		{
			frequency += frequencies[i];
		}
		return frequency / frequencies[2];
	}
};

/* Let f(x) = 1/1+exp(-x). Squash(x) = 2^12 * f(x*2^(-8)) */
/* Input is a integer (ideally with precision -2^11 to 2^11) */
/* and the output is in the range (0, 2^12). */
int Squash(int x)
{
	static const int lookUpTable[33] = {
		1,2,3,6,10,16,27,45,73,120,194,310,488,747,1101,
		1546,2047,2549,2994,3348,3607,3785,3901,3975,4022,
		4050,4068,4079,4085,4089,4092,4093,4094
	};

	if (x < -2047)
		return 0;
	else if (x > 2047)
		return 4095;

	int higherX = x & 127; // We'll use to interpolate x since our precision is in the 2^8 range.
	x = (x >> 7) + 16; // converts x to the [0, 32) range.

	/* We return ceil((lookUpTable[x] * (128-higherX)/128 + lookUpTable[x+1] * higherX / 128) */
	return (lookUpTable[x] * (128 - higherX) + lookUpTable[x + 1] * higherX + 64) >> 7;
}

class Stretch
{
private:
	int lookUpTable[4096];
public:
	Stretch()
	{
		int inv = 0;
		for (int i = -2047; i < 2048; i++)
		{
			int value = Squash(i);
			for (int j = inv; j <= value; j++)
			{
				lookUpTable[j] = i;
			}
			inv = value + 1;
		}
	}
	int operator()(int x)
	{
		if (x > 4096 || x < 0)
		{
			throw exception("x must be between 0 and 4096");
		}
		return lookUpTable[x];
	}
} stretch;

class MixerModel : public ProbabilityModel
{
	vector<ProbabilityModel*> models;
	vector<int> weights;
	double learningRate;
	bool initialized;
	double computedCumulatives[NUM_CHARACTERS];
	uint _GetProbability(uchar word)
	{
		int probability = 0;
		for (int i = 0; i < models.size(); i++)
		{
			int stretchedProbability = stretch(models[i]->GetProbability(word));
			probability += weights[i] * stretchedProbability;
		}
		probability = Squash(probability);
		cout << probability << "\n";
		return probability;
	}
	void _ComputeCumulatives()
	{
		/* Do our computations. */
		computedCumulatives[0] = MAX_PROBABILITY - _GetProbability(1);
		computedCumulatives[1] = MAX_PROBABILITY;
		cout << "F(0) = " << computedCumulatives[0] << "\n";
	}
public:
	MixerModel()
	{
		//ProbabilityModel* uniform = new UniformModel();
		//models.push_back(uniform);
		ProbabilityModel* zeroOrder = new ZeroOrderModel();
		models.push_back(zeroOrder);
		for (int i = 0; i < models.size(); i++)
			weights.push_back(1);
		_ComputeCumulatives();
		learningRate = 0.01;
	}
	~MixerModel()
	{
		for (int i = 0; i < models.size(); i++)
		{
			cout << "weights[" << i << "] = " << weights[i] << "\t";
		}
		cout << "\n";
		for (auto& model : models)
		{
			delete model;
		}
	}
	virtual uint GetProbability(uchar word)
	{
		if (word == 0)
			return GetCumulativeProbability(0);
		else
			return GetCumulativeProbability(word) - GetCumulativeProbability(word-1);
	}
	virtual uint GetCumulativeProbability(uchar word)
	{
		return computedCumulatives[word];
	}
	void StochasticGradientDescent(uchar word)
	{
		double crossEntropy = 0.0;
		int z = GetProbability(1);
		for (int i = 0; i < models.size(); i++)
		{
			int err = ((word << 12) - z) * 7;
			assert(err >= -32768 && err < 32768);
			int gradient = err * stretch(models[i]->GetProbability(1));
			//cout << "gradient = " << gradient << "\n";
			weights[i] = weights[i] + learningRate * gradient;
			cout << "wi = " << weights[i] << "\n";
		}
		crossEntropy = -log(z);
		//cout << crossEntropy << "\n";
	}

	virtual void UpdateModel(uchar word)
	{
		/* First do stochastic gradient descent. */
		StochasticGradientDescent(word);
		/* THEN update the models. */
		for (int i = 0; i < models.size(); i++)
		{
			models[i]->UpdateModel(word);
		}
		/* Find all the probabilities once. */
		_ComputeCumulatives();
	}
};

class MixerAlphabet : public Alphabet
{
private:
	MixerModel mixer;
protected:
	ull totalCount;
public:
	MixerAlphabet() : Alphabet()
	{
		totalCount = MAX_PROBABILITY;
	}
	virtual ull GetComulativeCount(long i)
	{
		if (i < 0)
			return 0;
		return mixer.GetCumulativeProbability(i);
	}
	virtual void Update(int lastSeenWord)
	{
		mixer.UpdateModel(lastSeenWord);
	}
	virtual ull GetTotalCount()
	{
		return totalCount;
	}
	long int GetEOFCharacter()
	{
		return totalCount - 1;
	}
};