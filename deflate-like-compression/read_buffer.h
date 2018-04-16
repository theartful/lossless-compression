#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <fstream>

class ReadBuffer{

char* buffer;
int bufferSize;

int readByteIndex;
int readBitIndex;
int readBufferSize;

bool eof;

std::ifstream inputStream;

public:
	ReadBuffer(int bufferSize, char* fileName);
	int readByte();
	~ReadBuffer();
};

#endif
