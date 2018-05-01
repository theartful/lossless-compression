#include "stdafx.h"
#include "MixerNN.h"
#include "MathUtilities.h"

void MixerNN::Train(short* inputs, short* weights, int size, int error)
{
	//size = (size + 7) & (-8);
	assert(size == ((size + 7)&(-8))); // n must be a multiple of 8.
	for (int i = 0; i < size; i++)
	{
		// we scale down 16 bits to restore the true error, scale up by 2 to get the error to be +/1
		// and then calculate the gradient as inputs[i] * (error) [which minimizes the cross entropy]
		// if CE = - log p = - log (Squash(SUM wk Stretch(p[k])) is the cross entropy 
		// then dCE/dx = (p - 1) * Stretch(p[k]) = (probability error) * (inputs[k])
		int gradient = Div2Ceil((inputs[i] * error * 2) >> 16);
		weights[i] = Clamp(weights[i] + gradient, -MAX_WEIGHT, MAX_WEIGHT);
	}
}

MixerNN::MixerNN(int numInputs, int numHiddenUnits, int numExperts) : maxNumInputs(RoundToEightMultiple(numInputs)),
								maxNumHiddenUnits(numHiddenUnits), maxNumExperts(numExperts), inputs(maxNumInputs, 0),
								weights(maxNumInputs * maxNumHiddenUnits, 0), chosenHiddenUnits(maxNumExperts, 0), 
								expertOutputs(maxNumExperts, 0)
{
	currentNumExperts = 0;
	currentExpertOffset = 0;
	currentNumInputs = 0;
	/* Depending on the number of experts, initialize the output NN. */
	if (maxNumExperts > 1)
		outputNN = new MixerNN(maxNumExperts, 1, 1);
	else
		outputNN = nullptr;
}

void MixerNN::Update(int prevOutput)
{
	for (int i = 0; i < currentNumExperts; i++)
	{
		// Error is between -1/2 and 1/2, and is scaled 4 bits upwards beyond MAX_PRECISION.
		// The initial bit is always 0 though, because we're always talking +/- 1/2.
		int error = ((prevOutput << MAX_PRECISION) - expertOutputs[i]) * 7;
		assert(error >= -MAX_WEIGHT && error < MAX_WEIGHT);
		int expertWeightsPosition = maxNumInputs * chosenHiddenUnits[i]; // for the ith expert.
		Train(&inputs[0], &weights[expertWeightsPosition], currentNumInputs, error);
	}
	if (outputNN != nullptr)
		outputNN->Update(prevOutput);
	ClearForwardPropagationPath();
}


void MixerNN::AddInput(int input)
{
	assert(currentNumInputs < maxNumInputs);
	inputs[currentNumInputs] = input;
	currentNumInputs++;
}


void MixerNN::SetExpertHiddenUnit(int chosenUnit, int range)
{
	assert(range >= 0 && currentNumExperts < maxNumExperts && chosenUnit >= 0 && currentExpertOffset + chosenUnit < maxNumHiddenUnits);
	chosenHiddenUnits[currentNumExperts] = currentExpertOffset + chosenUnit;
	currentNumExperts++;
	currentExpertOffset += range;
}


int MixerNN::GetPrediction()
{
	while (currentNumInputs & 7)
	{
		inputs[currentNumInputs++] = 0; // Pad the inputs if they are not enough.
	}
	if (outputNN != nullptr)
	{
		for (int i = 0; i < currentNumExperts; i++)
		{
			int weightsPos = maxNumInputs * chosenHiddenUnits[i]; // We find which hidden unit will make the prediction for this expert.
			/* Weights are on the order of -2^15 to 2^15, inputs are in the 0 to 2^9(=256) range. */
			/* Hence the dot product is on the order of (currentNumInputs)(-2^24 to 2^24) * 2^-8. Here currentNumInputs is at most 512 */
			/* Hence the dot product is bounded by 2^16 to 2^16. Squash takes an input with precision -2^11 to 2^11, we therefore shift */
			/* left by 5 bits. This calculates the probability prediction from the ith expert. */
			expertOutputs[i] = Squash(DotProduct(&inputs[0], &weights[weightsPos], currentNumInputs) >> 5);
			//cout << inputs[2] << "\t";
			/* We add the expert's probability prediction  for the outputNN to mix them. */
			/* The inputs to the outputNN are stretched "probabilities" between -2^11 to 2^11. */
			outputNN->AddInput(Stretch(expertOutputs[i]));
		}
		outputNN->SetExpertHiddenUnit(0, 1); // We only have a single expert in the output NN anyway. With one output node.
		return outputNN->GetPrediction();
	}
	else
	{
		/* The dot product here is on the order (-2^11 to 2^11) * (-2^15 to 2^15) * (max n. of experts) * 2^-8 -> -2^19 to 2^19  */
		/* Squash takes probabilities precision -2^11 to 2^11, hence we shift down by 8 bits.  */
		expertOutputs[0] = Squash(DotProduct(&inputs[0], &weights[0], currentNumInputs) >> 8);
		return expertOutputs[0]; // Final probability in between 0 and 4096.
	}
}

void MixerNN::ClearForwardPropagationPath()
{
	currentNumInputs = 0;
	currentExpertOffset = 0;
	currentNumExperts = 0;
	if (outputNN != nullptr)
		outputNN->ClearForwardPropagationPath();
}
