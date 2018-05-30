#ifndef UTFREADER_H
#define UTFREADER_H

#include "read_buffer.h"

class UtfReader : public ReadBuffer
{
private:

public:
    UtfReader(char* fileName);
    uint_fast32_t readUtfCharacter(int&);
    ~UtfReader();
};

#endif // UTFREADER_H
