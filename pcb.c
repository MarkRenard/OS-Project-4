// pcb.c was created by Mark Renard on 3/28/2020.
//
// This file contains an implementation of a function that returns a process
// control block in its initial state.

#include "pcb.h"

// Returns an initialized process control block
ProcessControlBlock initialProcessControlBlock(){
	ProcessControlBlock pcb;

	// Initializes logical pid, process state, and scheduling class
	pcb.pid = -1;
	pcb.state = NEW;
	pcb.schedulingClass = NORMAL;

	// Initializes times
	pcb.totalCpuTime = zeroClock();
	pcb.timeCreated = zeroClock();
	pcb.timeUsedDurringLastBurst = zeroClock();
	pcb.timeOfLastBurst = zeroClock();

	pcb.previous = NULL;	// No previous block, not in queue yet

	return pcb;
}
