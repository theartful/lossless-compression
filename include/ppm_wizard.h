#ifndef PPMWIZARD_H
#define PPMWIZARD_H

#include "huffman_wizard.h"
#include "basic_structs.h"
#include "write_buffer.h"
#include "read_buffer.h"
#include "buffer.h"
#include "model.h"
#include <math.h>

#include <iostream>

class PPMModel : public Model
{

private:
    static const int MAX_CONTEXT = 9;
    static const int NUMBER_OF_SYMBOLS = 114 + 1;
    static const int EOF_SYMBOL = NUMBER_OF_SYMBOLS - 1;
    static const int SYMBOL_LENGTH = 8;
    static const int F = 50;
    static const int D = MAX_CONTEXT;
    static const ull DENOMINATOR = ((ull)1) << 28;
    static const ull MINIMUM_PROP = 2;

    double alpha_matrix[F][D];
    double beta_matrix[F][D];

    ull symbolProbabilities[NUMBER_OF_SYMBOLS];
    double lol = 1.0d/ NUMBER_OF_SYMBOLS;
    double contextProbabilities[MAX_CONTEXT + 1];

    int bitNumber;
    ull zeroProp;
    ull oneProp;

public:
    struct TrieNode
    {
        TrieNode* trieNodes[NUMBER_OF_SYMBOLS];
        ull frequency;
        ull cum_count;

        TrieNode()
        {
            for(int i = 0; i < NUMBER_OF_SYMBOLS; i++)
                trieNodes[i] = nullptr;
            frequency = 0;
            cum_count = 0;
        }

        ~TrieNode()
        {
            for(int i = 0; i < NUMBER_OF_SYMBOLS; i++)
            {
                if(trieNodes[i] != nullptr) delete trieNodes[i];
            }
        }
    };

private:
    TrieNode* trie;
    Buffer* context;
    int currentContext = 0;

    TrieNode* getTrieNode(int depth, bool createAlongTheWay);
    void updateFrequencies(int byte);
    inline int getFanout(TrieNode* node);
    void updateCoefficients(int symbol);
    double calculateDerivativeAlpha(int depth, int symbol, double alpha, double beta);
    double calculateDerivativeBeta(int depth, double alpha, double beta);
    double _getProbability(int depth, int k);
    ull getProbability(int k);

public:
    PPMModel();
    void addSymbol(int symbol);
    ull getTotCount();
    ull getCumCount(int k);
    int getNumberOfSymbols();
    int getSymbolLength();
    int getEof();
    void compressSymbol(int byte, ull& sym_low, ull& sym_high, ull& tot_count);
    ~PPMModel();
};

#endif // PPMWIZARD_H
