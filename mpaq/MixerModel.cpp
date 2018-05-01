#include "stdafx.h"
#include "MixerModel.h"
#include "UniformModel.h"
#include "MathUtilities.h"
#include "ContextMaps.h"
#include "RunContextMap.h"
#include "Trie.h"
#include "NaiveContextModel.h"

MixerModel::MixerModel() : mixerNeuralNetwork(512, 1024, 4), scm1(MEMORY_USAGE / 8192, &contextInfo), scm2(MEMORY_USAGE / 32, &contextInfo),
	cm1(MEMORY_USAGE, &contextInfo), cm2(MEMORY_USAGE, &contextInfo), cm3(MEMORY_USAGE, &contextInfo), cm4(MEMORY_USAGE*2, &contextInfo),
	cm5(MEMORY_USAGE * 2, &contextInfo), cm6(MEMORY_USAGE * 2, &contextInfo), cm7(MEMORY_USAGE * 2, &contextInfo),
	rcm1(MEMORY_USAGE, &contextInfo), rcm2(MEMORY_USAGE, &contextInfo), rcm3(MEMORY_USAGE, &contextInfo)
{
	contextInfo.bitPosition = 0;
	contextInfo.lastBit = 0;
	contextInfo.lastByte = 1;
	contextInfo.lastFourBytes = 0;
	contextInfo.buffer.currentPosition = 0;
	computedProbabilities[0] = 2048;
	computedProbabilities[1] = 2048;
}

void MixerModel::MixContextMaps()
{
	if (contextInfo.bitPosition == 0) {
		int i = 0;
		// The first two context maps don't hash anything.
		scm1.SetContext(0);
		scm2.SetContext(contextInfo.buffer(1));
		// Hash the the last few bytes for each of the context maps.
		cm1.SetContext(Hash(++i, contextInfo.buffer(1), contextInfo.buffer(2)));
		cm2.SetContext(Hash(++i, contextInfo.buffer(1), contextInfo.buffer(2), contextInfo.buffer(3)));
		cm3.SetContext(Hash(++i, contextInfo.buffer(1), contextInfo.buffer(2), contextInfo.buffer(3), contextInfo.buffer(4)));
		uint h = Hash(++i, contextInfo.buffer(1), contextInfo.buffer(2), contextInfo.buffer(4), contextInfo.buffer(5));
		cm4.SetContext(h);

		// We run out of integers in our hash function here, so we starting hashing with the last hash. 
		h = Hash(++i, h, contextInfo.buffer(6));
		cm5.SetContext(h);
		h = Hash(++i, h, contextInfo.buffer(7));
		rcm1.SetContext(h);
		h = Hash(++i, h, contextInfo.buffer(8));
		cm6.SetContext(h);
		h = Hash(++i, h, contextInfo.buffer(9), contextInfo.buffer(10));
		rcm2.SetContext(h);
		h = Hash(++i, h, contextInfo.buffer(11), contextInfo.buffer(12));
		rcm3.SetContext(h);
		h = Hash(++i, h, contextInfo.buffer(13), contextInfo.buffer(14));
		cm7.SetContext(h);
	}
	int order = 0;
	int p1 = 0;
	int p2 = 0;

	mixerNeuralNetwork.AddInput(256);
	// For rcm's t = p. For cm's/scm's t1 = p1, t2 = Stretch(p2) >> 2.
	order += scm1.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += scm2.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += cm1.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += cm2.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += cm3.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += cm4.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	order += cm5.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	p1 = rcm1.GetPrediction();
	mixerNeuralNetwork.AddInput(p1);
	cm6.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	p1 = rcm2.GetPrediction();
	mixerNeuralNetwork.AddInput(p1);
	p1 = rcm3.GetPrediction();
	mixerNeuralNetwork.AddInput(p1);
	cm7.GetPredictions(p1, p2);
	mixerNeuralNetwork.AddInput(p1);
	mixerNeuralNetwork.AddInput(Stretch(p2) >> 2);
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.buffer(1) + 8, 264);
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.lastByte, 256);
	mixerNeuralNetwork.SetExpertHiddenUnit(order + 8 * (contextInfo.lastFourBytes >> 5 & 7) 
		+ 64 * (contextInfo.buffer(1) == contextInfo.buffer(2)), 128);
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.buffer(2), 256);
}

void MixerModel::MixContextModels()
{
	int p = 128;
	int i = 0;
	int h = 0;
	ull context = 0;
	mixerNeuralNetwork.AddInput(p);
	p = ncm0.GetProbability(context, contextInfo.lastBit) >> 4;
	mixerNeuralNetwork.AddInput(p >> 4);
	context += contextInfo.buffer(1);
	p = ncm1.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(2)) << 8);
	p = ncm2.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(3)) << 16);
	p = ncm3.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(4)) << 24);
	p = ncm4.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(4)) << 32);
	p = ncm5.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(5)) << 40);
	p = ncm6.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(6)) << 48);
	p = ncm7.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	context += (ull(contextInfo.buffer(7)) << 56);
	p = ncm8.GetProbability(context, contextInfo.lastBit);
	mixerNeuralNetwork.AddInput(p >> 4);
	/* We run out of places */
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.buffer(1) + 8, 264);
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.lastByte, 256);
	mixerNeuralNetwork.SetExpertHiddenUnit(4 + 8 * (contextInfo.lastFourBytes >> 5 & 7)
		+ 64 * (contextInfo.buffer(1) == contextInfo.buffer(2)), 128);
	mixerNeuralNetwork.SetExpertHiddenUnit(contextInfo.buffer(2), 256);
}

void MixerModel::ComputeProbabilities()
{
	MixContextMaps();
	//MixContextModels();
	computedProbabilities[1] = mixerNeuralNetwork.GetPrediction();
	//cout << computedProbabilities[1] << "\t";
	assert(computedProbabilities[1] >= 0 && computedProbabilities[1] < 4096);
	computedProbabilities[0] = 4096 - computedProbabilities[1];
}


uint MixerModel::GetProbability(short bit)
{
	if (bit < 0 || bit > 1)
		return 0;
	else
		return computedProbabilities[bit];
}

uint MixerModel::GetCumulativeProbability(short bit)
{
	assert(bit <= 1 && bit >= -1);
	if (bit < 0)
		return 0;
	else if (bit == 0)
		return computedProbabilities[0];
	else
		return computedProbabilities[0] + computedProbabilities[1];
}

void MixerModel::Update(int lastBit)
{
	assert(lastBit == 0 || lastBit == 1);
	contextInfo.lastBit = lastBit;
	contextInfo.lastByte = (contextInfo.lastByte << 1) + contextInfo.lastBit;
	if (contextInfo.lastByte >= 256)
	{
		contextInfo.buffer[contextInfo.buffer.currentPosition] = contextInfo.lastByte;
		contextInfo.buffer.currentPosition++;
		contextInfo.lastFourBytes = (contextInfo.lastFourBytes << 8) + contextInfo.lastByte - 256;
		contextInfo.lastByte = 1;
	}
	contextInfo.bitPosition = (contextInfo.bitPosition + 1) & 7;
	mixerNeuralNetwork.Update(lastBit);
	ComputeProbabilities();
}
