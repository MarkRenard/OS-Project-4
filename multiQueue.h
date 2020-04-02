// multiQueue.h was created by Mark Renard on 3/30/2020.
//
// This file contains the MultiQueue type definition and  function headers
// for functions that enable enqueueing and dequeueing process control blocks
// in a multi-level feedback queue.

#ifndef MULTIQUEUE_H
#define MULTIQUEUE_H

#include "constants.h"
#include "queue.h"
#include "pcb.h"

typedef struct multiQueue {
	Queue readyQueues[NUM_QUEUE_LEVELS];
	Queue blockedQueue;

	int count;		// Total count of pcbs in all queues
	int readyCount;		// Number of pcbs in ready queues
	int blockedCount;	// Number of pcbs in blocked queue

} MultiQueue;

void initializeMultiQueue(MultiQueue * multiQ);
void checkBlockedProcesses(MultiQueue * multiQ, Clock * now);
void mEnqueue(MultiQueue * multiQ, ProcessControlBlock * pcb);
ProcessControlBlock * mDequeue(MultiQueue * multiQ, Clock currentTime);
void mAddPreempted(MultiQueue *, ProcessControlBlock *);

#endif
