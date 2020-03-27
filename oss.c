// oss.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the scheduling of an operating
// system.


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "constants.h"
#include "clock.h"
#include "pcb.h"
#include "perrorExit.h"
#include "queue.h"
#include "randomGen.h"
#include "sharedMemory.h"

/* Prototypes */
static void assignSignalHandlers();
static void cleanUpAndExit(int param);
static void cleanUp();
//static pid_t createChild(int logicalPid, int shmSize);

/* Static Global Variables */
static char * shm = NULL;               // Pointer to the shared memory region
static FILE * logFile = NULL;		// Log of execution times

int main(int argc, char * argv[]){
	alarm(MAX_REAL_LIFE_SECONDS);	// Limits total execution time 
	exeName = argv[0];		// Assigns exe name for perrorExit
	assignSignalHandlers();		// Sets response to ctrl + C & alarm

	sleep(2);

	return 0;
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

        // Closes files
        if (logFile != NULL) fclose(logFile);

        // Detatches from and removes shared memory
        detach(shm);
        removeSegment();
}

