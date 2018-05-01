#include "stdafx.h"
#include "Buffer.h"

Buffer::Buffer(int size) : buffer(size, 0)
{
}

uchar& Buffer::operator[](int index)
{
	index = index & (buffer.size() - 1); // For wrapping around.
	return buffer[index];
}

int Buffer::operator()(int index)
{
	assert(index > 0);
	return buffer[(currentPosition - index) & (buffer.size() - 1)];
}

int Buffer::size() const
{
	return buffer.size();
}