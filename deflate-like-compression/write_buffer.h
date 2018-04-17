#ifndef WRITE_BUFFER_H
#define WRITE_BUFFER_H

#include <fstream>

class WriteBuffer{

char* buffer;
int bufferSize;

int writeByteIndex;
int writeBitIndex;
int writeBufferSize;

std::ofstream outputStream;

public:
	WriteBuffer(int bufferSize, char* fileName);
	void writeSymbol(int code, int codeLength);
	void finish();
	~WriteBuffer();
};

#endif
