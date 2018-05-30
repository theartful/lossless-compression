#ifndef BUFFER_H
#define BUFFER_H

#include <list>
#include <iostream>

class Buffer
{

private:
    uint_fast32_t absoluteSize;
    char* buffer;
    int bufferSize;
    bool indexed;
    std::list<long>**** dict;

    int lastElementIndex;
    int firstElementIndex;

public:

    Buffer(int bufferSize, bool indexed);
    void addByte(int b);
    std::list<long>* getPositionsList(int a, int b, int c);
    char getFirstByte();
    int getByte(int index);
    bool isEmpty();
    char removeFirst();
    unsigned long size();
    int readByte();
    ~Buffer();
    uint_fast32_t getAbsoluteSize();
};

#endif
