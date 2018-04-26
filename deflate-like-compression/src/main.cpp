#include <iostream>
#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include "lzw_wizard.h"
#include "utf_reader.h"
#include <bitset>
#include <vector>
#include <algorithm>
#include "arabic_utf_converter.h"
#include "huffman_wizard.h"

using namespace std;

typedef util::Symbol Symbol;

char* files[20] =
{
"test-files/DataSet/DataSet_1.tsv", "test-files/DataSet/DataSet_2.tsv", "test-files/DataSet/DataSet_3.tsv", "test-files/DataSet/DataSet_4.tsv",
"test-files/DataSet/DataSet_5.tsv", "test-files/DataSet/DataSet_6.tsv", "test-files/DataSet/DataSet_7.tsv", "test-files/DataSet/DataSet_8.tsv", "test-files/DataSet/DataSet_9.tsv",
"test-files/DataSet/DataSet_10.tsv", "test-files/DataSet/DataSet_11.tsv", "test-files/DataSet/DataSet_12.tsv", "test-files/DataSet/DataSet_13.tsv", "test-files/DataSet/DataSet_14.tsv",
"test-files/DataSet/DataSet_15.tsv", "test-files/DataSet/DataSet_16.tsv", "test-files/DataSet/DataSet_17.tsv", "test-files/DataSet/DataSet_18.tsv", "test-files/DataSet/DataSet_19.tsv",
"test-files/DataSet/DataSet_20.tsv"
};

char* compressedFiles[20] =
{
"test-files/compressed/DataSet_1.bin", "test-files/compressed/DataSet_2.bin", "test-files/compressed/DataSet_3.bin", "test-files/compressed/DataSet_4.bin",
"test-files/compressed/DataSet_5.bin", "test-files/compressed/DataSet_6.bin", "test-files/compressed/DataSet_7.bin", "test-files/compressed/DataSet_8.bin", "test-files/compressed/DataSet_9.bin",
"test-files/compressed/DataSet_10.bin", "test-files/compressed/DataSet_11.bin", "test-files/compressed/DataSet_12.bin", "test-files/compressed/DataSet_13.bin", "test-files/compressed/DataSet_14.bin",
"test-files/compressed/DataSet_15.bin", "test-files/compressed/DataSet_16.bin", "test-files/compressed/DataSet_17.bin", "test-files/compressed/DataSet_18.bin", "test-files/compressed/DataSet_19.bin",
"test-files/compressed/DataSet_20.bin"
};

char* decompressedFiles[20] =
{
"test-files/DataSet/DataSet_1-out.tsv", "test-files/DataSet/DataSet_2-out.tsv", "test-files/DataSet/DataSet_3-out.tsv", "test-files/DataSet/DataSet_4-out.tsv",
"test-files/DataSet/DataSet_5-out.tsv", "test-files/DataSet/DataSet_6-out.tsv", "test-files/DataSet/DataSet_7-out.tsv", "test-files/DataSet/DataSet_8-out.tsv", "test-files/DataSet/DataSet_9-out.tsv",
"test-files/DataSet/DataSet_10-out.tsv", "test-files/DataSet/DataSet_11-out.tsv", "test-files/DataSet/DataSet_12-out.tsv", "test-files/DataSet/DataSet_13-out.tsv", "test-files/DataSet/DataSet_14-out.tsv",
"test-files/DataSet/DataSet_15-out.tsv", "test-files/DataSet/DataSet_16-out.tsv", "test-files/DataSet/DataSet_17-out.tsv", "test-files/DataSet/DataSet_18-out.tsv", "test-files/DataSet/DataSet_19-out.tsv",
"test-files/DataSet/DataSet_20-out.tsv"
};

int main()
{

    int numberOfLiterals = 113;

    Symbol** symbols = new Symbol*[numberOfLiterals];
    for(int i = 0; i < numberOfLiterals; i++)
    {
        symbols[i] = new Symbol;
        symbols[i]->symbolCode = i;
        symbols[i]->length = 8;
        symbols[i]->type = util::LITERAL;
    }

    Buffer* slidingWindow = new Buffer(LzwWizard::slidingWindowSize, true);
    Buffer* slidingWindow2 = new Buffer(LzwWizard::slidingWindowSize, false);

    for(int i = 0; i < 20; i++)
    {
        std::cout << "Compressing file #" << i << std::endl;
        ArabicUtfConverter::encodeFile(files[i], "test-files/out", "symbols");
        std::cout << "\tArabicUtfConverter finished" << std::endl;
        LzwWizard::encodeFile("test-files/out", "test-files/out2", slidingWindow);
        std::cout << "\tLzwWizard finished" << std::endl;
        HuffmanWizard::encodeLzwEncoded("test-files/out2", compressedFiles[i], symbols, numberOfLiterals);
        std::cout << "\tHuffmanWizard finished" << std::endl;

        std::cout << "Decompressing file #" << i << std::endl;
        HuffmanWizard::decodeLzwEncoded(compressedFiles[i], "test-files/out", symbols, numberOfLiterals);
        std::cout << "\tHuffmanWizard finished" << std::endl;
        LzwWizard::decodeFile("test-files/out", "test-files/out2", slidingWindow2);
        std::cout << "\tLzwWizard finished" << std::endl;
        ArabicUtfConverter::decodeFile("test-files/out2", decompressedFiles[i], "symbols");
        std::cout << "\tArabicUtfConverter finished" << std::endl;
    }

    for(int i = 0; i < numberOfLiterals; i++)
    {
        delete symbols[i];
    }
    delete[] symbols;
    delete slidingWindow;
    delete slidingWindow2;
}
