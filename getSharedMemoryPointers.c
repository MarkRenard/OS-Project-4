// getSharedMemoryPointers.c was created by Mark Renard on 3/27/2020.
//
// This file contains definitions for a shared memory function specific to
// assignment 4. This function is used by oss.c and userProcess.c.

#include "constants.h"
#include "clock.h"
#include "pcb.h"
#include "sharedMemory.h"

void getSharedMemoryPointers(char ** shm,  Clock ** systemClock,
			     ProcessControlBlock ** processTable, int flags) {
	// Computes size of the shared memory region
	int shmSize = sizeof(ProcessControlBlock) * MAX_SIMUL_USER_PROCS \
                      + sizeof(Clock);

	// Attaches to shared memory
        *shm = sharedMemory(shmSize, flags);

	// Gets pointer to simulated system clock
	*systemClock = (Clock *) *shm;

	// Gets pointer to first process control block
	*processTable = (ProcessControlBlock *) (*shm + sizeof(Clock));

}

