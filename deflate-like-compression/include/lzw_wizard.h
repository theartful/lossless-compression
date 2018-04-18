#ifndef LZW_WIZARD_H
#define LZW_WIZARD_H

#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include <list>

namespace LzwWizard
{

// number of bits
static const int SLIDING_WINDOW_SIZE = 18;
static const int BUFFER_BITS_SIZE = 5;
static const int MINIMUM_MATCH_SIZE = 3;

// actual size in bytes
static const int slidingWindowSize = 1 << SLIDING_WINDOW_SIZE;
static const int lookAheadBufferSize = 1 << BUFFER_BITS_SIZE;

inline static void findLongestMatch (int&, int&, Buffer*, Buffer*);

void encodeFile(char* fromFile, char* toFile);
void decodeFile(char* fromFile, char* toFile);

};

#endif
