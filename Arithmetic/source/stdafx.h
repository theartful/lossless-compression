// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define MAX_CHILDREN 256
#define MAX_DEPTH	5
#define MAX_PRECISION 12
#define MAX_PROBABILITY 4096
#define NUM_CHARACTERS 2

#include "targetver.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>
#include <string>
#include <exception>
// #include <boost/multiprecision/gmp.hpp>
// namespace mp = boost::multiprecision;
// using ull = mp::mpz_int;
using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::exception;
using ull = unsigned long long;
using uint = unsigned int;
using uchar = unsigned char;