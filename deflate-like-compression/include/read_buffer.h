#ifndef READ_BUFFER_H
#define READ_BUFFER_H

#include <fstream>

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
    int readSymbol(int length);
    bool isEmpty();
    void carriageReturn();
    void newLine();
    int getByteIndex();
    int getBitIndex();
    void setByteIndex(int);
    void setBitIndex(int);
    ~ReadBuffer();
};

#endif
