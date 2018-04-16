#include "lzw_wizard.h"
#include <iostream>
using namespace std;


LzwWizard::LzwWizard() {
	lookAheadBuffer = nullptr;
	slidingWindow = nullptr;
	readBuffer = nullptr;
	writeBuffer = nullptr;
}

void LzwWizard::encodeFile(char* fromFile, char* toFile) {

    int slidingWindowSize = 1 << SLIDING_WINDOW_SIZE;
	int lookAheadBufferSize = (1 << BUFFER_BITS_SIZE) + MINIMUM_MATCH_SIZE - 1;

    slidingWindow = new Buffer(slidingWindowSize, true);
    lookAheadBuffer = new Buffer(lookAheadBufferSize, false);
    readBuffer = new ReadBuffer(1024 * 64, fromFile);
    writeBuffer = new WriteBuffer(1024 * 64, toFile);

    // populate lookAheadBuffer
    for(int i = 0; i < lookAheadBufferSize; i ++) {
    	int c = readBuffer->readByte();
		if(c == -1)	break;
		lookAheadBuffer->addByte(c & 0b11111111);
	}

    char first;
    int b;
	int distance;
    int length;

    while (!lookAheadBuffer->isEmpty()) {

        findLongestMatch(distance, length);
        if (length >= MINIMUM_MATCH_SIZE) {
            // cout << "length " << length << " distance " << distance << endl;
            writeBuffer->writeSymbol(1, 1);
            writeBuffer->writeSymbol(distance, SLIDING_WINDOW_SIZE);
            writeBuffer->writeSymbol(length - MINIMUM_MATCH_SIZE, BUFFER_BITS_SIZE);

            // if (((processed / 1024) + 1) * 1024 - processed <= length) p = true;
            for (int i = 0; i < length; i++) {
                b = readBuffer->readByte();
                first = lookAheadBuffer->removeFirst();
                slidingWindow->addByte(first);
                if (b != -1) {
                    lookAheadBuffer->addByte((char) b);
                }
            }
        } else {
            // write byte as is
            writeBuffer->writeSymbol(0, 1);
            writeBuffer->writeSymbol(lookAheadBuffer->getFirstByte(), 8);
            first = lookAheadBuffer->removeFirst();
            slidingWindow->addByte(first);
            b = readBuffer->readByte();

            if (b != -1) {
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


// void decodeFile(char* fromFile, char* toFile);


inline void LzwWizard::findLongestMatch (int& maxDistance, int& maxLength) {
    maxDistance = 0;
    maxLength = 0;

    std::list<long>* positions = slidingWindow->getPositionsList(lookAheadBuffer->getByte(0) & 0b11111111,
            lookAheadBuffer->getByte(1) & 0b11111111);
    std::list<long>::iterator iterator = positions->begin();

    while (iterator != positions->end()) {
    	long pos = *iterator;
        int slidingIndex = (int) (pos - (slidingWindow->getAbsoluteSize() - slidingWindow->size()));
        if (slidingIndex < 0 || slidingIndex >= slidingWindow->size()) {
        	iterator = positions->erase(iterator);
            continue;
        }
        int distance = slidingIndex;
        int length = 2;
        int nextIndex = 3;
        char toBeMatched = lookAheadBuffer->getByte(2);
        slidingIndex += 2;

        for (; slidingIndex < slidingWindow->size(); slidingIndex++) {

            char slidingWindowByte = slidingWindow->getByte(slidingIndex);
            if (slidingWindowByte == toBeMatched) {
                length++;
                if (nextIndex >= lookAheadBuffer->size()) {
                    break;
                } else {
                    toBeMatched = lookAheadBuffer->getByte(nextIndex);
                    nextIndex++;
                }
            } else {
                break;
            }
        }
        if (length > maxLength) {
            maxLength = length;
            maxDistance = distance;
        }
        iterator++;
    }
    return;
}

LzwWizard::~LzwWizard() {
}

