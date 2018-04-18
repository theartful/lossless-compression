#include "huffman_wizard.h"
#include <iostream>

namespace HuffmanWizard
{

typedef util::Node Node;
typedef util::Symbol Symbol;

int DISTANCES_SIZE = 6;
int LENGTHES_SIZE = 5;
unsigned int blockSize = 1024 * 32;

Symbol* readHuffmanSymbol(ReadBuffer& readBuffer, Node* tree)
{
    Node* node = tree;
    while(true)
    {
        int bit = readBuffer.readSymbol(1);
        if(bit)
        {
            if(node->right == nullptr) return nullptr;
            node = node->right;
        }
        else
        {
            if(node->left == nullptr) return nullptr;
            node = node->left;
        }
        if(node->symbol != nullptr) return node->symbol;
    }
}

void decodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** literals, int numberOfSymbols)
{
    ReadBuffer readBuffer(1024, fromFile);
    WriteBuffer writeBuffer(1024, toFile);

    Symbol** distances = new Symbol*[1 << DISTANCES_SIZE];
    Symbol** lengthes = new Symbol*[1 << LENGTHES_SIZE];

    for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
    {
        distances[i] = new Symbol();
        distances[i]->symbolCode = i;
        distances[i]->length = DISTANCES_SIZE;
    }

    for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
    {
        lengthes[i] = new Symbol();
        lengthes[i]->symbolCode = i;
        lengthes[i]->length = LENGTHES_SIZE;
    }

    while(!readBuffer.isEmpty())
    {
        for(int i = 0; i < numberOfSymbols; i++)
        {
            int code = readBuffer.readSymbol(5);
            literals[i]->compressedLength = code;
            literals[i]->compressedSymbolCode = 0;
        }
        for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
        {
            int code = readBuffer.readSymbol(5);
            distances[i]->compressedLength = code;
            distances[i]->compressedSymbolCode = 0;
        }
        for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
        {
            int code = readBuffer.readSymbol(5);
            lengthes[i]->compressedLength = code;
            lengthes[i]->compressedSymbolCode = 0;
        }

        setCanonicalCodes(literals, numberOfSymbols);
        setCanonicalCodes(distances, 1 << DISTANCES_SIZE);
        setCanonicalCodes(lengthes, 1 << LENGTHES_SIZE);

        Node* literalsSearchTree = new Node;
        Node* distancesSearchTree = new Node;
        Node* lengthesSearchTree = new Node;

        for(int i = 0; i < numberOfSymbols; i++)
        {
            util::addSymbol(literals[i], literalsSearchTree, util::getCompressedCode, util::getCompreessedLength);
        }
        for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
        {
            util::addSymbol(distances[i], distancesSearchTree, util::getCompressedCode, util::getCompreessedLength);
        }
        for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
        {
            util::addSymbol(lengthes[i], lengthesSearchTree, util::getCompressedCode, util::getCompreessedLength);
        }

        while(!readBuffer.isEmpty())
        {
            int bit = readBuffer.readSymbol(1);
            if(bit)
            {
                // read rest of distance
                int tmp = readBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp == -1)
                    break;
                writeBuffer.writeSymbol(tmp, LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);

                // read distance
                Symbol* symbol = readHuffmanSymbol(readBuffer, distancesSearchTree);
                if(symbol == nullptr)
                    break;
                writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);

                // read rest of length
                tmp = readBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp == -1)
                    break;
                writeBuffer.writeSymbol(tmp, LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);

                // read length
                symbol = readHuffmanSymbol(readBuffer, lengthesSearchTree);
                if(symbol == nullptr)
                    break;
                writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);
            }
            else
            {
                Symbol* symbol = readHuffmanSymbol(readBuffer, literalsSearchTree);
                if(symbol == nullptr || symbol == literals[numberOfSymbols - 1])
                    break;
                writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);
            }
        }
        delete literalsSearchTree;
        delete distancesSearchTree;
        delete lengthesSearchTree;
    }

    for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
    {
        delete distances[i];
    }
    delete[] distances;
    for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
    {
        delete lengthes[i];
    }
    delete[] lengthes;
}

