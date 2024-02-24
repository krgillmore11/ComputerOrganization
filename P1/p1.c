//P1 - Kevin Gillmore
//Computer Orginization 2122
#include <stdio.h> 											//for printf function

int MyStrlen(char * s) {
	int retval = 0;
	top:
		if (*(s++) == 0)									//while loops are not c! use labels
			goto bottom;
		retval++;
		goto top;

	bottom:
		return retval;										//returns number of chars in argv as int
}

int main(int argc, char * argv[]) { 						//argv the address of a variable holding the address of a string
	top:
		if (*argv == NULL) 									//remove brackets by reversing if statement and using labels
			goto bottom; 									//stops printing and returns
		printf("[%d] [%s]\n", MyStrlen(*argv), *(argv++));	//printf acts as cout
		goto top;
	
	bottom:
		return 0;
}