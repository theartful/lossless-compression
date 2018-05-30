#include "huffman_wizard.h"
#include <iostream>

namespace HuffmanWizard
{

typedef util::Node Node;
typedef util::Symbol Symbol;

int DISTANCES_SIZE = 8;
int LENGTHES_SIZE = 8;

int HUFFMAN_HEADER_LENGTH = 7;

unsigned int blockSize = 1024 * 32;


void decodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** literals, int numberOfLiterals)
{
    ReadBuffer readBuffer(1024 * 32, fromFile);
    WriteBuffer writeBuffer(1024 * 32, toFile);

    Symbol** lengthes = new Symbol*[1 << LENGTHES_SIZE];
    Symbol** distances = new Symbol*[1 << DISTANCES_SIZE];

    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
    {
        lengthes[i] = new Symbol();
        lengthes[i]->symbolCode = i;
        lengthes[i]->length = LENGTHES_SIZE;
        lengthes[i]->type = util::LENGTH;
    }

    for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
    {
        distances[i] = new Symbol();
        distances[i]->symbolCode = i;
        distances[i]->length = DISTANCES_SIZE;
        distances[i]->type = util::DISTANCE;
    }

    Symbol** literalsAndLengthes = new Symbol*[(1 << LENGTHES_SIZE) + numberOfLiterals];
    for(int i = 0; i < numberOfLiterals; i++)
        literalsAndLengthes[i] = literals[i];
    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
        literalsAndLengthes[i + numberOfLiterals] = lengthes[i];

    while(!readBuffer.isEmpty())
    {
        for(int i = 0; i < numberOfLiterals; i++)
        {
            int code = readBuffer.readSymbol(HUFFMAN_HEADER_LENGTH);
            if(code == -1)
                break;
            literals[i]->compressedLength = code;
            literals[i]->compressedSymbolCode = 0;
        }

        for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
        {
            int code = readBuffer.readSymbol(HUFFMAN_HEADER_LENGTH);
            if(code == -1)
                break;
            lengthes[i]->compressedLength = code;
            lengthes[i]->compressedSymbolCode = 0;
        }

        for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
        {
            int code = readBuffer.readSymbol(HUFFMAN_HEADER_LENGTH);
            if(code == -1)
                break;
            distances[i]->compressedLength = code;
            distances[i]->compressedSymbolCode = 0;
        }

        setCanonicalCodes(literalsAndLengthes, numberOfLiterals + (1 << LENGTHES_SIZE));
        setCanonicalCodes(distances, 1 << DISTANCES_SIZE);

        Node* literalsAndLengthesSearchTree = new Node;
        Node* distancesSearchTree = new Node;

        for(int i = 0; i < numberOfLiterals + (1 << LENGTHES_SIZE); i++)
        {
            util::addSymbol(literalsAndLengthes[i], literalsAndLengthesSearchTree, util::getCompressedCode, util::getCompreessedLength);
        }

        for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
        {
            util::addSymbol(distances[i], distancesSearchTree, util::getCompressedCode, util::getCompreessedLength);
        }

        int tmp1, tmp2;
        unsigned int i = 0;
        Symbol* symbol;
        Symbol* distanceSymbol;

        while(!readBuffer.isEmpty() && i < blockSize)
        {
            symbol = readBuffer.readSymbol(literalsAndLengthesSearchTree);

            if(symbol == nullptr)
                break;

            switch(symbol->type)
            {
            case util::LITERAL:
                writeBuffer.writeSymbol(0, 1);
                writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);
                break;

            case util::LENGTH:
                // read rest of length
                tmp2 = readBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp2 == -1)
                    break;

                distanceSymbol = readBuffer.readSymbol(distancesSearchTree);
                if(distanceSymbol == nullptr)
                    break;

                tmp1 = readBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp1 == -1)
                    break;

                writeBuffer.writeSymbol(1, 1);

                writeBuffer.writeSymbol(symbol->symbolCode, symbol->length);
                writeBuffer.writeSymbol(tmp2, LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);

                writeBuffer.writeSymbol(distanceSymbol->symbolCode, distanceSymbol->length);
                writeBuffer.writeSymbol(tmp1, LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);

                break;
            }
            i++;
        }
        delete literalsAndLengthesSearchTree;
        delete distancesSearchTree;
    }
    writeBuffer.finish();

    for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
    {
        delete distances[i];
    }
    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
    {
        delete lengthes[i];
    }
    delete[] lengthes;
    delete[] literalsAndLengthes;
    delete[] distances;
}

void clearFrequencies(util::Symbol** symbols, int n)
{
    for(int i = 0; i < n; i++)
    {
        symbols[i]->frequency = 0;
    }
}

