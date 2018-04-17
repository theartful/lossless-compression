#ifndef BASICSTRUCTS_H
#define BASICSTRUCTS_H

#include <iostream>

namespace util
{

struct Symbol
{
    int length;
    uint_fast32_t symbolCode;

    int compressedLength;
    uint_fast32_t compressedSymbolCode;

    int frequency;

    Symbol()
    {
        length = 0;
        symbolCode = 0;
        compressedLength = 0;
        compressedSymbolCode = 0;
        frequency = 0;
    }
};

struct Node
{
    Symbol* symbol;
    Node* left;
    Node* right;
    int depth;
    int frequency;

    Node()
    {
        symbol = nullptr;
        left = nullptr;
        right = nullptr;
        depth = 0;
        frequency = 0;
    }
};

bool addSymbol(Symbol* symbol, Node* tree, uint_fast32_t (*getCode)(Symbol*), int (*getLength)(Symbol*));

uint_fast32_t getSymbolCode(Symbol* symbol);

int getSymbolLength(Symbol* symbol);

uint_fast32_t getCompressedCode(Symbol* symbol);

int getCompreessedLength(Symbol* symbol);

}
#endif // BASICSTRUCTS_H
