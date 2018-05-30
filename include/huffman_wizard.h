#ifndef HUFFMAN_WIZARD_H
#define HUFFMAN_WIZARD_H

#include "buffer.h"
#include "write_buffer.h"
#include "read_buffer.h"
#include "basic_structs.h"
#include <queue>
#include "lzw_wizard.h"

namespace HuffmanWizard
{

void encodeCharacter(util::Symbol** symbols, int n, WriteBuffer& writeBuffer, util::Symbol* symbol);
util::Symbol* decodeCharacter(util::Symbol** symbols, int n, ReadBuffer& readBuffer);

void encodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** literals, int numberOfSymbols);
void decodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** litearls, int numberOfSymbols);

void assignCodes(util::Symbol** symbols, int n);
void setCanonicalCodes(util::Symbol** symbols, int n);
}

#endif // HUFFMAN_WIZARD_H
