// userProgram.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the activity of a process
// being managed by oss.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "clock.h"
#include "getSharedMemoryPointers.h"
#include "pcb.h"
#include "perrorExit.h"

int main(int argc, char * argv[]){
	char * shm;				// Pointer to shared memory
	Clock * systemClock;			// Shared memory process table
	ProcessControlBlock * processTable;	// Shared memory system clock
	int simPid;				// Simulated pid of process

	exeName = argv[0];	// Assigns executable name for perrorExit

	// Gets logical pid of process
	if (argc < 2) perrorExit("Must pass logical pid of process in argv");
	simPid = atoi(argv[1]);

	getSharedMemoryPointers(&shm, &systemClock, &processTable, 0);

	// Keeps checking shared memory location if it has been scheduled
	while (processTable[simPid].state != RUNNING);

	fprintf(stderr, "\nPROCESS %d RUNNING!\n\n\n", simPid);
	
	return 0;
}
