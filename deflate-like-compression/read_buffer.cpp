#include "read_buffer.h"

ReadBuffer::ReadBuffer(int bufferSize, char* fileName) {
	this->bufferSize = bufferSize;
	this->readByteIndex = 0;
	this->readBitIndex = 0;
	this->readBufferSize = 0;
    this->eof = false;

	this->inputStream.open(fileName, std::ios::binary);

	buffer = new char[bufferSize + 200];
}

int ReadBuffer::readByte()
{
    if(readBitIndex != 0) return readSymbol(8);
	if (checkEndOfStream()) return -1;
	readByteIndex++;
	return ((int) buffer[readByteIndex - 1]) & 0b11111111;
}

bool ReadBuffer::checkEndOfStream()
{
    if(readBufferSize <= readByteIndex && !eof) {
		inputStream.read(buffer, bufferSize);
		readBufferSize = inputStream.gcount();
		readByteIndex = 0;
		eof = inputStream.eof();
	}
	return readBufferSize <= readByteIndex;
}

int ReadBuffer::readSymbol(int length)
{
    if(checkEndOfStream()) return -1;
    int code = 0;
    for(int i = 0; i < length; i++)
    {
        if(readBitIndex > 7)
        {
            readBitIndex = 0;
            readByteIndex++;
            if(checkEndOfStream()) return -1;
        }
        code |= ((buffer[readByteIndex] >> readBitIndex) & 1) << i;
        readBitIndex++;
    }
    return code;
}

bool ReadBuffer::isEmpty() {
    return (readBufferSize <= readByteIndex) && eof;
}

ReadBuffer::~ReadBuffer() {
	this->inputStream.close();
	delete[] buffer;
}
