#include "basic_structs.h"
#include <iostream>

namespace util
{

bool addSymbol(Symbol* symbol, Node* tree, uint_fast32_t (*getCode)(Symbol*), int (*getLength)(Symbol*))
{
    uint_fast32_t symbolCode = getCode(symbol);
    int length = getLength(symbol);

    Node* node = tree;
    for(int i = length - 1; i >= 0; i--)
    {
        int bit = (symbolCode >> i) & 1;
        if(bit)
        {
            if(node->right == nullptr)
            {
                Node* newNode = new Node();
                node->right = newNode;
            }
            node = node->right;
        }
        else
        {
            if(node->left == nullptr)
            {
                Node* newNode = new Node();
                node->left = newNode;

            }
            node = node->left;
        }
    }
    if(node->symbol == nullptr)
    {
        node->symbol = symbol;
        return true;
    }
    else
    {
        return false;
    }
}

uint_fast32_t getSymbolCode(Symbol* symbol)
{
    return symbol->symbolCode;
}

int getSymbolLength(Symbol* symbol)
{
    return symbol->length;
}

uint_fast32_t getCompressedCode(Symbol* symbol)
{
    return symbol->compressedSymbolCode;
}

int getCompreessedLength(Symbol* symbol)
{
    return symbol->compressedLength;
}

}
