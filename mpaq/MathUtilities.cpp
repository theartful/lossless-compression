#include "stdafx.h"
#include "MathUtilities.h"

uint Hash(uint a, uint b, uint c, uint d, uint e)
{
	uint h = a * 200002979u + b * 30005491u + c * 50004239u + d * 70004807u + e * 110002499u;
	return h^h >> 9 ^ a >> 2 ^ b >> 3 ^ c >> 4 ^ c >> 5 ^ e >> 6;
}

int DotProduct(short* x, short* y, int n)
{
	int sum = 0;
	for (int i = 0; i < n; i++)
	{
		sum += x[i] * y[i];
	}
	sum = sum >> 8;
	return sum;
}

/* Returns value rounded upwards to a multiple of 8. */
int RoundToEightMultiple(int value)
{
	return (value + 7) & (-8);
}

/* returns ceil(value/2) = (value + 1) / 2*/
int Div2Ceil(int value)
{
	return (value + 1) >> 1;
}

int Clamp(int value, int min, int max)
{
	if (value < min)
		return min;
	else if (value > max)
		return max;
	else
		return value;
}

int ILog(ushort x)
{
	assert(x >= 1 && x < 65536);
	static vector<uchar> lookUpTable;
	if (lookUpTable.size() == 0)
	{
		/* Initialize the function. */
		uint x = 14155776;
		lookUpTable.push_back(0);
		lookUpTable.push_back(0);
		/* Compute the lookup table by numerically integrating 1/x. */
		for (int i = 2; i < 65536; i++)
		{
			x += 774541002 / (i * 2 - 1); // The numerator is 2^29/ln(2)
			lookUpTable.push_back(x >> 24);
		}
	}
	return lookUpTable[x];
}

int LLog(uint x)
{
	if (x >= (1 << 24))
	{
		// 16 log(2^16 * short(x)) = 16*16 + 16*log (short(x)) 
		return 256 + ILog(x >> 16);
	}
	else if (x >= (1 << 16))
	{
		return 128 + ILog(x >> 8);
	}
	else
	{
		return ILog(x);
	}
}

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

int Stretch(int x)
{
	static short lookUpTable[4096] = { 0 };
	static bool initialized = false;
	assert(x < 4096 && x >= 0);
	if (!initialized)
	{
		int squashedValue = 0;
		for (int i = -2047; i < 2048; i++)
		{
			int value = Squash(i);
			for (int j = squashedValue; j <= value; j++)
			{
				lookUpTable[j] = i;
			}
			squashedValue = value + 1;
		}
		lookUpTable[4095] = 2047;
		initialized = true;
	}
	return lookUpTable[x];
}