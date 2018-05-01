#pragma once
#include "stdafx.h"
#include "Buffer.h"

struct ContextInfo
{
	int lastBit;
	int bitPosition;
	int lastByte;
	uint lastFourBytes;
	Buffer buffer;
};