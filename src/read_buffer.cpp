#include "read_buffer.h"

ReadBuffer::ReadBuffer(int bufferSize, char* fileName)
{
    this->bufferSize = bufferSize;
    this->readByteIndex = 0;
    this->readBitIndex = 7;
    this->readBufferSize = 0;
    this->eof = false;
    this->inputStream.open(fileName, std::ios::binary);
    this->buffer = new char[bufferSize + 200];
}

int ReadBuffer::readByte()
{
    if(readBitIndex != 7)
        return readSymbol(8);
    if (checkEndOfStream())
        return -1;
    readByteIndex++;
    return ((int) buffer[readByteIndex - 1]) & 0b11111111;
}

bool ReadBuffer::checkEndOfStream()
{
    if(readBufferSize <= readByteIndex && !eof)
    {
        inputStream.read(buffer, bufferSize);
        readBufferSize = inputStream.gcount();
        readByteIndex = 0;
        eof = inputStream.eof();
    }
    return readBufferSize <= readByteIndex;
}

uint_fast64_t ReadBuffer::readSymbol(int length)
{
    if(length == 0) return 0;
    if(checkEndOfStream())
        return -1;
    uint_fast64_t code = 0;
    for(int i = length - 1; i >= 0; i--)
    {
        if(readBitIndex < 0)
        {
            readBitIndex = 7;
            readByteIndex++;
            if(checkEndOfStream())
                return -1;
        }
        code |= ((buffer[readByteIndex] >> readBitIndex) & 1) << i;
        readBitIndex--;
    }
    return code;
}

util::Symbol* ReadBuffer::readSymbol(util::Node* searchTree)
{
    util::Node* node = searchTree;
    while(true)
    {
        int bit = readSymbol(1);
        if(bit == -1) return nullptr;
        if(bit)
        {
            if(node->right == nullptr)
            {
                std::cout << "ReadHuffmanSymbol: Error\n";
                return nullptr;
            }
            node = node->right;
        }
        else
        {
            if(node->left == nullptr)
            {
                std::cout << "ReadHuffmanSymbol: Error\n";
                return nullptr;
            }
            node = node->left;
        }
        if(node->symbol != nullptr)
            return node->symbol;
    }
}


bool ReadBuffer::isEmpty()
{
    return (readBufferSize <= readByteIndex) && eof;
}

ReadBuffer::~ReadBuffer()
{
    this->inputStream.close();
    delete[] buffer;
}
