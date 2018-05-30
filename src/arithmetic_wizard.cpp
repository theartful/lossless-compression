#include "arithmetic_wizard.h"

ArithmeticWizard::ArithmeticWizard(Model* model)
{
    l = 0;
    h = MAX_RANGE - 1;
    scale3 = 0;
    tag = 0;
    this->model = model;

    writeBuffer = nullptr;
    readBuffer = nullptr;
}

void ArithmeticWizard::encode(char* fromFile, char* toFile)
{
    readBuffer = new ReadBuffer(1024 * 32, fromFile);
    writeBuffer = new WriteBuffer(1024 * 32, toFile);

    this->readBuffer = readBuffer;
    this->writeBuffer = writeBuffer;
    ull sym_low, sym_high, tot_count;

    while(!readBuffer->isEmpty())
    {
        int symbol = readBuffer->readByte();

        model->compressSymbol(symbol, sym_low, sym_high, tot_count);
        encodeSymbol(sym_low, sym_high, tot_count);
        model->addSymbol(symbol);
    }

    model->compressSymbol(model->getEof(), sym_low, sym_high, tot_count);
    encodeSymbol(sym_low, sym_high, tot_count);;
    model->addSymbol(model->getEof());

    for(int i = 0; i < PRECISION; i++)
    {
        int bit = (l >> (PRECISION - 1)) & 1;
        writeBuffer->writeSymbol(bit, 1);
        l = (l << 1);
        while (scale3 > 0)
        {
            writeBuffer->writeSymbol(~bit, 1);
            scale3--;
        }
    }
    writeBuffer->finish();
    delete readBuffer;
    delete writeBuffer;
}


void ArithmeticWizard::encodeSymbol(ull sym_low, ull sym_high, ull tot_count)
{
    ull range = h - l + 1;
    h = l + ((range * sym_high) / tot_count) - 1;
    l = l + (range * sym_low) / tot_count;

    while((l & MSB_MASK) == (h & MSB_MASK))
    {
        int bit = (l >> (PRECISION - 1)) & 1;
        writeBuffer->writeSymbol(bit, 1);
        while(scale3 > 0)
        {
            writeBuffer->writeSymbol(~bit, 1);
            scale3--;
        }
        l = l << 1;
        h = (h << 1) + 1;
    }

    while((l & QUARTER) != 0 && (h & QUARTER) == 0)
    {
        l = l << 1;
        h = (h << 1) + 1;
        l = l ^ MSB_MASK;
        h = h ^ MSB_MASK;
        scale3++;
    }
    h &= CLEAN_MASK;
    l &= CLEAN_MASK;
}

void ArithmeticWizard::decode(char* fromFile, char* toFile)
{
    readBuffer = new ReadBuffer(1024 * 32, fromFile);
    writeBuffer = new WriteBuffer(1024 * 32, toFile);

    tag = readBuffer->readSymbol(PRECISION);

    while (!readBuffer->isEmpty())
    {
        ull tot_count = model->getTotCount();
        ull tmp = ((tag - l + 1) * tot_count - 1) / (h - l + 1);
        int k = 0;
        while(tmp >= model->getCumCount(k))
            k++;

        if(k >= 0 && k < model->getNumberOfSymbols() - 1)
            writeBuffer->writeSymbol(k, 8);

		ull range = (h - l + 1);

        h = l + (range * model->getCumCount(k)) / tot_count - 1;
        l = l + (range * model->getCumCount(k - 1)) / tot_count;

        model->addSymbol(k);
        if(k == model->getEof()) break;

        while((l & MSB_MASK) == (h & MSB_MASK))
        {
            l = l << 1;
            h = (h << 1) + 1;
            tag = (tag << 1) | ((ull) readBuffer->readSymbol(1));
        }

        while((l & QUARTER) != 0 && (h & QUARTER) == 0)
        {
            l = l << 1;
            h = (h << 1) + 1;
            tag = (tag << 1) | ((ull) readBuffer->readSymbol(1));
            l = l ^ (MSB_MASK);
            h = h ^ (MSB_MASK);
            tag = tag ^ (MSB_MASK);
        }

        h &= CLEAN_MASK;
        l &= CLEAN_MASK;
        tag &= CLEAN_MASK;
	}
    writeBuffer->finish();
    delete readBuffer;
    delete writeBuffer;
}

ArithmeticWizard::~ArithmeticWizard()
{
    delete model;
}
