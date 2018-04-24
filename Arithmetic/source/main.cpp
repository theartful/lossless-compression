#include "stdafx.h"
#include "Alphabet.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"
#include "IOUtilities.h"

void testCoder(string inputFileName, string outputFileName)
{
	/* Make an alphabet. */
	int numChars = 256;
	Alphabet alphabet(numChars);
	alphabet.cumulativeCount[0] = 1;
	for (int i = 1; i < numChars; i++)
		alphabet.cumulativeCount[i] = 1 + alphabet.cumulativeCount[i - 1];
	alphabet.totalCount = numChars;

	/* Read the sequence to be encoded. */
	vector<bool> sequenceToEncode;
	readBinaryFile(sequenceToEncode, "DataSet_1.tsv");
	sequenceToEncode.push_back(numChars - 1); // EOF, so that we know when to stop encoding.
	cout << "Started Encoding.\n";

	/* Make an encoder using the created alphabet. */
	ArithmeticEncoder encoder(&alphabet);
	/* Encode the sequence. */
	auto start = std::chrono::system_clock::now();
	vector<bool> output = encoder.EncodeSequence(sequenceToEncode);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	/* Print some stats. */
	cout << "Encoding Finished. Time elapsed = " << elapsed_seconds.count() << " seconds. \n";
	cout << "Size to be encoded: " << sequenceToEncode.size() / (8.0 * 1024 * 1024)  << " megabytes. \n";
	cout << "Size encoded: " << output.size() / (8.0 * 1024 * 1024) << " megabytes. \n";
	cout << "Compression ratio: " << sequenceToEncode.size() * 1.0f / output.size() << ".\n";

	/* Print the decoded file. */
	printBinaryFile(output, "out-file.bin");
	/* Now read it again, to verify the encoding. */
	output.clear();
	readBinaryFile(output, "out-file.bin");
	/* Create a decoded using the same alphabet, and decode the sequence. */
	ArithmeticDecoder decoder(&alphabet);
	cout << "Started Decoding.\n";
	start = std::chrono::system_clock::now();
	vector<bool> decodedSequence = decoder.DecodeSequence(output);
	end = std::chrono::system_clock::now();
	elapsed_seconds = end - start;
	cout << "Decoding Finished. Time elapsed = " << elapsed_seconds.count() << " seconds \n";
	/* Write any errors if there are any. */
	bool error = false;
	for (int i = 0; i < sequenceToEncode.size(); i++)
	{
		if (sequenceToEncode[i] != decodedSequence[i])
		{
			cout << "Error: " << sequenceToEncode[i] << ", " << decodedSequence[i] << "\n";
			error = true;
		}
	}
	if (!error)
	{
		cout << "Decoding Successful.\n";
	}
}


int main()
{
	testCoder("DataSet_1.txt", "output.bin");
	return 0;
}

