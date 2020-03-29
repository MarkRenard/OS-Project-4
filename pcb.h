// pcb.h was created by Mark Renard on 3/26/2020.
//
// This file contains a definition of the ProcessControlBlock data type and
// the function header for a function that returns a ProcessControlBlock in its
// default initial state.

#ifndef PCB_H
#define PCB_H

#include "clock.h"

typedef enum ProcessState {NEW, READY, RUNNING, BLOCKED, EXIT} ProcessState;
typedef enum SchedulingClass {NORMAL, REAL_TIME} SchedulingClass;

typedef struct processControlBlock {
	int simPid;			 // Simulated process identifier
	int priority;			 // Priority level of the process
	ProcessState state;		 // The simulated state of the process
	SchedulingClass schedulingClass; // Whether process is real time
	
	Clock totalCpuTime;		 // Total simulated execution time
	Clock timeCreated;		 // Used to compute total time in system
	Clock timeUsedDurringLastBurst;	 // Time passed durring last execution
	Clock timeOfLastBurst;		 // Time the last cpu burst started

	// Link to the previous process control block in its queue
	struct processControlBlock * previous;
} ProcessControlBlock;

ProcessControlBlock initialProcessControlBlock(int, Clock);

#endif
