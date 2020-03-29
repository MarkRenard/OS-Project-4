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
#include "message.h"
#include "pcb.h"
#include "perrorExit.h"
#include "randomGen.h"

static void determineTimeUsed(char * msgText);

int main(int argc, char * argv[]){
	char * shm;				// Pointer to shared memory
	Clock * systemClock;			// Shared memory process table
	ProcessControlBlock * processTable;	// Shared memory system clock
	int simPid;				// Simulated pid of process
	int messageQueueId;			// Used to access queue
	char msgText[MSG_SZ];			// Queue message text
	int finished = 0;			// Nonzero when finished

	exeName = argv[0];	// Assigns executable name for perrorExit

	// Gets logical pid of process
	if (argc < 2) perrorExit("Must pass logical pid of process in argv");
	simPid = atoi(argv[1]);

	// Attatches to shared memory and gets pointers
	getSharedMemoryPointers(&shm, &systemClock, &processTable, 0);

	// Gets message queue
	messageQueueId = getMessageQueue(MQ_KEY, MQ_PERMS);

	while (!finished){

		// Keeps checking shared memory location if it's been scheduled
		while (processTable[simPid].state != RUNNING);

		// Waits for time quantum from oss
		waitForMessage(msgText, messageQueueId);

#ifdef DEBUG
		fprintf(stderr, "\t PROCESS %d RUNNING!\n", simPid);
		fprintf(stderr, "\t\t MESSAGE: %s\n", msgText);
		//sleep(2);
#endif

		// Determines whether process will terminate
		if (randBinary(TERMINATION_PROBABILITY)){
			finished = 1;
			determineTimeUsed(msgText);
		}	

	
		// Sends message to oss
		sendMessage(msgText, messageQueueId);

		// Waits for state change
		while (processTable[simPid].state == RUNNING); 
	}

#ifdef DEBUG
	fprintf(stderr, "\tPROCESS %d RUNNING!\n", simPid);
#endif	
	return 0;
}

static void determineTimeUsed(char * msgText){
	unsigned int timeUsed;
	unsigned int quantum = atoi(msgText);

	timeUsed = randUnsigned(0, quantum);

	sprintf(msgText, "%d", timeUsed);
}
