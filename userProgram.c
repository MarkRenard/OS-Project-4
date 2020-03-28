// userProgram.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the activity of a process
// being managed by oss.

#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]){
	printf("%s executing...\n", argv[1]);

	sleep(1);
	
	return 0;
}
