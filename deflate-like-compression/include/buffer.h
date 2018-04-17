#ifndef BUFFER_H
#define BUFFER_H

#include <list>

class Buffer
{

private:
    long absoluteSize;
    char* buffer;
    int bufferSize;
    bool indexed;
    std::list<long>*** dict;

    int lastElementIndex;
    int firstElementIndex;

public:

    Buffer(int bufferSize, bool indexed);
    void addByte(char b);
    std::list<long>* getPositionsList(int a, int b);
    char getFirstByte();
    char getByte(int index);
    bool isEmpty();
    char removeFirst();
    int size();
    int readByte();
    ~Buffer();
    long getAbsoluteSize();
};

#endif