void flipCodes(util::Symbol** symbols, int n)
{
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < symbols[j]->compressedLength / 2; i++) {
            int a = (symbols[j]->compressedSymbolCode & (1 << i)) != 0 ? 1 : 0;
            int b = (symbols[j]->compressedSymbolCode & (1 << (symbols[j]->compressedLength - i - 1))) != 0 ? 1 : 0;

            if (a == 1) symbols[j]->compressedSymbolCode |= (1 << (symbols[j]->compressedLength - i - 1));
            else symbols[j]->compressedSymbolCode &= ~(1 << (symbols[j]->compressedLength - i - 1));
            if (b == 1) symbols[j]->compressedSymbolCode |= (1 << i);
            else symbols[j]->compressedSymbolCode &= ~(1 << i);
        }
    }
}

void encodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** literals, int numberOfSymbols)
{
    ReadBuffer analyzerBuffer(1024 * 32, fromFile);
    ReadBuffer encoderBuffer(1024 * 32, fromFile);
    WriteBuffer writeBuffer(1024 * 32, toFile);

    Node* literalsSearchTree = new Node;
    Node* distancesSearchTree = new Node;
    Node* lengthesSearchTree = new Node;

    Symbol** distances = new Symbol*[1 << DISTANCES_SIZE];
    Symbol** lengthes = new Symbol*[1 << LENGTHES_SIZE];

    for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
    {
        distances[i] = new Symbol();
        distances[i]->symbolCode = i;
        distances[i]->length = DISTANCES_SIZE;
        util::addSymbol(distances[i], distancesSearchTree, util::getSymbolCode, util::getSymbolLength);
    }

    for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
    {
        lengthes[i] = new Symbol();
        lengthes[i]->symbolCode = i;
        lengthes[i]->length = LENGTHES_SIZE;
        util::addSymbol(lengthes[i], lengthesSearchTree, util::getSymbolCode, util::getSymbolLength);
    }

    for(int i = 0; i < numberOfSymbols - 1; i++)
    {
        util::addSymbol(literals[i], literalsSearchTree, util::getSymbolCode, util::getSymbolLength);
    }
    literals[numberOfSymbols - 1]->frequency = 1;

    while(true)
    {
        unsigned int i = 0;
        while(!analyzerBuffer.isEmpty() && i < blockSize)
        {
            int bit = analyzerBuffer.readSymbol(1);
            if(bit)
            {
                // read rest of distance
                int tmp =analyzerBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp == -1)
                    break;

                // read distance
                int distance = analyzerBuffer.readSymbol(DISTANCES_SIZE);
                if(distance == -1)
                    break;
                Symbol* symbol = util::getSymbol(distancesSearchTree, distance, DISTANCES_SIZE);
                symbol->frequency++;

                // read rest of length
                tmp = analyzerBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp == -1)
                    break;

                // read length
                int length = analyzerBuffer.readSymbol(LENGTHES_SIZE);
                if(length == -1)
                    break;
                symbol = util::getSymbol(lengthesSearchTree, length, LENGTHES_SIZE);
                symbol->frequency++;
            }
            else
            {
                int byte = analyzerBuffer.readSymbol(8);
                if(byte == -1)
                    break;
                Symbol* symbol = util::getSymbol(literalsSearchTree, byte, 8);
                symbol->frequency++;
            }
            i++;
        }
        assignCodes(literals, numberOfSymbols);
        assignCodes(distances, 1 << DISTANCES_SIZE);
        assignCodes(lengthes, 1 << LENGTHES_SIZE);
        flipCodes(literals, numberOfSymbols);
        flipCodes(distances, 1 << DISTANCES_SIZE);
        flipCodes(lengthes, 1 << LENGTHES_SIZE);

        for(int i = 0; i < numberOfSymbols; i++)
        {
            writeBuffer.writeSymbol(literals[i]->compressedLength, 5);
        }
        for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
        {
            writeBuffer.writeSymbol(distances[i]->compressedLength, 5);
        }
        for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
        {
            writeBuffer.writeSymbol(lengthes[i]->compressedLength, 5);
        }
        i = 0;
        while(!encoderBuffer.isEmpty() && i < blockSize)
        {
            int bit = encoderBuffer.readSymbol(1);
            if(bit)
            {
                // read rest of distance
                int tmp = encoderBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp == -1)
                    break;
                writeBuffer.writeSymbol(tmp, LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);

                // read distance
                int distance = encoderBuffer.readSymbol(DISTANCES_SIZE);
                if(distance == -1)
                    break;
                Symbol* symbol = util::getSymbol(distancesSearchTree, distance, DISTANCES_SIZE);
                writeBuffer.writeSymbol(symbol->compressedSymbolCode, symbol->compressedLength);

                // read rest of length
                tmp = encoderBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp == -1)
                    break;
                writeBuffer.writeSymbol(tmp, LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);

                // read length
                int length = encoderBuffer.readSymbol(LENGTHES_SIZE);
                if(length == -1)
                    break;
                symbol = util::getSymbol(lengthesSearchTree, length, LENGTHES_SIZE);
                writeBuffer.writeSymbol(symbol->compressedSymbolCode, symbol->compressedLength);
            }
            else
            {
                uint_fast32_t byte = encoderBuffer.readSymbol(8);
                if(byte == -1)
                    break;
                Symbol* symbol = util::getSymbol(literalsSearchTree, byte, 8);
                writeBuffer.writeSymbol(symbol->compressedSymbolCode, symbol->compressedLength);
            }
            i++;
        }
        writeBuffer.writeSymbol(literals[numberOfSymbols - 1]->compressedSymbolCode, literals[numberOfSymbols - 1]->compressedLength);
        if(encoderBuffer.isEmpty()) break;
    }
    writeBuffer.finish();
    delete literalsSearchTree;
    delete distancesSearchTree;
    delete lengthesSearchTree;
    for(int i = 0; i < 1 << DISTANCES_SIZE; i++)
    {
        delete distances[i];
    }
    delete[] distances;
    for(int i = 0; i < 1 << LENGTHES_SIZE; i++)
    {
        delete lengthes[i];
    }
    delete[] lengthes;
}

