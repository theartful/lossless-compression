#include "stdafx.h"
#include "SimpleAlphabet.h"
#include "ArithmeticEncoder.h"
#include "ArithmeticDecoder.h"
#include "IOUtilities.h"
#include "AdaptiveAlphabet.h"

void testTrie()
{
	Trie t;
	vector<int> string;
	string.push_back('H');
	t.VisitString(string);
	string.push_back('E');
	t.VisitString(string);
	string.push_back('L');
	t.VisitString(string);
	string.push_back('L');
	t.VisitString(string);
	string.push_back('O');
	t.VisitString(string);
	string.push_back(' ');
	t.VisitString(string);
	string.push_back('W');
	t.VisitString(string);
	string.push_back('O');
	t.VisitString(string);
	string.push_back('R');
	t.VisitString(string);
	string.push_back('L');
	t.VisitString(string);
	string.push_back('D');
	t.VisitString(string);
	string.push_back('!');
	t.VisitString(string);
	string.push_back('R');
	t.VisitString(string);
	string.push_back('L');
	t.VisitString(string);
	string.push_back('D');
	t.VisitString(string);
	cout << "string = ";
	for (auto word : string)
		cout << (char)word;
	cout << "\n";
	t.PrintProbabilities();
	vector<int> query;
	query.push_back('!');
	query.push_back('R');
	query.push_back('L');
	query.push_back('D');
	ull cumProb;
	ull totProb;
	t.GetCumulativeProbability(cumProb, totProb, query, 3);
	cout << cumProb << " / " << totProb << "\n";
}

void testCoder(string inputFileName, string outputFileName)
{
	/* Make an alphabet. */
	int numChars = 256;
	SimpleAlphabet alphabet(numChars);
	/* Read the sequence to be encoded. */
	vector<bool> sequenceToEncode;
	readBinaryFile(sequenceToEncode, inputFileName);
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
	printBinaryFile(output, outputFileName);
	/* Now read it again, to verify the encoding. */
	output.clear();
	readBinaryFile(output, outputFileName);
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
			cout << "Error: at position " << i << ": " << sequenceToEncode[i] << ", " << decodedSequence[i] << "\n";
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
	//testCoder("DataSet/DataSet_1.tsv", "output.bin");
	testTrie();
	return 0;
}

