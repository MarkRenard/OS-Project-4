// multiQueue.c was created by Mark Renard on 3/30/2020.
//
// These functions facillitate the enqueueing and dequeueing of simulated 
// process control blocks into a multi-level feedback queue.

#include "multiQueue.h"
#include "queue.h"
#include "pcb.h"

const Clock WAIT_TIME_THRESHOLD = {PROMOTION_WAIT_TIME_THRESHOLD_SEC,
				   PROMOTION_WAIT_TIME_THRESHOLD_NS};


void initializeMultiQueue(MultiQueue * multiQ){
	int i;
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		initializeQueue(&multiQ->readyQueues[i]);
	}

	initializeQueue(&multiQ->blockedQueue);

	multiQ->count = 0;
}

void printMultiQueue(FILE * fp, MultiQueue * mQueue){
	int i;
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		fprintf(fp, "(%02d)Queue %d: ", 
			mQueue->readyQueues[i].count, i);

		printQueue(fp, &mQueue->readyQueues[i]);
		fprintf(fp, "\n");
	}

	fprintf(fp, "(%02d)Blocked: ", mQueue->readyQueues[i].count);
	printQueue(fp, &mQueue->blockedQueue);
	fprintf(fp, "\n\n");
}

void mEnqueue(MultiQueue * multiQ, ProcessControlBlock * pcb){
#ifdef DEBUG_Q
	fprintf(stderr, "\nCount: %d\n", multiQ->count);
	printMultiQueue(stderr, multiQ);
#endif
	// Adds to highest priority queue if new or real time
	if (pcb->schedulingClass == REAL_TIME || pcb->state == NEW){
		pcb->priority = 0;
		
	// Adds to blocked queue and resets priority if blocked
	} else if (pcb->state == BLOCKED) {
#ifdef DEBUG_Q
		fprintf(stderr, "Enqueueing on blocked queue\n");
#endif
		pcb->priority = 0;
		enqueue(&multiQ->blockedQueue, pcb); 
		multiQ->count++;
		return;

	// Otherwise, lowers priority if not at minimum
	} else if (pcb->priority < NUM_QUEUE_LEVELS - 1){
		pcb->priority++;
	}

#ifdef DEBUG_Q
	fprintf(stderr, "Enqueueing on readyQueue %d\n", pcb->priority);
#endif

	// Enqueues pcb
	enqueue(&multiQ->readyQueues[pcb->priority], pcb);
	multiQ->count++;
}

// Promotes processes in danger of starvation
static void promoteSufficientlyAgedProcesses(MultiQueue * multiQ, Clock now){
	ProcessControlBlock * pcb;
	Clock processWaitTime;
	double cpuUtilization;

	int i;
	for (i = 1; i < NUM_QUEUE_LEVELS; i++){
		pcb = multiQ->readyQueues[i].front; // Selects next queue head

		// Skips empty queues
		if (pcb == NULL) continue;
	
		// Computes process wait time
		processWaitTime = clockDiff(now, pcbTimeLastExecuting(pcb));

		// Computes cpu utilization
		cpuUtilization = pcbCpuUtilization(pcb, now); 

		// Determines whether to promote process
		if (cpuUtilization < UTIL_THRESHOLD \
		    &&(clockCompare(processWaitTime, WAIT_TIME_THRESHOLD) >=0)){
		
			// Increases process priority
			pcb->priority--;

			// Moves process control block to the next queue
			dequeue(&multiQ->readyQueues[i]);
			enqueue(&multiQ->readyQueues[i - 1], pcb);
		}				

	}
}

// Promotes aged processes and returns the PCB of the next process to schedule
ProcessControlBlock * mDequeue(MultiQueue * multiQ, Clock currentTime){
	promoteSufficientlyAgedProcesses(multiQ, currentTime);
#ifdef DEBUG_Q
	fprintf(stderr, "\nCount: %d\n", multiQ->count);
	printMultiQueue(stderr, multiQ);
#endif
	// Finds the index of the highest priority non-empty queue
	int i;
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		if (multiQ->readyQueues[i].count > 0) break;
	}

#ifdef DEBUG_Q
	fprintf(stderr, "Dequeueing from readyQueue %d\n", i);
#endif
	// Dequeues and returns process from the chosen queue
	multiQ->count--;
	return dequeue(&multiQ->readyQueues[i]); 
}