void assignCodes(util::Symbol** symbols, int n)
{
    auto cmp = [](Node* a, Node* b)
    {
        return a->frequency > b->frequency;
    };
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> priorityQueue(cmp);
    for(int i = 0; i < n; i++)
    {
        Node* node = new Node();
        node->frequency = symbols[i]->frequency;
        node->symbol = symbols[i];
        priorityQueue.push(node);
    }
    Node* node;
    for(int i = 0; i < n - 1; i++)
    {
        node = new Node();
        node->left = priorityQueue.top();
        priorityQueue.pop();
        node->right = priorityQueue.top();
        priorityQueue.pop();
        node->left->incrementDepth();
        node->right->incrementDepth();
        node->frequency = node->left->frequency + node->right->frequency;
        priorityQueue.push(node);
    }
    priorityQueue.pop();
    delete node;
    setCanonicalCodes(symbols, n);
}

void setCanonicalCodes(util::Symbol** symbols, int n)
{
    int maxCodeLength = 0;
    for(int i = 0; i < n; i++)
    {
        if(symbols[i]->compressedLength > maxCodeLength)
            maxCodeLength = symbols[i]->compressedLength;
    }
    int l_count[maxCodeLength + 1];
    for(int i = 0; i < maxCodeLength + 1; i++)
        l_count[i] = 0;

    for(int i = 0; i < n; i++)
    {
        if(symbols[i]->compressedLength == 0)
            continue;
        l_count[symbols[i]->compressedLength]++;
    }
    uint_fast32_t next_code[maxCodeLength + 1];
    uint_fast32_t code = 0;
    for(int bits = 1; bits <= maxCodeLength; bits++)
    {
        code = (code + l_count[bits - 1]) << 1;
        next_code[bits] = code;
    }
    for(int i = 0; i < n; i++)
    {
        if(symbols[i]->compressedLength != 0)
        {
            symbols[i]->compressedSymbolCode = next_code[symbols[i]->compressedLength];
            next_code[symbols[i]->compressedLength]++;
        }
    }
}

}
