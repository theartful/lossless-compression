#pragma once
#include "stdafx.h"

/* Hash 2-5 integers into a new integer. */
uint Hash(uint a, uint b, uint c = ~0, uint d = ~0, uint e = ~0);

/* Returns value rounded upwards to a multiple of 8. */
int RoundToEightMultiple(int value);

/* returns ceil(value/2) = (value + 1) / 2*/
int Div2Ceil(int value);

int Clamp(int value, int min, int max);

/* Returns the dot product of two arrays of short integers. */
/* Result is scaled down by 2^8.							*/
int DotProduct(short* x, short* y, int n);

/* ILog(x) = round(log2(x)*16), 1 <= x < 64K */
int ILog(ushort x);

/* LLog(x) = round(log2(x) * 16) */
int LLog(uint x);

/* Let f(x) = 1/1+exp(-x). Squash(x) = 2^12 * f(x*2^(-8)) */
/* Input is a integer (with precision -2^11 to 2^11) */
/* and the output is in the range (0, 2^12). */
int Squash(int x);

/* Takes x between 0 and 4096 and returns a stretched value between -2^11 and 2^11. */
int Stretch(int x);