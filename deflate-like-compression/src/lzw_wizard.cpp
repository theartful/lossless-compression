#include "lzw_wizard.h"
#include <iostream>
using namespace std;

const int LzwWizard::SLIDING_WINDOW_SIZE = 17;
const int LzwWizard::BUFFER_BITS_SIZE = 6;
const int LzwWizard::MINIMUM_MATCH_SIZE = 3;

// actual size in bytes
int LzwWizard::slidingWindowSize = 1 << SLIDING_WINDOW_SIZE;
int LzwWizard::lookAheadBufferSize = (1 << BUFFER_BITS_SIZE) + MINIMUM_MATCH_SIZE - 1;

void LzwWizard::encodeFile(char* fromFile, char* toFile)
{
    static Buffer* slidingWindow = new Buffer(slidingWindowSize, true);
    static Buffer* lookAheadBuffer = new Buffer(lookAheadBufferSize, false);
    static ReadBuffer* readBuffer = new ReadBuffer(1024 * 64, fromFile);
    static WriteBuffer* writeBuffer = new WriteBuffer(1024 * 64, toFile);

    // populate lookAheadBuffer
    for(int i = 0; i < lookAheadBufferSize; i ++)
    {
        int c = readBuffer->readByte();
        if(c == ~0)
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
            // cout << "length " << length << " distance " << distance << endl;
            writeBuffer->writeSymbol(1, 1);
            writeBuffer->writeSymbol(distance, SLIDING_WINDOW_SIZE);
            writeBuffer->writeSymbol(length - MINIMUM_MATCH_SIZE, BUFFER_BITS_SIZE);

            // if (((processed / 1024) + 1) * 1024 - processed <= length) p = true;
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

    delete slidingWindow;
    delete lookAheadBuffer;
    delete readBuffer;
    delete writeBuffer;
}


void LzwWizard::decodeFile(char* fromFile, char* toFile)
{
    const int UNSPECIFIED = -1;
    const int NORMAL_BYTE = 0;
    const int DISTANCE = 1;
    const int LENGTH = 2;

    static Buffer* slidingWindow = new Buffer(slidingWindowSize, true);
    static Buffer* lookAheadBuffer = new Buffer(lookAheadBufferSize, false);
    static ReadBuffer* readBuffer = new ReadBuffer(1024 * 64, fromFile);
    static WriteBuffer* writeBuffer = new WriteBuffer(1024 * 64, toFile);

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
                readMode = DISTANCE;
            length = 0;
            distance = 0;
        }
        else if (readMode == DISTANCE)
        {
            distance = readBuffer->readSymbol(SLIDING_WINDOW_SIZE);
            if(distance == -1)
                break;
            readMode = LENGTH;
        }
        else if (readMode == LENGTH)
        {
            length = readBuffer->readSymbol(BUFFER_BITS_SIZE);
            if(length == -1)
                break;
            length += MINIMUM_MATCH_SIZE;
            char bytes[length];
            for(int i = distance; i < distance + length; i++)
            {
                bytes[i - distance] = slidingWindow->getByte(i);
                writeBuffer->writeSymbol(bytes[i - distance], 8);
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

    delete slidingWindow;
    delete lookAheadBuffer;
    delete readBuffer;
    delete writeBuffer;
}

inline void LzwWizard::findLongestMatch (int& maxDistance, int& maxLength, Buffer* slidingWindow, Buffer* lookAheadBuffer)
{
    maxDistance = 0;
    maxLength = 0;

    std::list<long>* positions = slidingWindow->getPositionsList(lookAheadBuffer->getByte(0) & 0b11111111,
                                 lookAheadBuffer->getByte(1) & 0b11111111);
    std::list<long>::iterator iterator = positions->begin();

    while (iterator != positions->end())
    {
        long pos = *iterator;
        int slidingIndex = (int) (pos - (slidingWindow->getAbsoluteSize() - slidingWindow->size()));
        if (slidingIndex < 0 || slidingIndex >= slidingWindow->size())
        {
            iterator = positions->erase(iterator);
            continue;
        }
        int distance = slidingIndex;
        int length = 2;
        int nextIndex = 3;
        char toBeMatched = lookAheadBuffer->getByte(2);
        slidingIndex += 2;

        for (; slidingIndex < slidingWindow->size(); slidingIndex++)
        {

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
