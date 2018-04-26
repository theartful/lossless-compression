#include "arabic_utf_converter.h"

typedef util::Node Node;
typedef util::Symbol Symbol;

namespace ArabicUtfConverter
{

void encodeFile(char* fromFile, char* toFile, char* symbolsFile)
{
    Node* tree = new Node();
    UtfReader reader(symbolsFile);
    uint_fast32_t i = 0;
    std::vector<Symbol*> symbols;
    while(!reader.isEmpty())
    {
        int numberOfBytes;
        uint_fast32_t code = reader.readUtfCharacter(numberOfBytes);
        Symbol* symbol = new Symbol();
        symbol->length = numberOfBytes * 8;
        symbol->symbolCode = code;
        symbol->compressedLength = 8;
        symbol->compressedSymbolCode = i;
        util::addSymbol(symbol, tree, util::getSymbolCode, util::getSymbolLength);
        symbols.push_back(symbol);
        i++;
    }

    UtfReader readBuffer(fromFile);
    WriteBuffer writeBuffer(32 * 1024, toFile);

    while(!readBuffer.isEmpty())
    {
        Symbol* symbol = readBuffer.readSymbol(tree);
        if(symbol == nullptr) break;
        writeBuffer.writeSymbol(symbol->compressedSymbolCode, 8);
    }
    writeBuffer.finish();
    delete tree;
    for(int i = 0; i < symbols.size(); i++)
    {
        delete symbols[i];
    }
}

void decodeFile(char* fromFile, char* toFile, char* symbolsFile)
{
    Node* tree = new Node();
    UtfReader reader(symbolsFile);
    uint_fast32_t i = 0;
    std::vector<Symbol*> symbols;
    while(!reader.isEmpty())
    {
        int numberOfBytes;
        uint_fast32_t code = reader.readUtfCharacter(numberOfBytes);
        Symbol* symbol = new Symbol();
        symbol->length = numberOfBytes * 8;
        symbol->symbolCode = code;
        symbol->compressedLength = 8;
        symbol->compressedSymbolCode = i;
        util::addSymbol(symbol, tree, util::getCompressedCode, util::getCompreessedLength);
        symbols.push_back(symbol);
        i++;
    }

    ReadBuffer readBuffer(32 * 1024, fromFile);
    WriteBuffer writeBuffer(32 * 1024, toFile);

    while(!readBuffer.isEmpty())
    {
        Symbol* symbol = readBuffer.readSymbol(tree);
        if(symbol == nullptr) break;
        writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);
    }
    writeBuffer.finish();
    delete tree;
    for(int i = 0; i < symbols.size(); i++)
    {
        delete symbols[i];
    }
}

}
