#ifndef ARITHMETICWIZARD_H
#define ARITHMETICWIZARD_H

#include "read_buffer.h"
#include "write_buffer.h"
#include "model.h"
#include <math.h>
#include <iostream>
#include <queue>

class ArithmeticWizard
{
    typedef uint_fast32_t ull;

private:
    unsigned const int PRECISION = 32;
    const uint_fast64_t MAX_RANGE = ((ull) 1) << PRECISION;
    const ull MSB_MASK = ((ull) 1) << (PRECISION - 1);
    const ull QUARTER = ((ull) 1) << (PRECISION - 2);
    const ull CLEAN_MASK = MAX_RANGE - 1;

    WriteBuffer* writeBuffer;
    ReadBuffer* readBuffer;

    Model* model;
    ull l;
    ull h;
    ull tag;
    int scale3;

    void rescale();

public:

    ArithmeticWizard(Model* model);
    void encodeSymbol(ull sym_low, ull sym_high, ull tot_count);

    void encode(char* fromFile, char* toFile);
    void decode(char* fromFile, char* toFile);

    ~ArithmeticWizard();

protected:

};

#endif // ARITHMETICWIZARD_H
