// pcb.c was created by Mark Renard on 3/28/2020.
//
// This file contains an implementation of a function that returns a process
// control block in its initial state.

#include "pcb.h"

// Returns an initialized process control block
ProcessControlBlock initialProcessControlBlock(int simPid, Clock currentTime,
					       SchedulingClass schedulingClass){
	ProcessControlBlock pcb;

	// Sets logical pid, timeCreated, and schedulingClass to argument values
	pcb.simPid = simPid;
	pcb.timeCreated = currentTime;
	pcb.schedulingClass = schedulingClass;
	
	// Initializes priority to 1 for normal, 0 for real-time
	pcb.priority = schedulingClass == NORMAL ? 1 : 0;

	// Clocks initialized to zero
	pcb.timeOfLastBurst = zeroClock();
	pcb.timeUsedDurringLastBurst = zeroClock();
	pcb.totalCpuTime = zeroClock();
	pcb.nextIoEventTime = zeroClock();

	pcb.state = NEW;

	pcb.previous = NULL;	// No previous block, not in queue yet

	return pcb;
}


Clock pcbTimeLastExecuting(const ProcessControlBlock * pcb){
	return clockSum(pcb->timeOfLastBurst, pcb->timeUsedDurringLastBurst);
}


Clock pcbTimeInSystem(const ProcessControlBlock * pcb, Clock currentTime){
	return clockDiff(currentTime, pcb->timeCreated);
}


double pcbCpuUtilization(const ProcessControlBlock * pcb, Clock currentTime){
	return clockRatio(pcb->totalCpuTime, pcbTimeInSystem(pcb, currentTime));
}
