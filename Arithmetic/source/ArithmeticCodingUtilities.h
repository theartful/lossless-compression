#pragma once
#include "stdafx.h"
#include "Alphabet.h"
#include <algorithm>
using std::max;
#define MIN_PRECISION 45

void complementMaskedBits(ull& x, short bitNumber);
bool getMaskedBit(ull& x, short bitNumber);
void setBit(ull& x, short bitNumber);