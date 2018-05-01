#include "stdafx.h"
#include "StateTable.h"

const int StateTable::maxOtherCount[5] = { 64, 32, 16, 8, 5 };
int StateTable::transitionTable[65][65][2];

int StateTable::operator() (int state, int selection)
{
	return nextState[state * 4 + selection];
}

void StateTable::InitializeNextStates()
{
	int state = 0;
	for (int i = 0; i < 64; i++)
	{
		for (int y = 0; y <= i; y++)
		{
			int x = i - y;
			for (int k = 0; k < transitionTable[x][y][1]; k++)
			{
				int x0 = x, y0 = y, x1 = x, y1 = y;  // next x,y for input 0,1
				int ns0 = 0, ns1 = 0;
				if (state<15) {
					++x0;
					++y1;
					ns0 = transitionTable[x0][y0][0] + state - transitionTable[x][y][0];
					ns1 = transitionTable[x1][y1][0] + state - transitionTable[x][y][0];
					if (x>0) ns1 += transitionTable[x - 1][y + 1][1];
					nextState[state * 4] = ns0;
					nextState[state * 4 + 1] = ns1;
					nextState[state * 4 + 2] = x;
					nextState[state * 4 + 3] = y;
				}
				else if (transitionTable[x][y][1]) {
					NextState(x0, y0, 0);
					NextState(x1, y1, 1);
					nextState[state * 4] = ns0 = transitionTable[x0][y0][0];
					nextState[state * 4 + 1] = ns1 = transitionTable[x1][y1][0] + (transitionTable[x1][y1][1]>1);
					nextState[state * 4 + 2] = x;
					nextState[state * 4 + 3] = y;
				}
				assert(state >= 0 && state < 256);
				assert(transitionTable[x][y][1] > 0);
				assert(transitionTable[x][y][0] <= state);
				assert(transitionTable[x][y][1] + transitionTable[x][y][0] > state);
				assert(transitionTable[x][y][1] <= 6);
				assert(transitionTable[x0][y0][1] > 0);
				assert(transitionTable[x1][y1][1] > 0);
				assert(ns0 - transitionTable[x0][y0][0] < transitionTable[x0][y0][1]);
				assert(ns0 - transitionTable[x0][y0][0] >= 0);
				assert(ns1 - transitionTable[x1][y1][0] < transitionTable[x1][y1][1]);
				assert(ns1 - transitionTable[x1][y1][0] >= 0);
				state++;
			}
		}
	}
}

void StateTable::InitializeTransitionTable()
{
	for (int i = 0; i < 65; i++)
	{
		for (int j = 0; j < 65; j++)
		{
			for (int k = 0; k < 2; k++)
				transitionTable[i][j][k] = 0;
		}
	}
	int state = 0;
	for (int i = 0; i < 256; i++)
	{
		for (int y = 0; y <= i; y++)
		{
			int x = i - y;
			int numStates = CountStates(x, y);
			if (numStates)
			{
				transitionTable[x][y][0] = state;
				transitionTable[x][y][1] = numStates;
				state += numStates;
			}
		}
	}
}

int StateTable::CountStates(int x, int y)
{
	/* We always assume x > y. */
	if (x < y)
		return CountStates(y, x);
	if (x < 0 || y < 0 || x >= 64 || y >= 64 || y >= 5 || x >= maxOtherCount[y])
		return 0;

	/* States 0-30 are a history of the last 0-4 bits. */
	/* stateNumber = (x+y)Cx = (x+y)!/x!y! = (x+y)(x+y-1)...(x+y-(y-1))/y! */
	if (x + y <= 4)
	{
		int r = 1;
		for (int i = x + 1; i <= x + y; i++)
			r *=  i;
		for (int i = 2; i <= y; i++)
			r /= i;
		return r;
	}
	/* States 31-255 represent a 0, 1 count and possibly the last bit */
	/* If the state is reachable by either a 0 or a 1.				  */
	else
	{
		return 1 + (y > 0 && x + y < 8);
	}
}

void StateTable::Discount(int& x)
{
	if (x > 2)
		x = ILog(x) >> 3; //  x = 2 log2(x);
}

void StateTable::NextState(int& x, int& y, int bit)
{
	/* We always assume x > y. */
	if (x < y)
	{
		NextState(y, x, 1 - bit);
	}
	else
	{
		if (bit)
		{
			y++;
			Discount(x);
		}
		else
		{
			x++;
			Discount(y);
		}
		/* If the state is unreachable... */
		if (!transitionTable[x][y][1])
		{
			/* x is just too large. */
			if (y == 0)
			{
				x--;
			}
			else if (y == 1)
			{
				y = 0;
				x /= 4;
			}
			else
			{
				/* To-do: understand why this rule works. */
				x = (x * (y - 1) + (y / 2)) / y;
				y--;
			}
		}
	}
}

StateTable::StateTable() : nextState(1024, 0)
{
	InitializeTransitionTable();
	InitializeNextStates();
}

int StateInfo(int state, int selection)
{
	static StateTable stateTable;
	return stateTable(state, selection);
}
