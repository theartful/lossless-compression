#pragma once
#include "stdafx.h"

/* This HashMap maps a 32 bit hash to an array of [size] elements each having [numBytes] bytes. */
/* The elements are each structured as follows: the first byte is a checksum, the second byte is a priority */
/* and the remaining numBytes-2 bytes are values. The second byte prioritizes the other values,
/* The values are arranged by priority. The first */
/* size must be a power of two.	*/
template<int numBytes> class HashMap
{
	const int searchLimit = 4;
	vector<uchar> elements;
	const uint lastElement; // = size - 1.
public:
	explicit HashMap(int size) : lastElement(size - 1), elements(numBytes * size, 0)
	{
		assert(numBytes >= 2 && size > 0 && (size&(size - 1)) == 0);
	}

	/* hashMap[i] returns a pointer to the element mapped to 'index'   */
	uchar* operator[] (uint key)
	{
		int chksum = ((key >> 24) ^ (key >> 12) ^ key) & 255;
		key &= lastElement;
		int replacementKey = key;
		int replacementPriority = 1024;
		uchar* replacementElement;
		for (int j = 0; j < searchLimit; j++)
		{
			replacementElement = &elements[(key ^ j) * numBytes];
			if (replacementElement[0] == chksum)
			{
				return replacementElement; // Match exists.
			}
			else if (replacementElement[1] == 0)
			{
				replacementElement[0] = chksum;
				return replacementElement; // Place is empty.
			}
			else if (replacementElement[1] < replacementPriority)
			{
				replacementPriority = replacementElement[1];
				replacementKey = key^j; // The best replacement so far.
			}
		}
		/* We searched and no element corresponding to the key was found, so we return the lowest priority */
		/* element instead. */
		replacementElement = &elements[replacementKey * numBytes];
		for (int i = 0; i < numBytes; i++)
		{
			replacementElement[i] = 0;
		}
		replacementElement[0] = chksum;
		return replacementElement;
	}
};