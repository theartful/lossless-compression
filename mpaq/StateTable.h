#pragma once
#include "stdafx.h"
#include "MathUtilities.h"

/** State table:
/*	State 0 is the starting state.
/*	States 1-30 represent all possible sequences of 1-4 bits.
/*	States 31-255 represent a pair of counts, (n0,n1), the number of 0 and 1 bits respectively.
/*	For states 31-67 where n0+n1 < 8 and n0 > 0 and n1 > 0, there are two states for
/*	each (n0,n1) pair depending on whether the last bit was a 0 or 1.
/*  There are no states where both n0 and n1 are large:
/*   n0 < 64.                  n1 < 64.
/*   If n0 = 1 then n1 < 32.   If n1 = 1 then n0 < 32.
/*   If n0 = 2 then n1 < 16.   If n1 = 2 then n0 < 16.
/*   If n0 = 3 then n1 < 8.    If n1 = 3 then n0 < 8.
/*	On bit 0, n0 is incremented and if n1 > 2 then n1 := 2 log2(n1).
/*   (This rule tends to favor newer data over old).
/*	On bit 1, then n1 is incremented and n0 is reduced as above.
/*	If an increment leads to a nonexistent state, then go to the
/*   nearest state with about the same ratio (n0+1/4)/(n1+1/4).
/*   For example, state (7,3) on bit 0 would lead to nonexistent
/*   state (8,3), so go to (5,2) instead.
/*   State (31,1) on 0 would go to nonexistent state (32,1) so
/*   go to (8,0) instead. */
class StateTable {
private:
	vector<uchar> nextState; // nextState[state * 4 + y] = nextState if y = lastBit (0/1), n0 if y=2, n1 if y=3
	static const int maxOtherCount[5]; // maximumCount[nx] = max(n_(1-x)).
									   // transitionTable[x][y][0] = state number, transitionTable[x][y][1] = count of states 
	static int transitionTable[65][65][2];
	/* Returns the number of states with n0 = x, n1 = y or vice versa. */
	int CountStates(int x, int y);
	/* Discount the value of x if the opposite bit is observed. */
	void Discount(int& x);
	/* Compute next x, y (0 to 64) given input bit (0, 1) */
	void NextState(int& x, int& y, int bit);
	/* Initialize Transition Table. */
	void InitializeTransitionTable();
	/* Initialize next states array. */
	void InitializeNextStates();
public:
	// State -> last bit: 1 = last bit is 0, 2 = last bit is 1, 0=unknown
	const uchar lastBit[256] = {
		0, 1,2, 1,2,1,2, 1,1,1,2,1,2,2,2,1,1,1,1,2,1,1,1,2,2,2,1,2,2,2,2,
		1,1,2,1,2,1,2,1,2,2, 1,1,2,1,2,1,2,1,2,1,2,2, 1,1,2,1,2,1,2,1,2,1,2,1,2,2 };
	StateTable();
	//	stateTable(state, 0) = next state if y = 0, 0 <= state < 256
	//	stateTable(state, 1) = next state if y = 1
	//	stateTable(state, 2) = number of zeros in bit history represented by state
	//	stateTable(state, 3) = number of ones in bit history represented by state
	int operator() (int state, int selection);
};

//	StateInfo(state, 0) = next state if y = 0, 0 <= state < 256
//	StateInfo(state, 1) = next state if y = 1
//	StateInfo(state, 2) = number of zeros in bit history represented by state
//	StateInfo(state, 3) = number of ones in bit history represented by state
int StateInfo(int state, int selection);