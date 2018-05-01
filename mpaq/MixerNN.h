#pragma once
#include "stdafx.h"
#define MAX_WEIGHT 32768 // Used for regularizing the weights.

/* The neural network used for mixing outputs. */
/* This is a single output neural network using a mixture of experts architecture if numExperts > 1.	   */
class MixerNN
{
private:
	const int maxNumInputs;
	const int maxNumHiddenUnits;
	const int maxNumExperts;

	vector<short> inputs;
	vector<short> weights;
	vector<int>	chosenHiddenUnits; // 1 for each expert.

	int currentNumExperts;
	int currentExpertOffset;
	int currentNumInputs;

	vector<int> expertOutputs;
	MixerNN* outputNN;
	/* Trains weights given inputs using error: weights[i] = weights[i] + error. i = 0 to size-1.*/
	void Train(short* inputs, short* weights, int size, int error);
	/* Clear forward propagation path. */
	void ClearForwardPropagationPath();
public:
	MixerNN(int numInputs, int numHiddenUnits, int numExperts = 1);
	void Update(int prevOutput); // Update so as to minimize the cross-entropy.
	/* Inputs are in the 0 to 2^9(=256) range. */
	void AddInput(int input);
	/* chosenUnit: hidden unit for the currently selected expert. */
	/* range: the total number of hidden units owned by that expert. */
	void SetExpertHiddenUnit(int chosenUnit, int range);
	/* Evaluate the neural network at this point. Get a prediction between 0 and 4096. */
	int GetPrediction();
};