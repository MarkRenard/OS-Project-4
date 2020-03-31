// oss.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the scheduling of an operating
// system using a multi-level feedback queue.

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "bitVector.h"
#include "constants.h"
#include "clock.h"
#include "logging.h"
#include "message.h"
#include "pcb.h"
#include "perrorExit.h"
#include "queue.h"
#include "randomGen.h"
#include "sharedMemory.h"
#include "getSharedMemoryPointers.h"


	/* Prototypes */

static void launchUserProcesses(Clock *, ProcessControlBlock *);
static void generateProcess(Clock, ProcessControlBlock *, Queue *);
static void launchProcess(int);
static ProcessControlBlock * dispatchProcess(Clock, Queue *);
static unsigned int processMessage(const char*, ProcessControlBlock*, Queue*);
static void assignSignalHandlers();
static void cleanUpAndExit(int param);
static void cleanUp();


	/* Constants (computed from values in constants.h) */

// Min and max time between generating processes (0 and 2 seconds by default)
const static Clock minTimeBetweenNewProcs = {0, 0};
const static Clock maxTimeBetweenNewProcs = {maxTimeBetweenNewProcsSecs,
					     maxTimeBetweenNewProcsNS};

// Min and max system clock increment at each loop (default 1 and 1.000001 secs)
const static Clock MIN_LOOP_INCREMENT = {LOOP_INCREMENT_SECONDS,
					 MIN_LOOP_INCREMENT_NS};
const static Clock MAX_LOOP_INCREMENT = {LOOP_INCREMENT_SECONDS,
					 MAX_LOOP_INCREMENT_NS};

// Min and max simulated scheduling overhead (100ns and 1000ns by default)
const static Clock MIN_SCHEDULING_TIME = {0, MIN_SCHEDULING_TIME_NS};
const static Clock MAX_SCHEDULING_TIME = {0, MAX_SCHEDULING_TIME_NS};


	/* Static Global Variables */

static char * shm = NULL; // Pointer to the shared memory region
static int dispatchMqId;  // ID of message queue for process dispatching
static int interruptMqId; // ID of message queue for recieiving interrupt info

int main(int argc, char * argv[]){
	ProcessControlBlock * processTable;	// Shared memory process table
	Clock * systemClock;			// Shared memory system clock

	alarm(MAX_SECONDS);	// Limits total execution time
	exeName = argv[0];	// Assigns exeName for perrorExit
	assignSignalHandlers();	// Sets response to ctrl + C & alarm
	initializeBitVector();	// Sets bit vector values to 0

	srand(BASE_SEED - 1);	// Seeds pseudorandom number generator

	// Creates message queues
	dispatchMqId = getMessageQueue(DISPATCH_MQ_KEY, MQ_PERMS | IPC_CREAT);
	interruptMqId = getMessageQueue(INTERRUPT_MQ_KEY, MQ_PERMS | IPC_CREAT);

	// Creates shared memory region and gets pointers
	getSharedMemoryPointers(&shm, &systemClock, &processTable, IPC_CREAT);

	// Generates, enqueues, and dispatches user processes in a loop
	launchUserProcesses(systemClock, processTable);

	cleanUp();

	return 0;
}

// Schedules and launches user processes
static void launchUserProcesses(Clock * systemClock,
				ProcessControlBlock * processTable) {
	char msgText[MSG_SZ];	   // Message text from last dispatched process
	unsigned int execTime;	   // Nanoseconds used by last dispatched process
	ProcessControlBlock * pcb; // PCB of last dispatched process
	
	int totalGenerated = 0;	   // Total processes generated
	Clock timeToGenerate;	   // Random time to generate the next process
	Queue q;		   // Queue of process control blocks

	initializeQueue(&q);

	// Initializes system clock
	*systemClock = zeroClock();

	// Sets random time in the future to launch a process
	timeToGenerate = randomTime(minTimeBetweenNewProcs, 
				    maxTimeBetweenNewProcs);

	// Generates and schedules user processes in a loop
	do {
		// Generates process if time reached and within process limits
		if (clockCompare(*systemClock, timeToGenerate) >= 0
		    && q.count < MAX_BLOCKS
		    && totalGenerated < MAX_TOTAL_GENERATED){

			// Generates new process, updates counter
			generateProcess(*systemClock, processTable, &q);	
			totalGenerated++;

			// Sets new random time to launch a new process
			incrementClock(&timeToGenerate, 
				       randomTime(minTimeBetweenNewProcs,
						  maxTimeBetweenNewProcs));
		}

		// Schedules/dispatches a process from queue, if non-empty
		if (q.count > 0){

			// Adds simulated time taken by scheduling
			incrementClock(systemClock, 
				       randomTime(MIN_SCHEDULING_TIME,
						  MAX_SCHEDULING_TIME));
			
			// Dispatches a process from the queue
			pcb = dispatchProcess(*systemClock, &q);

			// Waits for message from dispatched process
			waitForMessage(interruptMqId, msgText, pcb->simPid + 1);
		
			// Records time & re-queues process or logs termination
			execTime = processMessage(msgText, pcb, &q);

			// Adds execution time to the system clock
			incrementClock(systemClock, newClock(0, execTime));
		}

		// Increments system clock
		Clock rand = randomTime(MIN_LOOP_INCREMENT,
				        MAX_LOOP_INCREMENT);
		incrementClock(systemClock, rand);

	// Continues until max user processes generated and queue is empty
	} while ( (totalGenerated < MAX_TOTAL_GENERATED || q.count > 0) );
}

