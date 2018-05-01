/* Liteweight PAQ-like compression / decompression. Under GNU GPL v2. */
/* Based on PAQ7, copyright (C) 2006 Matt Mahoney. */
#pragma once
#include "stdafx.h"
#include "ProbabilityModel.h"
#include "MixerNN.h"
#include "Buffer.h"
#include "ContextInfo.h"
#include "NaiveContextModel.h"
#include "ContextMaps.h"
#include "NonstationaryContextMaps.h"

class MixerModel : public ProbabilityModel
{
	MixerNN mixerNeuralNetwork;
	uint computedProbabilities[2];
	ContextInfo contextInfo;

	SmallContextMap scm1;
	SmallContextMap scm2;
	ContextMap cm1;
	ContextMap cm2;
	ContextMap cm3;
	ContextMap cm4;
	ContextMap cm5;
	ContextMap cm6;
	ContextMap cm7;
	RunContextMap rcm1;
	RunContextMap rcm2;
	RunContextMap rcm3;

	NaiveContextModel ncm0;
	NaiveContextModel ncm1;
	NaiveContextModel ncm2;
	NaiveContextModel ncm3;
	NaiveContextModel ncm4;
	NaiveContextModel ncm5;
	NaiveContextModel ncm6;
	NaiveContextModel ncm7;
	NaiveContextModel ncm8;

	/* Add the stretched outputs of the context map into the mixer neural network. */
	void MixContextMaps();
	/* Mix Context Models. */
	void MixContextModels();
	/* Actually compute the probabilities. Done once every input. */
	void ComputeProbabilities();
public:
	MixerModel();
	/* Gets a probability between 0 and 4096. */
	virtual uint GetProbability(short bit);
	/* Gets a cumulative probability between 0 and 4096. */
	virtual uint GetCumulativeProbability(short bit);
	/* Update with the last seen bit. */
	virtual void Update(int lastBit);
};
