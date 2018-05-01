#include "stdafx.h"
#include "ProbabilityModel.h"
#include "MixerModel.h"
#include "IOUtilities.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"
#include <chrono>

void testCoder(string inputFileName, string outputFileName, bool printProgress = false)
{
	ProbabilityModel* model = new MixerModel();
	/* Read the sequence to be encoded. */
	vector<bool> sequenceToEncode;
	readBinaryFile(sequenceToEncode, inputFileName);
	cout << "Started Encoding.\n";
	/* Make an encoder using the created alphabet. */
	ArithmeticEncoder encoder(model);
	/* Encode the sequence. */
	auto start = std::chrono::system_clock::now();
	vector<bool> output = encoder.EncodeSequence(sequenceToEncode, printProgress);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	/* Print some stats. */
	cout << "Encoding Finished. Time elapsed = " << elapsed_seconds.count() << " seconds. \n";
	cout << "Size to be encoded: " << sequenceToEncode.size() / (8.0 * 1024 * 1024) << " megabytes. \n";
	cout << "Size encoded: " << output.size() / (8.0 * 1024 * 1024) << " megabytes. \n";
	cout << "Compression ratio: " << sequenceToEncode.size() * 1.0f / output.size() << ".\n";
	cout << "Bits per character: " << output.size() * 8.0f / sequenceToEncode.size() << ".\n";
	/* Print the encoded file. */
	printBinaryFile(output, outputFileName);
	/* Now read it again, to verify the encoding. */
	output.clear();
	readBinaryFile(output, outputFileName);

	/* Create a new alphabet. */
	delete model;
	model = new MixerModel();
	ArithmeticDecoder decoder(model);
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
			cout << "Error: at position " << i << ": " << sequenceToEncode[i] << ", " << decodedSequence[i] << "\n";
			error = true;
		}
	}
	if (!error)
	{
		cout << "Decoding Successful.\n";
	}
	delete model;
}

int main()
{
	testCoder("DataSet_1.tsv", "output.bin", false);
    return 0;
}