// Creates a process control block and launches a corresponding process
static void generateProcess(Clock time, ProcessControlBlock * processTable, 
			    Queue * queue){
	int newPid;	// The simulated pid of the new process

	// Gets an available simulated pid from the int vector
	newPid = getIntFromBitVector();
	if (newPid == -1)
		perrorExit("generateProcess called with no available PCBs");
 
	// Initializes the process control block for the new process
	processTable[newPid] = initialProcessControlBlock(newPid, time);

	// Logs process generation
	logGeneration(newPid, processTable[newPid].priority, time);

	// Adds the new process control block to the queue
	enqueue(&processTable[newPid], queue);

#ifdef DEBUG
	fprintf(stderr, "About to launch process %d\n", newPid);
	sleep(1);
#endif
	// Forks and execs new child process
	launchProcess(newPid);	

	// Changes process state to ready in new process control block
	processTable[newPid].state = READY;

}

// Forks and execs a new child process
static void launchProcess(int simPid){
	int realPid;

	// Forks or exits on failure using perror
	if ((realPid = fork()) == -1){
		perrorExit("Failed to fork");
	}

	// Child execs the user process
	if (realPid == 0){

		// Converts simPid to string
		char sPid[BUFF_SZ];
		sprintf(sPid, "%d", simPid);

		// Execs binary
		execl(USER_PROG_PATH, USER_PROG_PATH, sPid, NULL);
		perrorExit("Failed to exec user program");
	}
}

// Dequeues a PCB, changes state to running, and messages process with quantum
static ProcessControlBlock * dispatchProcess(Clock time, Queue * q){
	ProcessControlBlock * pcb; // PCB of dispatched process
	char msgText[MSG_SZ];      // Buffer of message to add to message queue

	// Logs dispatch and time
	logDispatch(q->front->simPid, q->front->priority, time); 

	// Selects and runs a process
	pcb = dequeue(q);		// Gets PCB from queue
	pcb->state = RUNNING;		// Changes state to running

	// Messages running process with time quantum
	sprintf(msgText, "%d", BASE_QUANTUM >> pcb->priority);
	sendMessage(dispatchMqId, msgText, pcb->simPid + 1);

	// Returns process control block of dispatched process
	return pcb;
}	

// Records message from user process, re-enqueues or removes pcb
static unsigned int processMessage(const char * msg, ProcessControlBlock * pcb,
				   Queue * q){
	unsigned int quantum;		// Nanoseconds allotted to process
	unsigned int usedNanoseconds;	// Nanoseconds used by the process

	// Computes the number of nanoseconds allotted in last burst
	quantum = BASE_QUANTUM >> pcb->priority;

	// Converts the message from the process to an integer
	usedNanoseconds = atoi(msg);

	// Writes a line to the log indicating pid and burst time
	logMessageReciept(pcb->simPid, usedNanoseconds);

	// Re-enqueues pcb if entire quantum was used
	if (usedNanoseconds == quantum){
		pcb->state = READY;

		// Logs the simPid and queue number of re-enqueued pcb
		logEnqueue(pcb->simPid, pcb->priority);

		enqueue(pcb, q);

	// If process terminted, changes state to exit, waits, and frees simPid
	} else {
		pcb->state = EXIT;
		wait(NULL);
		freeInBitVector(pcb->simPid);

		// Writes a line to the log indicating termination
		logPartialQuantumUse();
	}

	return usedNanoseconds;
}

// Determines the processes response to ctrl + c or alarm
static void assignSignalHandlers(){
        struct sigaction sigact;

        // Initializes sigaction values
        sigact.sa_handler = cleanUpAndExit;
        sigact.sa_flags = 0;

        // Assigns signals to sigact
        if ((sigemptyset(&sigact.sa_mask) == -1)
            ||(sigaction(SIGALRM, &sigact, NULL) == -1)
            ||(sigaction(SIGINT, &sigact, NULL)  == -1))
                perrorExit("Faild to install signal handler");
}

// Signal handler - closes files, removes shm, terminates children, and exits
static void cleanUpAndExit(int param){

        // Closes files, removes shm, terminates children
        cleanUp();

        // Prints error message
        char buff[BUFF_SZ];
        sprintf(buff,
                 "%s: Error: Terminating after receiving a signal",
                 exeName
        );
        perror(buff);

        // Exits
        exit(1);
}

// Ignores interrupts, kills child processes, closes files, removes shared mem
static void cleanUp(){
        // Handles multiple interrupts by ignoring until exit
        signal(SIGALRM, SIG_IGN);
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);

        // Kills all other processes in the same process group
        kill(0, SIGQUIT);

	// Removes message queue
	removeMessageQueue(dispatchMqId);      
	removeMessageQueue(interruptMqId);      
	
	// Detatches from and removes shared memory
        detach(shm);
        removeSegment();
}

