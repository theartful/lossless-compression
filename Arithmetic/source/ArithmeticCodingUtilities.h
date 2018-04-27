#pragma once
#include "stdafx.h"
#include "Alphabet.h"
#include <algorithm>
using std::max;

void complementMaskedBits(ull& x, short bitNumber);
bool getMaskedBit(ull& x, short bitNumber);
void setBit(ull& x, short bitNumber);