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


void readBinaryFile(vector<char>& output, string inFileName)
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
		output.push_back(addr[i]);
	}
}

void printBinaryFile(vector<bool>& output, string outputFileName)
{
	ofstream outFile(outputFileName, std::ios::binary);
	ull size = ceil(output.size() / 8.0f);
	char* cArray = new char[size];
	int currWordPos = 0;
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
		cArray[currWordPos] = (char)tempNum;
		currWordPos++;
	}
	outFile.write(cArray, size);
	delete[] cArray;
	outFile.close();
}