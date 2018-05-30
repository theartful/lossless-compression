#include "arabic_utf_converter.h"

typedef util::Node Node;
typedef util::Symbol Symbol;

namespace ArabicUtfConverter
{

void encodeFile(UtfReader* readBuffer, WriteBuffer* writeBuffer, char* symbolsFile)
{
    Node* tree = new Node();
    UtfReader reader(symbolsFile);
    int i = 0;
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

    while(!readBuffer->isEmpty())
    {
        Symbol* symbol = readBuffer->readSymbol(tree);
        if(symbol == nullptr) break;
        writeBuffer->writeSymbol(symbol->compressedSymbolCode, 8);
    }

    // writeBuffer->writeSymbol(i, 8);

    delete tree;
    for(int j = 0; j < symbols.size(); j++)
    {
        delete symbols[j];
    }
}

void decodeFile(ReadBuffer* readBuffer, WriteBuffer* writeBuffer, char* symbolsFile)
{
    Node* tree = new Node();
    UtfReader reader(symbolsFile);
    int i = 0;
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
    Symbol* eofSymbol = new Symbol();
    eofSymbol->compressedLength = 8;
    eofSymbol->compressedSymbolCode = i;
    util::addSymbol(eofSymbol, tree, util::getCompressedCode, util::getCompreessedLength);

    while(!readBuffer->isEmpty())
    {
        Symbol* symbol = readBuffer->readSymbol(tree);
        if(symbol == nullptr || symbol == eofSymbol) break;
        writeBuffer->writeSymbol(symbol->symbolCode, symbol->length);
    }
    delete tree;

    for(int j = 0; j < symbols.size(); j++)
    {
        delete symbols[j];
    }
    delete eofSymbol;
}

void encodeFile(char* fromFile, char* toFile, char* symbolsFile)
{
    UtfReader* readBuffer = new UtfReader(fromFile);
    WriteBuffer* writeBuffer = new WriteBuffer(32 * 1024, toFile);
    encodeFile(readBuffer, writeBuffer, symbolsFile);
    writeBuffer->finish();
    delete readBuffer;
    delete writeBuffer;
}

void decodeFile(char* fromFile, char* toFile, char* symbolsFile)
{
    ReadBuffer* readBuffer = new ReadBuffer(32 * 1024, fromFile);
    WriteBuffer* writeBuffer = new WriteBuffer(32 * 1024, toFile);
    decodeFile(readBuffer, writeBuffer, symbolsFile);
    writeBuffer->finish();
    delete readBuffer;
    delete writeBuffer;
}

}
