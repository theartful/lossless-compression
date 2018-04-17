#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <fstream>

class ReadBuffer{

char* buffer;
int bufferSize;

int readByteIndex;
int readBitIndex;
int readBufferSize;

std::ifstream inputStream;

bool eof;
bool checkEndOfStream();

public:
	ReadBuffer(int bufferSize, char* fileName);
	int readByte();
	int readSymbol(int length);
	bool isEmpty();
	~ReadBuffer();
};

#endif
