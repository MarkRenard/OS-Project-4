// logging.c was created by Mark Renard on 3/29/2020.
//
// This file contains definitions for functions that aid in the collection,
// formatting, and logging of data pertinent to Assignment 4.

#include "clock.h"
#include "constants.h"
#include "perrorExit.h"
#include <stdio.h>

static FILE * log = NULL;
static int lines = 0;

// Opens the log file if closed, prints the line, and increments lines
static void printLine(char * line){
	if (++lines > MAX_LOG_LINES) return;

	// Opens the log file if it is not already open
	if (log == NULL)
		if ((log = fopen(LOG_FILE_NAME, "w+")) == NULL)
			perrorExit("logging.c - failed to open log file");

	fprintf(log, "%s", line); 
	//fflush(log);
}

// Logs the generation and enqueueing of a new process
void logGeneration(int simPid, int queueNum, Clock time){
	char buff[BUFF_SZ];
	lines++;	// Exta new line character

	// Prints message
	sprintf(buff, "\nOSS: Generating process with PID %d and putting it in" \
		" queue %d at time %03u : %09u\n", simPid, queueNum, 
		time.seconds, time.nanoseconds); 
	printLine(buff);
}

// Logs the dispatching of a process 
void logDispatch(int simPid, int queueNum, Clock time){
	char buff[BUFF_SZ];
	lines++;	// Extra new line character

	// Prints message
	sprintf(buff, "\nOSS: Dispatching process with PID %d from queue %d at" \
		" time %03u : %09u\n", simPid, queueNum, time.seconds,
		time.nanoseconds);
	printLine(buff);
}

// Logs the simulated processor time taken to dispatch a process 
void logDispatchTime(unsigned int time){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: total time this dispatch was %u nanoseconds\n", time); 
	printLine(buff);
}

// Logs messages recieved from user processes
void logMessageReciept(int simPid, unsigned int burstLength){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: Receiving that process with PID %d ran for %u" \
	       " nanoseconds\n", simPid, burstLength);
	printLine(buff);
}

// Logs that the process that sent the message didn't use its entire timeslice
void logPartialQuantumUse(){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: not using its entire time quantum\n");
	printLine(buff);
}


// Logs the enqueueing of a process control block
void logEnqueue(int simPid, int queueNum){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: Putting process with PID %d into queue %d\n", 
		simPid, queueNum);
	printLine(buff);
}

// Logs that a process is blocked until an I/O event at a particular time
void logBlocking(int simPid, Clock nextIoEventTime){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: Process with PID %d is BLOCKED, waiting for I/O"
		" until time %d : %d\n", simPid, nextIoEventTime.seconds, 
		nextIoEventTime.nanoseconds);
	printLine(buff);
}

// Logs that a process has been preempted at a given time
void logPreemption(int simPid, int queueNum, Clock currentTime){
	char buff[BUFF_SZ];
	sprintf(buff, "OSS: Process with PID %d is PREEMPTED, added to the"
		" head of queue %d at time %03u : %09u\n", simPid, queueNum,
		currentTime.seconds, currentTime.nanoseconds);
	printLine(buff);
}

// Logs that a process has been moved from the blocked queue to a ready queue
void logWakeUp(int simPid, int queueNum, Clock time){
	char buff[BUFF_SZ];
	lines++; 	// Accounts for extra new line character
	
	sprintf(buff, "\nOSS: Process with PID %d has been moved from the"
		" blocked queue to queue %d at %03u : %09u\n", simPid,
		queueNum, time.seconds, time.nanoseconds);
	printLine(buff);

}
