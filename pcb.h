// pcb.h was created by Mark Renard on 3/26/2020.
//
// This file contains a definition of the ProcessControlBlock data type and
// the function header for a function that returns a ProcessControlBlock in its
// default initial state.

#ifndef PCB_H
#define PCB_H

#include "clock.h"

typedef enum ProcessState {NEW, READY, RUNNING, BLOCKED, PREEMPTED, EXIT} ProcessState;
typedef enum SchedulingClass {NORMAL, REAL_TIME} SchedulingClass;

typedef struct processControlBlock {

	// Determined at process generation time
	int simPid;			 // Simulated process identifier
	Clock timeCreated;		 // Used to compute total time in system
	SchedulingClass schedulingClass; // Whether process is real time

	// Updated by multi-level feedback queue
	int priority;			 // Priority level of the process

	// Updated at dispatch time
	Clock timeOfLastBurst;		 // Time the last cpu burst started

	// Updated at end of burst
	Clock timeUsedDurringLastBurst;	 // Time passed durring last execution
	Clock totalCpuTime;		 // Total simulated execution time
	Clock nextIoEventTime;		 // Time of next I/O event

	// Updated at dispatch and end of burst
	ProcessState state;		 // The simulated state of the process

	// Link to the previous process control block in its queue
	struct processControlBlock * previous;

} ProcessControlBlock;

ProcessControlBlock initialProcessControlBlock(int, Clock, SchedulingClass);
Clock pcbTimeLastExecuting(const ProcessControlBlock *);
Clock pcbTimeInSystem(const ProcessControlBlock *, Clock currentTime);
double pcbCpuUtilization(const ProcessControlBlock *, Clock currentTime);

#endif
