#include "arabic_utf_converter.h"

typedef util::Node Node;
typedef util::Symbol Symbol;

void ArabicUtfConverter::encodeFile(char* fromFile, char* toFile, char* symbolsFile)
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
        int length = 0;
        uint_fast32_t code = readBuffer.readUtfCharacter(length);
        length *= 8;
        Node* node = tree;
        for(int i = length - 1; i >= 0; i--)
        {
            int bit = (code >> i) & 1;
            if(bit)
                node = node->right;
            else
                node = node->left;
        }
        writeBuffer.writeSymbol(node->symbol->compressedSymbolCode, 8);
    }
    writeBuffer.finish();
    delete tree;
    for(int i = 0; i < symbols.size(); i++)
    {
        delete symbols[i];
    }
}

void ArabicUtfConverter::decodeFile(char* fromFile, char* toFile, char* symbolsFile)
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
        int code = readBuffer.readByte();
        Node* node = tree;
        for(int i = 7; i >= 0; i--)
        {
            int bit = (code >> i) & 1;
            if(bit)
                node = node->right;
            else
                node = node->left;
        }
        int numberOfBytes = node->symbol->length/8;
        for(int i = 0; i < numberOfBytes; i++)
        {
            int byte = node->symbol->symbolCode >> ((numberOfBytes - 1 - i) * 8);
            writeBuffer.writeSymbol(byte, 8);
        }
    }
    writeBuffer.finish();
    delete tree;
    for(int i = 0; i < symbols.size(); i++)
    {
        delete symbols[i];
    }
}
