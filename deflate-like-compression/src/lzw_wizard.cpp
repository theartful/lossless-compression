#include "lzw_wizard.h"
#include <iostream>
using namespace std;

namespace LzwWizard
{

void encodeFile(char* fromFile, char* toFile, Buffer* slidingWindow)
{
    Buffer* lookAheadBuffer = new Buffer(lookAheadBufferSize, false);
    ReadBuffer* readBuffer = new ReadBuffer(1024, fromFile);
    WriteBuffer* writeBuffer = new WriteBuffer(1024, toFile);

    // populate lookAheadBuffer
    for(int i = 0; i < lookAheadBufferSize; i ++)
    {
        int c = readBuffer->readByte();
        if(c == -1)
            break;
        lookAheadBuffer->addByte(c);
    }

    char first;
    int b;
    int distance;
    int length;

    while (!lookAheadBuffer->isEmpty())
    {
        findLongestMatch(distance, length, slidingWindow, lookAheadBuffer);
        if (length >= MINIMUM_MATCH_SIZE)
        {
            writeBuffer->writeSymbol(1, 1);
            writeBuffer->writeSymbol(length - MINIMUM_MATCH_SIZE, BUFFER_BITS_SIZE);
            writeBuffer->writeSymbol(distance, SLIDING_WINDOW_SIZE);

            for (int i = 0; i < length; i++)
            {
                b = readBuffer->readByte();
                first = lookAheadBuffer->removeFirst();
                slidingWindow->addByte(first);
                if (b != -1)
                {
                    lookAheadBuffer->addByte((char) b);
                }
            }
        }
        else
        {
            writeBuffer->writeSymbol(0, 1);
            writeBuffer->writeSymbol(lookAheadBuffer->getFirstByte(), 8);
            first = lookAheadBuffer->removeFirst();
            slidingWindow->addByte(first);
            b = readBuffer->readByte();
            if (b != -1)
            {
                lookAheadBuffer->addByte((char) b);
            }
        }
    }
    writeBuffer->finish();

    delete lookAheadBuffer;
    delete readBuffer;
    delete writeBuffer;
}


void decodeFile(char* fromFile, char* toFile, Buffer* slidingWindow)
{
    const int UNSPECIFIED = -1;
    const int NORMAL_BYTE = 0;
    const int DISTANCE = 1;
    const int LENGTH = 2;

    Buffer* lookAheadBuffer = new Buffer(lookAheadBufferSize, false);
    ReadBuffer* readBuffer = new ReadBuffer(1024, fromFile);
    WriteBuffer* writeBuffer = new WriteBuffer(1024, toFile);

    int readMode = UNSPECIFIED;
    int length = 0;
    int distance = 0;

    while(!readBuffer->isEmpty())
    {
        if(readMode == UNSPECIFIED)
        {
            int bit = readBuffer->readSymbol(1);
            if(bit == -1)
                break;
            if(bit == 0)
                readMode = NORMAL_BYTE;
            else
                readMode = LENGTH;
            length = 0;
            distance = 0;
        }
        else if (readMode == LENGTH)
        {
            length = readBuffer->readSymbol(BUFFER_BITS_SIZE);
            if(length == -1)
                break;
            length += MINIMUM_MATCH_SIZE;
            readMode = DISTANCE;
        }
        else if (readMode == DISTANCE)
        {
            distance = readBuffer->readSymbol(SLIDING_WINDOW_SIZE);
            if(distance == -1)
                break;
            char bytes[length];
            for(int i = 0; i < length; i++)
            {
                bytes[i] = slidingWindow->getByte(distance + i % (slidingWindow->size() - distance));
                writeBuffer->writeSymbol(bytes[i], 8);
            }
            for(int i = 0; i < length; i++)
            {
                slidingWindow->addByte(bytes[i]);
            }
            readMode = UNSPECIFIED;
            length = 0;
            distance = 0;
        }
        else if (readMode == NORMAL_BYTE)
        {
            int byte = readBuffer->readByte();
            if(byte == -1)
                break;
            slidingWindow->addByte((char) byte);
            writeBuffer->writeSymbol(byte, 8);
            readMode = UNSPECIFIED;
        }
    }
    writeBuffer->finish();

    delete lookAheadBuffer;
    delete readBuffer;
    delete writeBuffer;
}

inline void findLongestMatch (int& maxDistance, int& maxLength, Buffer* slidingWindow, Buffer* lookAheadBuffer)
{
    maxDistance = 0;
    maxLength = 0;

    std::list<long>* positions = slidingWindow->getPositionsList(lookAheadBuffer->getByte(0) & 0b11111111,
                                 lookAheadBuffer->getByte(1) & 0b11111111, lookAheadBuffer->getByte(2) & 0b11111111);
    std::list<long>::iterator iterator = positions->begin();

    while (iterator != positions->end())
    {
        long pos = *iterator;
        uint_fast32_t slidingIndex = (int) (pos - (slidingWindow->getAbsoluteSize() - slidingWindow->size()));
        if (slidingIndex - 2 < 0 || slidingIndex - 2 >= slidingWindow->size())
        {
            iterator = positions->erase(iterator);
            continue;
        }
        int distance = slidingIndex - 2;
        int length = 3;
        int nextIndex = 4;
        char toBeMatched = lookAheadBuffer->getByte(3);

        int i = 3;
        for (;;i++)
        {
            slidingIndex = distance + i % (slidingWindow->size() - distance);
            char slidingWindowByte = slidingWindow->getByte(slidingIndex);
            if (slidingWindowByte == toBeMatched)
            {
                length++;
                if (nextIndex >= lookAheadBuffer->size())
                {
                    break;
                }
                else
                {
                    toBeMatched = lookAheadBuffer->getByte(nextIndex);
                    nextIndex++;
                }
            }
            else
            {
                break;
            }
        }
        if (length > maxLength)
        {
            maxLength = length;
            maxDistance = distance;
        }
        iterator++;
    }
    return;
}

}
