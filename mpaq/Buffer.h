#pragma once
#include "stdafx.h"

class Buffer
{
	vector<uchar> buffer;
public:
	int currentPosition = 0;
	Buffer(int size = MEMORY_USAGE / 8);
	uchar& operator[](int index);
	int operator()(int index); // Returns the index-th byte back from currentPosition.
	int size() const;
};