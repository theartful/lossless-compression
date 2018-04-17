#ifndef LZW_WIZARD_H
#define LZW_WIZARD_H

#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include <list>

class LzwWizard {

    // number of bits
	static const int SLIDING_WINDOW_SIZE = 17;
	static const int BUFFER_BITS_SIZE = 6;
	static const int MINIMUM_MATCH_SIZE = 3;

    // actual size in bytes
    int slidingWindowSize;
    int lookAheadBufferSize;

	Buffer* slidingWindow;
	Buffer* lookAheadBuffer;
	ReadBuffer* readBuffer;
	WriteBuffer* writeBuffer;

	void findLongestMatch (int&, int&);

public:

	LzwWizard();

	void encodeFile(char* fromFile, char* toFile);
	void decodeFile(char* fromFile, char* toFile);

    ~LzwWizard();

};

#endif
