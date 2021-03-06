#ifndef BASICSTRUCTS_H
#define BASICSTRUCTS_H

#include <iostream>

namespace util
{

const int LITERAL = 0;
const int DISTANCE = 1;
const int LENGTH = 2;

struct Symbol
{
    int length;
    uint_fast32_t symbolCode;

    int compressedLength;
    uint_fast32_t compressedSymbolCode;

    uint_fast32_t frequency;

    int type;

    Symbol()
    {
        length = 0;
        symbolCode = 0;
        compressedLength = 0;
        compressedSymbolCode = 0;
        frequency = 0;
        type = LITERAL;
    }
};

struct Node
{
    Symbol* symbol;
    Node* left;
    Node* right;
    int depth;
    uint_fast32_t frequency;

    Node()
    {
        symbol = nullptr;
        left = nullptr;
        right = nullptr;
        depth = 0;
        frequency = 0;
    }

    ~Node()
    {
        if(left != nullptr) delete left;
        if(right != nullptr) delete right;
    }

    void incrementDepth()
    {
        depth++;
        if(symbol != nullptr) symbol->compressedLength = depth;
        if(left != nullptr) left->incrementDepth();
        if(right != nullptr) right->incrementDepth();
    }
};

bool addSymbol(Symbol* symbol, Node* tree, uint_fast32_t (*getCode)(Symbol*), int (*getLength)(Symbol*));

Symbol* getSymbol(Node* tree, uint_fast32_t code, int length);

uint_fast32_t getSymbolCode(Symbol* symbol);

int getSymbolLength(Symbol* symbol);

uint_fast32_t getCompressedCode(Symbol* symbol);

int getCompreessedLength(Symbol* symbol);

}
#endif // BASICSTRUCTS_H
