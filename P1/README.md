This was my first project using assembly.  In it we deconstruct a C++ file in to c then convert it into handwritten assembly code.  Here is the original C++ code:
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
	while (*argv) {
		cout << *(argv++) << endl;
	}
	return 0;
}
