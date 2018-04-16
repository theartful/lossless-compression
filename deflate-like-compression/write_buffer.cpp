#include "write_buffer.h"
#include <iostream>


WriteBuffer::WriteBuffer(int bufferSize, char* fileName) {
	this->bufferSize = bufferSize;
	this->writeByteIndex = 0;
	this->writeBitIndex = 0;
	int actualBufferSize = bufferSize + 8;
	this->buffer = new char[actualBufferSize];
	outputStream.open(fileName, std::ios::binary);
}

void WriteBuffer::writeSymbol(int code, int codeLength) {
	for (int i = 0; i < codeLength; i++) {
        if ((code & (1 << i)) != 0) {
            buffer[writeByteIndex] |= (1 << writeBitIndex);
        } else {
            buffer[writeByteIndex] &= ~(1 << writeBitIndex);
        }
        writeBitIndex++;
        if (writeBitIndex > 7) {
            writeBitIndex = 0;
            writeByteIndex++;
        }
    }

    if (writeByteIndex > bufferSize) {
        outputStream.write(buffer, bufferSize);
        for (int i = bufferSize; i <= writeByteIndex; i++) {
            buffer[i - bufferSize] = buffer[i];
        }
        writeByteIndex = writeByteIndex - bufferSize;
	}
}

void WriteBuffer::finish() {
	if (writeByteIndex != 0)
		outputStream.write(buffer, writeByteIndex);
}

WriteBuffer::~WriteBuffer() {
	this->outputStream.close();
	delete[] buffer;
}
