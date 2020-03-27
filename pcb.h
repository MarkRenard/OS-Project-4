// pcb.h was created by Mark Renard on 3/26/2020.
//
// This file defines the ProcessControlBlock data type.

#ifndef PCB_H
#define PCB_H

#include "clock.h"

typedef enum ProcessState {NEW, READY, RUNNING, BLOCKED, EXIT} ProcessState;
typedef enum SchedulingClass {NORMAL, REAL_TIME} SchedulingClass;

typedef struct processControlBlock {
	int pid;			 // Simulated process identifier
	ProcessState state;		 // The simulated state of the process
	SchedulingClass schedulingClass; // Whether process is real time

	Clock totalCpuTime;		 // Total simulated execution time
	Clock timeCreated;		 // Used to compute total time in system
	Clock timeUsedDurringLastBurst;	 // Time passed durring last execution
	Clock timeOfLastBurst;		 // Time the last cpu burst started

	// Link to the previous process control block in its queue
	struct processControlBlock * previous;
} ProcessControlBlock;

#endif
