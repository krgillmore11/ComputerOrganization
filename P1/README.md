This was my first project using assembly.  In it we deconstruct a C++ file in to c then convert it into handwritten assembly code.  Here is the original C++ code:

```
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
	while (*argv) {
		cout << *(argv++) << endl;
	}
	return 0;
}
```
[Here](https://github.com/pkivolowitz/CSC3510-S-2022/tree/main/projects/p1) is the full project specification.
