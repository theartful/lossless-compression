#ifndef ARABICUTFCONVERTER_H
#define ARABICUTFCONVERTER_H

#include "basic_structs.h"
#include "utf_reader.h"
#include "write_buffer.h"
#include <vector>

namespace ArabicUtfConverter
{

void encodeFile(char* fromFile, char* toFile, char* symbolsFile);
void decodeFile(char* fromFile, char* toFile, char* symbolsFile);

};

#endif // ARABICUTFCONVERTER_H
