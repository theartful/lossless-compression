#ifndef ARABICUTFCONVERTER_H
#define ARABICUTFCONVERTER_H

#include "basic_structs.h"
#include "utf_reader.h"
#include "write_buffer.h"
#include <vector>

class ArabicUtfConverter
{

private:

public:
    static void encodeFile(char* fromFile, char* toFile, char* symbolsFile);
    static void decodeFile(char* fromFile, char* toFile, char* symbolsFile);

};

#endif // ARABICUTFCONVERTER_H
