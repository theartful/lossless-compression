#include "utf_reader.h"

UtfReader::UtfReader(char* fileName) : ReadBuffer(1024 * 32,fileName)
{
}

uint_fast32_t UtfReader::readUtfCharacter(int& numberOfBytes)
{
    uint_fast32_t code = 0;
    unsigned char byte = readByte();
    numberOfBytes = 0;
    for(int i = 7; i >= 0; i--)
    {
        if((byte & (1 << i)) != 0)
            numberOfBytes++;
        else
            break;
    }
    if(numberOfBytes == 0)
    {
        code |= ((uint_fast32_t) byte);
        numberOfBytes = 1;
        return code;
    }
    if(numberOfBytes == 1)
    {
        numberOfBytes = -1;
        return code;
    }
    code |= ((uint_fast32_t) byte) << ((numberOfBytes - 1)* 8);
    for(int j = numberOfBytes - 2; j >= 0; j--)
    {
        byte = readByte();
        code |= ((uint_fast32_t) byte) << (j * 8);
    }
    return code;
}

UtfReader::~UtfReader()
{

}
