#include <iostream>
#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include "lzw_wizard.h"

using namespace std;

int main() {
	
	LzwWizard wiz;
	wiz.encodeFile("test.bmp", "test.bin");

	return 0;
}