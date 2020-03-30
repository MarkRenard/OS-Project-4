// oss.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the scheduling of an operating
// system using a multi-level feedback queue.

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "bitVector.h"
#include "constants.h"
#include "clock.h"
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
static void processMessage(const char *, ProcessControlBlock *, Queue *);
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

static char * shm = NULL;	// Pointer to the shared memory region
static int msgQueueId;	// ID of message queue for process dispatching

int main(int argc, char * argv[]){
	ProcessControlBlock * processTable;	// Shared memory process table
	Clock * systemClock;			// Shared memory system clock

	alarm(MAX_SECONDS);		// Limits total execution time
	exeName = argv[0];		// Assigns exeName for perrorExit
	assignSignalHandlers();		// Sets response to ctrl + C & alarm
	initializeBitVector();		// Sets bit vector values to 0

	msgQueueId = getMessageQueue(MQ_KEY, MQ_PERMS | IPC_CREAT);

	getSharedMemoryPointers(&shm, &systemClock, &processTable, IPC_CREAT);

	launchUserProcesses(systemClock, processTable);

	cleanUp();

	return 0;
}

// Schedules and launches user processes
static void launchUserProcesses(Clock * systemClock,
				ProcessControlBlock * processTable) {
	int totalGenerated = 0;	   // Total processes generated
	char msgText[MSG_SZ];	   // Message text recieved from last process
	ProcessControlBlock * pcb; // PCB of last dispatched process
	Clock timeToGenerate;	   // Random time to generate the next process
	Queue q;		   // Queue of process control blocks

	initializeQueue(&q);

	// Initializes system clock
	*systemClock = zeroClock();

	// Sets random time in the future to launch a process
	timeToGenerate = randomTime(minTimeBetweenNewProcs, 
				    maxTimeBetweenNewProcs);

#ifdef DEBUG
	int i = 0;
#endif
	// Generates and schedules user processes in a loop
	do {
		// Generates process if time reached and within process limits
		if (clockCompare(*systemClock, timeToGenerate) >= 0
		    && q.count < MAX_BLOCKS
		    && totalGenerated < MAX_TOTAL_GENERATED){

			// Generates new process, updates counter
			generateProcess(*systemClock, processTable, &q);	
			totalGenerated++;
#ifdef DEBUG
			fprintf(stderr, "\tTotal generated: %d\n", totalGenerated);
#endif
			// Sets new random time to launch a new process
			incrementClock(&timeToGenerate, 
				       randomTime(minTimeBetweenNewProcs,
						  maxTimeBetweenNewProcs));
#ifdef DEBUG
			fprintf(stderr, "\ttimeToGenerate: ");
			printTimeln(stderr, timeToGenerate);
#endif
		}

		// Schedules/dispatches a process from queue, if non-empty
		if (q.count > 0){

			// Adds simulated time taken by scheduling
			incrementClock(systemClock, 
				       randomTime(MIN_SCHEDULING_TIME,
						  MAX_SCHEDULING_TIME));
			
			// Dispatches a process from the queue
			pcb = dispatchProcess(*systemClock, &q);

#ifdef DEBUG
			fprintf(stderr, "\tTime after scheduling: ");
			printTimeln(stderr, *systemClock);
#endif

			// Waits for message from dispatched process
			waitForMessage(msgText, msgQueueId);
		
			// Re-queues process or logs termination
			processMessage(msgText, pcb, &q);
		}

		// Increments system clock
		Clock rand = randomTime(MIN_LOOP_INCREMENT,
				        MAX_LOOP_INCREMENT);
		incrementClock(systemClock, rand);

#ifdef DEBUG		
		fprintf(stderr, "Time after iteration %d: ", i++);
		printTimeln(stderr, *systemClock);
		fprintf(stderr, "\n\n");
		sleep(1);
#endif
	// Continues until max user processes generated and queue is empty
	} while ( (totalGenerated < MAX_TOTAL_GENERATED || q.count > 0) );
}

static void generateProcess(Clock time, ProcessControlBlock * processTable, 
			    Queue * queue){
#ifdef DEBUG
	static int num = 1;
	printf("generateProcess call number %d - q size: %d\n", num++, \
		queue->count);
	fflush(stdout);
#endif
	int newPid = getIntFromBitVector();
 
	processTable[newPid] = initialProcessControlBlock(newPid, time);

	enqueue(&processTable[newPid], queue);

	launchProcess(newPid);	

	processTable[newPid].state = READY;	

}

static void launchProcess(int simPid){
	int realPid;

	sleep(1);

	if ((realPid = fork()) == -1){
		perrorExit("Failed to fork");
	}

	if (realPid == 0){
		char sPid[BUFF_SZ];
		sprintf(sPid, "%d", simPid);

		execl(USER_PROG_PATH, USER_PROG_PATH, sPid, NULL);
		perrorExit("Failed to exec user program");
	}
}

static ProcessControlBlock * dispatchProcess(Clock time, Queue * q){
#ifdef DEBUG
	static int num = 1;
	printf("dispatchProcess call number %d - q size: %d\n", num++, q->count);
	fflush(stdout);
#endif
	ProcessControlBlock * pcb;
	char msgText[MSG_SZ];

	// Selects and runs process control block from the queue
	pcb = dequeue(q);		 // Gets PCB from queue
	pcb->state = RUNNING;		 // Changes state to running

	// Messages running process with time quantum
	sprintf(msgText, "%d", BASE_QUANTUM >> pcb->priority);
	sendMessage(msgText, msgQueueId);			

	return pcb;
}	

static void processMessage(const char * msg, ProcessControlBlock * pcb, 
			   Queue * q){
#ifdef DEBUG
	static int num = 0;
	printf("processMessage call number %d - q size: %d\n", num++, q->count);
	printf("\tMessage: %s\n", msg);
	fflush(stdout);
#endif
	unsigned int quantum;		// Nanoseconds allotted to process
	unsigned int usedNanoseconds;	// Nanoseconds used by the process

	// Computes the number of nanoseconds allotted in last burst
	quantum = BASE_QUANTUM >> pcb->priority;

	// Converts the message from the process to an integer
	usedNanoseconds = atoi(msg);

	if (usedNanoseconds == quantum){
		pcb->state = READY;
		enqueue(pcb, q);
	} else {
		pcb->state = EXIT;
		freeInBitVector(pcb->simPid);
#ifdef DEBUG
		fprintf(stderr, "PROCESS %d IN EXIT STATE!!!\n", pcb->simPid);
		fprintf(stderr, "\tusedNanoseconds: %d\n", usedNanoseconds);
		fprintf(stderr, "\tquantum: %d\n", quantum);
#endif
	}
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
	removeMessageQueue(msgQueueId);      
	
	// Detatches from and removes shared memory
        detach(shm);
        removeSegment();
}

