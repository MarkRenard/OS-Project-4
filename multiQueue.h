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

	int count;	

} MultiQueue;

void initializeMultiQueue(MultiQueue * multiQ);
void mEnqueue(MultiQueue * multiQ, ProcessControlBlock * pcb);
ProcessControlBlock * mDequeue(MultiQueue * multiQ, Clock currentTime);

#endif
