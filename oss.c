// oss.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the scheduling of an operating
// system using a multi-level feedback queue.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


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
static void initializeBitVector(unsigned int *);
static void generateProcess(ProcessControlBlock*, unsigned int*, Queue*, FILE*);
static void dispatchProcess(Queue *, FILE *);
static Message waitForMessage();
static void processMessage(Message, Queue *, FILE *);
static void assignSignalHandlers();
static void cleanUpAndExit(int param);
static void cleanUp();
//static pid_t launchProcess(int logicalPid);

/* Named constants (computed from values in constants.h) */
const static Clock minTimeBetweenNewProcs = {0, 0};
const static Clock maxTimeBetweenNewProcs = {maxTimeBetweenNewProcsSecs,
					     maxTimeBetweenNewProcsNS};

const static Clock MIN_LOOP_INCREMENT = {LOOP_INCREMENT_SECONDS,
					 MIN_LOOP_INCREMENT_NS};
const static Clock MAX_LOOP_INCREMENT = {LOOP_INCREMENT_SECONDS,
					 MAX_LOOP_INCREMENT_NS};

const static Clock MIN_SCHEDULING_TIME = {0, MIN_SCHEDULING_TIME_NS};
const static Clock MAX_SCHEDULING_TIME = {0, MAX_SCHEDULING_TIME_NS};

/* Static Global Variables */
static char * shm = NULL;               // Pointer to the shared memory region

int main(int argc, char * argv[]){
	ProcessControlBlock * processTable;	// Shared memory process table
	Clock * systemClock;			// Shared memory system clock

	alarm(MAX_SECONDS);		// Limits total execution time
	exeName = argv[0];		// Assigns exeName for perrorExit
	assignSignalHandlers();		// Sets response to ctrl + C & alarm

	getSharedMemoryPointers(&shm, &systemClock, &processTable, IPC_CREAT);

	launchUserProcesses(systemClock, processTable);

	cleanUp();

	return 0;
}

// Schedules and launches user processes
static void launchUserProcesses(Clock * systemClock,
				ProcessControlBlock * processTable) {
	int totalGenerated = 0;	 // Total processes generated
	Clock timeToGenerate;	 // Random time to generate the next process
	Message msg;		 // Message received from the message queue
	FILE * log;		 // Log of oss activity	
	Queue q;		 // Queue of process control blocks

	q.count = 0;

	// Initializes vector used to track 
	unsigned int bitVector[BIT_VECTOR_SIZE];
	initializeBitVector(bitVector);

	// Opens log file
	if ((log = fopen(LOG_FILE_NAME, "w+")) == NULL)
		perrorExit("Couldn't open log file");

	// Initializes system clock
	*systemClock = zeroClock();

	// Sets random time in the future to launch a process
	timeToGenerate = randomTime(minTimeBetweenNewProcs, 
				    maxTimeBetweenNewProcs);

	int i = 0;
	// Generates and schedules user processes in a loop
	do {
		// Generates new process if time reached and block is available
		if (clockCompare(*systemClock, timeToGenerate) >= 0
		    && q.count < MAX_SIMUL_USER_PROCS
		    && totalGenerated < MAX_TOTAL_USER_PROCS){

			// Generates new process, logs action, updates counter
			generateProcess(processTable, bitVector, &q, log);	
			totalGenerated++;
			fprintf(stderr, "\tTotal generated: %d\n", totalGenerated);

			// Sets new random time to launch a new process
			incrementClock(&timeToGenerate, 
				       randomTime(minTimeBetweenNewProcs,
						  maxTimeBetweenNewProcs));
			fprintf(stderr, "\ttimeToGenerate: ");
			printTimeln(stderr, timeToGenerate);
		}

		// Schedules/dispatches a process from queue, if non-empty
		if (q.count > 0){

			// Adds simulated time taken by scheduling
			incrementClock(systemClock, 
				       randomTime(MIN_SCHEDULING_TIME,
						  MAX_SCHEDULING_TIME));
			
			// Dispatches a process from the queue
			dispatchProcess(&q, log);

			fprintf(stderr, "\tTime after scheduling: ");
			printTimeln(stderr, *systemClock);

			// Waits for message from dispatched process
			msg = waitForMessage();
		
			// Re-queues process or logs termination
			processMessage(msg, &q, log);
		}

		// Increments system clock
		Clock rand = randomTime(MIN_LOOP_INCREMENT,
				        MAX_LOOP_INCREMENT);
		incrementClock(systemClock, rand);
		
		fprintf(stderr, "Time after iteration %d: ", i++);
		printTimeln(stderr, *systemClock);
		fprintf(stderr, "\n\n");
		
	// Continues until max user processes generated and queue is empty
	} while ((totalGenerated < MAX_TOTAL_USER_PROCS || q.count > 0) && i < 150);

}

static void initializeBitVector(unsigned int * bitVector){
	int i;
	for (i = 0; i < BIT_VECTOR_SIZE; i++){
		bitVector[i] = 0;
	}
}


static void generateProcess(ProcessControlBlock * processTable, 
			    unsigned int * bitVector, Queue * q, FILE * log){
	static int num = 1;
	printf("generateProcess call number %d - q size: %d\n", num++, q->count++);
	fflush(stdout);

}

static void dispatchProcess(Queue * q, FILE * log){
	static int num = 1;
	printf("dispatchProcess call number %d - q size: %d\n", num++, q->count--);
	fflush(stdout);
}	

static Message waitForMessage(){
	Message msg;

	msg.type = 0;
	strcpy(msg.string, "stuff");

	return msg;
}

static void processMessage(Message msg, Queue * q, FILE * log){
	static int num = 0;
	printf("processMessage call number %d - q size: %d\n", num++, q->count);
	fflush(stdout);
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

        // Detatches from and removes shared memory
        detach(shm);
        removeSegment();
}