void encodeLzwEncoded(char* fromFile, char* toFile, util::Symbol** literals, int numberOfLiterals)
{
    ReadBuffer analyzerBuffer(1024 * 32, fromFile);
    ReadBuffer encoderBuffer(1024 * 32, fromFile);
    WriteBuffer writeBuffer(1024 * 32, toFile);

    Node* literalsSearchTree = new Node;
    Node* lengthesSearchTree = new Node;
    Node* distancesSearchTree = new Node;

    Symbol** distances = new Symbol*[1 << DISTANCES_SIZE];
    Symbol** lengthes = new Symbol*[1 << LENGTHES_SIZE];

    // set up literal symbols
    for(int i = 0; i < numberOfLiterals; i++)
    {
        util::addSymbol(literals[i], literalsSearchTree, util::getSymbolCode, util::getSymbolLength);
    }

    // set up length symbols
    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
    {
        lengthes[i] = new Symbol();
        lengthes[i]->symbolCode = i;
        lengthes[i]->length = LENGTHES_SIZE;
        util::addSymbol(lengthes[i], lengthesSearchTree, util::getSymbolCode, util::getSymbolLength);
    }

    // set up distance symbols
    for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
    {
        distances[i] = new Symbol();
        distances[i]->symbolCode = i;
        distances[i]->length = DISTANCES_SIZE;
        util::addSymbol(distances[i], distancesSearchTree, util::getSymbolCode, util::getSymbolLength);
    }

    Symbol** literalsAndLengthes = new Symbol*[(1 << LENGTHES_SIZE) + numberOfLiterals];
    for(int i = 0; i < numberOfLiterals; i++)
        literalsAndLengthes[i] = literals[i];
    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
        literalsAndLengthes[i + numberOfLiterals] = lengthes[i];

    while(!encoderBuffer.isEmpty())
    {
        unsigned int i = 0;
        while(!analyzerBuffer.isEmpty() && i < blockSize)
        {
            int bit = analyzerBuffer.readSymbol(1);
            if(bit == -1)
                break;
            if(bit)
            {
                // read length
                Symbol* lengthSymbol = analyzerBuffer.readSymbol(lengthesSearchTree);
                if(lengthSymbol == nullptr)
                    break;

                // read rest of length
                int tmp = analyzerBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp == -1)
                    break;

                // read distance
                Symbol* distanceSymbol = analyzerBuffer.readSymbol(distancesSearchTree);
                if(distanceSymbol == nullptr)
                    break;

                // read rest of distance
                tmp = analyzerBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp == -1)
                    break;

                lengthSymbol->frequency++;
                distanceSymbol->frequency++;
            }
            else
            {
                Symbol* symbol = analyzerBuffer.readSymbol(literalsSearchTree);
                if(symbol == nullptr)
                    break;

                symbol->frequency++;
            }
            i++;
        }

        assignCodes(distances, 1 << DISTANCES_SIZE);
        clearFrequencies(distances, 1 << DISTANCES_SIZE);

        assignCodes(literalsAndLengthes, numberOfLiterals + (1 << LENGTHES_SIZE));
        clearFrequencies(literalsAndLengthes, numberOfLiterals + (1 << LENGTHES_SIZE));

        for(int j = 0; j < numberOfLiterals; j++)
        {
            writeBuffer.writeSymbol(literals[j]->compressedLength, HUFFMAN_HEADER_LENGTH);
        }
        for(int j = 0; j < (1 << LENGTHES_SIZE); j++)
        {
            writeBuffer.writeSymbol(lengthes[j]->compressedLength, HUFFMAN_HEADER_LENGTH);
        }

        for(int j = 0; j < (1 << DISTANCES_SIZE); j++)
        {
            writeBuffer.writeSymbol(distances[j]->compressedLength, HUFFMAN_HEADER_LENGTH);
        }

        i = 0;
        while(!encoderBuffer.isEmpty() && i < blockSize)
        {
            int bit = encoderBuffer.readSymbol(1);
            if(bit)
            {
                // read length
                Symbol* lengthSymbol = encoderBuffer.readSymbol(lengthesSearchTree);
                if(lengthSymbol == nullptr)
                    break;

                // read rest of length
                int tmp1 = encoderBuffer.readSymbol(LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                if(tmp1 == -1)
                    break;

                // read distance
                Symbol* distanceSymbol = encoderBuffer.readSymbol(distancesSearchTree);
                if(distanceSymbol == nullptr)
                    break;

                // read rest of distance
                int tmp2 = encoderBuffer.readSymbol(LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
                if(tmp2 == -1)
                    break;

                writeBuffer.writeSymbol(lengthSymbol->compressedSymbolCode, lengthSymbol->compressedLength);
                writeBuffer.writeSymbol(tmp1, LzwWizard::BUFFER_BITS_SIZE - LENGTHES_SIZE);
                writeBuffer.writeSymbol(distanceSymbol->compressedSymbolCode, distanceSymbol->compressedLength);
                writeBuffer.writeSymbol(tmp2, LzwWizard::SLIDING_WINDOW_SIZE - DISTANCES_SIZE);
            }
            else
            {
                Symbol* symbol = encoderBuffer.readSymbol(literalsSearchTree);
                if(symbol == nullptr)
                {
                    break;
                }

                writeBuffer.writeSymbol(symbol->compressedSymbolCode, symbol->compressedLength);
            }
            i++;
        }
    }
    writeBuffer.finish();

    delete literalsSearchTree;
    delete distancesSearchTree;
    delete lengthesSearchTree;

    for(int i = 0; i < (1 << DISTANCES_SIZE); i++)
    {
        delete distances[i];
    }
    for(int i = 0; i < (1 << LENGTHES_SIZE); i++)
    {
        delete lengthes[i];
    }
    delete[] distances;
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
        symbols[i]->compressedLength = 0;
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
    for(int i = 0; i <= maxCodeLength; i++)
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


void encodeCharacter(util::Symbol** symbols, int n, WriteBuffer& writeBuffer, util::Symbol* symbol)
{
    assignCodes(symbols, n);
    writeBuffer.writeSymbol(symbol->compressedSymbolCode, symbol->compressedLength);
}

Symbol* decodeCharacter(Symbol** symbols, int n, ReadBuffer& readBuffer)
{
    assignCodes(symbols, n);
    Node* searchTree = new Node();
    for(int i = 0; i < n; i++)
    {
        util::addSymbol(symbols[i], searchTree, util::getCompressedCode, util::getCompreessedLength);
    }
    Symbol* symbol = readBuffer.readSymbol(searchTree);
    delete searchTree;
    return symbol;
}

}
