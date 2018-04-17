#include "buffer.h"

Buffer::Buffer(int bufferSize, bool indexed) {
    this->bufferSize = bufferSize;
    this->indexed = indexed;
    this->absoluteSize = 0;

	buffer = new char[bufferSize * 2];

    if (indexed) {
        dict = new std::list<long>**[256];
        for (int i = 0; i < 256; i++) {
        	dict[i] = new std::list<long>*[256];
        	for(int j = 0; j < 256; j++) {
	            dict[i][j] = new std::list<long>();
        	}
    	}
    }

    lastElementIndex = -1;
    firstElementIndex = 0;
}


void Buffer::addByte(char b) {
	if (lastElementIndex >= 2 * bufferSize - 1) {
		for(int i = 0; i < lastElementIndex - firstElementIndex + 1; i++)
			buffer[i] = buffer[firstElementIndex + i];
		lastElementIndex = lastElementIndex - firstElementIndex;
		firstElementIndex = 0;
	}

	lastElementIndex++;
	buffer[lastElementIndex] = b;

	if (lastElementIndex - firstElementIndex + 1 > bufferSize)
		firstElementIndex++;

	if(indexed && size() > 1) {
		dict[buffer[lastElementIndex - 1] & 0b11111111][b & 0b11111111]->push_back(absoluteSize - 1);
		absoluteSize++;
	}
}

std::list<long>* Buffer::getPositionsList(int a, int b) {
	return dict[a][b];
}

char Buffer::getFirstByte() {
	return buffer[firstElementIndex];
}

char Buffer::getByte(int index) {
	return buffer[firstElementIndex + index];
}

bool Buffer::isEmpty() {
	return size() == 0;
}

char Buffer::removeFirst() {
	if(isEmpty()) return -1;
	firstElementIndex++;
	return buffer[firstElementIndex - 1];
}

int Buffer::size() {
	return lastElementIndex - firstElementIndex + 1;
}

Buffer::~Buffer() {
    if(indexed){
	for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++)
            delete dict[i][j];
            delete[] dict[i];
        }
        delete[] dict;
    }
	delete[] buffer;
}

long Buffer::getAbsoluteSize() {
	return absoluteSize;
}

