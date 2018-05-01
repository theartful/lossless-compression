#pragma once
#include "stdafx.h"
#define MAX_CODING_PRECISION 28

void GetLowAndHigh(int k, int p1, int& low, int& high);
void complementMaskedBits(ull& x, short bitNumber);
bool getMaskedBit(ull& x, short bitNumber);
void setBit(ull& x, short bitNumber);