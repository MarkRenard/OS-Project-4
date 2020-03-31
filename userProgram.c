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

	int dispatchMqId;	// Message queue ID for dispatch messages
	int interruptMqId;	// Message queue ID for reporting interrupts
	char msgBuff[MSG_SZ];	// Buffer for sending and receiving messages

	int simPid = atoi(argv[1]); // Gets simulated pid of the process

	// Attatches to shared memory and gets pointers
	getSharedMemoryPointers(&shm, &systemClock, &processTable, 0);

	// Used to reseed prng
	srand(BASE_SEED + simPid + (*systemClock).nanoseconds);

	// Gets message queues
	dispatchMqId = getMessageQueue(DISPATCH_MQ_KEY, MQ_PERMS);
	interruptMqId = getMessageQueue(INTERRUPT_MQ_KEY, MQ_PERMS);
	
	int finished = 0; // Nonzero when finished
	while (!finished){
	
		// Keeps checking shared memory location if it's been scheduled
		while (processTable[simPid].state != RUNNING);

		// Waits for time quantum from oss
		waitForMessage(dispatchMqId, msgBuff, simPid + 1);

		// Determines whether process will terminate
		if (randBinary(TERMINATION_PROBABILITY)){
			finished = 1;
			determineTimeUsed(msgBuff);
		}	

		// Sends message to oss, unmodified if entire quantum used
		sendMessage(interruptMqId, msgBuff, simPid + 1);

		// Waits for state change
		// while (processTable[simPid].state == RUNNING); 
	}

	return 0;
}

static void determineTimeUsed(char * msgText){
	unsigned int timeUsed;
	unsigned int quantum = atoi(msgText);

	timeUsed = randUnsigned(0, quantum);

	sprintf(msgText, "%d", timeUsed);
}
