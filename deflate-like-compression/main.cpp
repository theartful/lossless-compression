#include <iostream>
#include "buffer.h"
#include "read_buffer.h"
#include "write_buffer.h"
#include "lzw_wizard.h"

using namespace std;

int main() {
    cout <<  char_traits<char>::eof()<< endl;
    LzwWizard wiz;
    cout << "Encoding: " << endl;
	wiz.encodeFile("./test-files/test.bmp", "./test-files/test.bin");

	cout << "Decoding: " << endl;
	wiz.decodeFile("./test-files/test.bin", "./test-files/test-out.bmp");
	return 0;
}
