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

static void terminateProcedure(char * msgText);
static void blockProcedure(char * msgText, int r, int s);
static void preemptProcedure(char * msgText);
static void useEntireQuantumProcedure(char * msgText);
static void createReplyMessage(char * msgText, char stateChar, int usedNano,
                               int r, int s);

int main(int argc, char * argv[]){
	char * shm;				// Pointer to shared memory
	Clock * systemClock;			// Shared memory process table
	ProcessControlBlock * processTable;	// Shared memory system clock

	int dispatchMqId;	// Message queue ID for dispatch messages
	int replyMqId;		// Message queue ID for replying to oss
	char msgBuff[MSG_SZ];	// Buffer for sending and receiving messages

	int simPid = atoi(argv[1]); // Gets simulated pid of the process

	// Attatches to shared memory and gets pointers
	getSharedMemoryPointers(&shm, &systemClock, &processTable, 0);

	// Seeds off a function of the process id
	srand(BASE_SEED + simPid + systemClock->nanoseconds);

	// Gets message queues
	dispatchMqId = getMessageQueue(DISPATCH_MQ_KEY, MQ_PERMS);
	replyMqId = getMessageQueue(REPLY_MQ_KEY, MQ_PERMS);

	int finished = 0; // Nonzero when finished
	while (!finished){
	
		// Keeps checking shared memory location if it's been scheduled
		while (processTable[simPid].state != RUNNING);

		// Waits on recieving a message giving it a timeslice
		waitForMessage(dispatchMqId, msgBuff, simPid + 1);

		// Decides if process terminates before using entire quantum
		if (randBinary(TERMINATION_PROBABILITY)){
			finished = 1;
			terminateProcedure(msgBuff);

		// Determines whether process will get blocked or preempted
		} else if (randBinary(BLOCK_OR_PREEMPT_PROBABILITY)){
			unsigned int r = randUnsigned(0, 3);
			unsigned int s = randUnsigned(0, 1000);

			if (r == 3){
				preemptProcedure(msgBuff);
			} else {
				blockProcedure(msgBuff, r, s);
			}

		// Indicates that the process will not terminate within quantum
		} else {
			// Creates message indicating non-termination
			useEntireQuantumProcedure(msgBuff);
		}

		// Indicates quantum use and whether terminating or blocking
		sendMessage(replyMqId, msgBuff, simPid + 1);
	}

	return 0;
}

// Changes msgText indicating partial quantum use before termination
static void terminateProcedure(char * msgText){
	unsigned int usedNano;	// Stores burst length in nanoseconds
	unsigned int quantum = atoi(msgText);

	// Generates random number in range [0, quantum] to see how long it runs
	usedNano = randUnsigned(0, quantum);

	createReplyMessage(msgText, TERMINATION_CH, usedNano, -1, -1);
}

// Changes msgText indicating use of entire quantum
static void useEntireQuantumProcedure(char * msgText){
	unsigned int quantum = atoi(msgText);

	createReplyMessage(msgText, USES_ALL_QUANTUM_CH, quantum, -1, -1);
}

// Changes msgText indicating the process has been prempted
static void preemptProcedure(char * msgText){
	unsigned int quantum = atoi(msgText);
	unsigned int usedNano;

	usedNano = quantum * randUnsigned(1, 99) / 100;

	createReplyMessage(msgText, PREEMPT_CH, usedNano, -1, -1);
}

// Changes msgText indicating the process is blocking, waiting for I/O
static void blockProcedure(char * msgText, int r, int s){
	unsigned int usedNano;
	unsigned int quantum = atoi(msgText);

	usedNano = randUnsigned(0, quantum);

	createReplyMessage(msgText, WAITING_FOR_IO_CH, usedNano, r, s);
}

// Changes msgText to use in the repy queue
static void createReplyMessage(char * msgText, char stateChar, int usedNano, 
			       int r, int s){
	int i = 0;
	
	// Adds stateChar
	msgText[i++] = stateChar;
	msgText[i++] = DELIM;

	// Adds time used
	char usedNanoBuff[BUFF_SZ];
	sprintf(usedNanoBuff, "%d", usedNano);

	do {
		msgText[i] = usedNanoBuff[i - 2];
	} while (msgText[i++] != '\0');
	msgText[i++] = DELIM;

	// Adds time of I/O event if blocking
	if (stateChar == WAITING_FOR_IO_CH){
		char rBuff[BUFF_SZ];
		char sBuff[BUFF_SZ];

		// Converts ints to strings
		sprintf(rBuff, "%d", r);
		sprintf(sBuff, "%d", s);

		// Copies r
		int j = 0;
		do {
			msgText[i] = rBuff[j++];
		} while (msgText[i++] != '\0');
		msgText[i++] = DELIM;

		// Copies s
		j = 0;
		do {
			msgText[i] = sBuff[j++];
		} while (msgText[i++] != '\0');
		msgText[i++] = DELIM;
	}
}
	
