#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <fstream>
#include "basic_structs.h"

class ReadBuffer
{

private:
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
    uint_fast64_t readSymbol(int length);
    util::Symbol* readSymbol(util::Node* searchTree);
    bool isEmpty();
    ~ReadBuffer();
};

#endif
