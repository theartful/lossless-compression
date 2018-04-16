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

int ReadBuffer::readByte() {
	if(readBufferSize <= readByteIndex && !eof) {
		inputStream.read(buffer, bufferSize);
		readBufferSize = inputStream.gcount();
		readByteIndex = 0;
		eof = inputStream.eof();
	}
	if (readBufferSize <= readByteIndex) return -1;

	readByteIndex ++;
	return ((int) buffer[readByteIndex - 1]) & 0b11111111;
}

ReadBuffer::~ReadBuffer() {
	this->inputStream.close();
	delete[] buffer;
}
