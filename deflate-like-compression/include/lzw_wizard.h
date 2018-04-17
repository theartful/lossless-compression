#ifndef LZW_WIZARD_H
#define LZW_WIZARD_H

#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include <list>

class LzwWizard
{

private:
    // number of bits
    static const int SLIDING_WINDOW_SIZE;
    static const int BUFFER_BITS_SIZE;
    static const int MINIMUM_MATCH_SIZE;

    // actual size in bytes
    static int slidingWindowSize;
    static int lookAheadBufferSize;

    static Buffer* slidingWindow;
    static Buffer* lookAheadBuffer;
    static ReadBuffer* readBuffer;
    static WriteBuffer* writeBuffer;

    inline static void findLongestMatch (int&, int&, Buffer*, Buffer*);

public:
    static void encodeFile(char* fromFile, char* toFile);
    static void decodeFile(char* fromFile, char* toFile);
};

#endif
