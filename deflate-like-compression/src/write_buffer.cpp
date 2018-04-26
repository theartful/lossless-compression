#include "write_buffer.h"
#include <iostream>

WriteBuffer::WriteBuffer(int bufferSize, char* fileName)
{
    this->bufferSize = bufferSize;
    this->writeByteIndex = 0;
    this->writeBitIndex = 7;
    int actualBufferSize = bufferSize + 8;
    this->buffer = new char[actualBufferSize];
    outputStream.open(fileName, std::ios::binary | std::ios::out);
}

void WriteBuffer::writeSymbol(int code, int codeLength)
{
    for (int i = codeLength - 1; i >= 0; i--)
    {
        if ((code & (1 << i)) != 0)
        {
            buffer[writeByteIndex] |= (1 << writeBitIndex);
        }
        else
        {
            buffer[writeByteIndex] &= ~(1 << writeBitIndex);
        }
        writeBitIndex--;
        if (writeBitIndex < 0)
        {
            writeBitIndex = 7;
            writeByteIndex++;
            buffer[writeByteIndex] = 0;
        }
    }

    if (writeByteIndex > bufferSize)
    {
        outputStream.write(buffer, bufferSize);
        for (int i = bufferSize; i <= writeByteIndex; i++)
        {
            buffer[i - bufferSize] = buffer[i];
        }
        writeByteIndex = writeByteIndex - bufferSize;
    }
}

void WriteBuffer::finish()
{
    if(writeBitIndex == 7)
        outputStream.write(buffer, writeByteIndex);
    else
        outputStream.write(buffer, writeByteIndex + 1);
}

WriteBuffer::~WriteBuffer()
{
    this->outputStream.close();
    delete[] buffer;
}
