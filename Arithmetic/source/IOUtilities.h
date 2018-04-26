#pragma once
#include "stdafx.h"
/* Boost 1.67 is used to load files quickly into memory. */
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
using namespace boost::interprocess;

void readBinaryFile(vector<bool>& output, string inFileName);
void readBinaryFile(vector<char>& output, string inFileName);
void printBinaryFile(vector<bool>& output, string outputFileName);