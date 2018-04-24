#include "stdafx.h"
#include "IOUtilities.h"

void readBinaryFile(vector<bool>& output, string inFileName)
{
	// Create the file mapping
	file_mapping fm(inFileName.c_str(), read_only);
	// Map the file in memory
	mapped_region region(fm, read_only);
	// Get the address where the file has been mapped
	char* addr = (char*)region.get_address();
	std::size_t elements = region.get_size() / sizeof(char);
	for (int i = 0; i < elements; i++)
	{
		char temp = addr[i];
		int check = 0;
		int multiple = 128;
		for (int i = 7; i >= 0; i--)
		{
			check += temp&multiple;
			output.push_back((bool)(temp & multiple));
			multiple /= 2;
		}
	}
}

void printBinaryFile(vector<bool>& output, string outputFileName)
{
	ofstream outFile(outputFileName, std::ios::binary);
	char* cArray = new char[output.size() / 8];
	for (auto i = 0; i < output.size();)
	{
		auto j = (output.size() - i < 8) ? output.size() - i : 8;
		int tempNum = 0;
		int multiple = pow(2, j - 1);
		for (; j > 0; j--)
		{
			tempNum += output[i] * multiple;
			i++;
			multiple /= 2;
		}
		cArray[i / 8 - 1] = (char)tempNum;
	}
	outFile.write(cArray, output.size() / 8);
	delete[] cArray;
	outFile.close();
}