#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <queue>

class Model
{
public:
    typedef uint_fast32_t ull;

    Model();

    virtual void addSymbol(int symbol) = 0;
    virtual ull getTotCount() = 0;
    virtual ull getCumCount(int k) = 0;
    virtual int getNumberOfSymbols() = 0;
    virtual int getSymbolLength() = 0;
    virtual int getEof() = 0;
    virtual void compressSymbol(int byte, ull& sym_low, ull& sym_high, ull& tot_count) = 0;
    virtual ~Model();
};

#endif // MODEL_H
