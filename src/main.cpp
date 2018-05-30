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
#include "ppm_wizard.h"
#include "arithmetic_wizard.h"

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

    for(int i = 0; i < 20; i++)
    {
        ArabicUtfConverter::encodeFile(files[i], compressedFiles[i], "symbols");
        ArabicUtfConverter::decodeFile(compressedFiles[i], decompressedFiles[i], "symbols");
    }

    /*
    for(int i = 0; i < 20; i++)
    {
        ArithmeticWizard arithWiz(new PPMModel());
       ArithmeticWizard arithWiz2(new PPMModel());
        std::cout << "Compressing #" << i << std::endl;
        arithWiz.encode(files[i], compressedFiles[i]);
        std::cout << "Decompressing #" << i << std::endl;
        arithWiz2.decode(compressedFiles[i], decompressedFiles[i]);
    }
    */

}
