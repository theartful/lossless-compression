#pragma once
#include "stdafx.h"

class Alphabet
{
protected:
	ull totalCount;
public:
	Alphabet() {}
	virtual ull GetComulativeCount(long i) = 0;
	virtual void Update(int lastSeenWord) = 0;
	virtual ull GetTotalCount() = 0;
};