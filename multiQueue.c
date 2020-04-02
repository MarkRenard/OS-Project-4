// multiQueue.c was created by Mark Renard on 3/30/2020.
//
// These functions facillitate the enqueueing and dequeueing of simulated 
// process control blocks in a multi-level feedback queue.

#include "multiQueue.h"
#include "queue.h"
#include "pcb.h"
#include "perrorExit.h"
#include "logging.h"

#include <unistd.h>

// Used for determining whether a process priority should be promoted due to age
const Clock WAIT_TIME_THRESHOLD = {PROMOTION_WAIT_TIME_THRESHOLD_SEC,
				   PROMOTION_WAIT_TIME_THRESHOLD_NS};

// Used to increment simulated system clock while waking up blocked processes
const Clock WAKE_UP_INCREMENT = {WAKE_UP_INCREMENT_SEC,
				 WAKE_UP_INCREMENT_NS};

// Initializes values in a MultiQueue, including those of constituent queues
void initializeMultiQueue(MultiQueue * multiQ){
	int i;
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		initializeQueue(&multiQ->readyQueues[i]);
	}

	initializeQueue(&multiQ->blockedQueue);

	multiQ->count = 0;
	multiQ->readyCount = 0;
	multiQ->blockedCount = 0;
}

// Prints a representation of a MultiQueue to the file
void printMultiQueue(FILE * fp, MultiQueue * mQueue){
	int i;

	// Prints ready queues
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		fprintf(fp, "(%02d)Queue %d: ", 
			mQueue->readyQueues[i].count, i);

		printQueue(fp, &mQueue->readyQueues[i]);
		fprintf(fp, "\n");
	}

	// Prints blocked queue
	fprintf(fp, "(%02d)Blocked: ", mQueue->readyQueues[i].count);
	printQueue(fp, &mQueue->blockedQueue);
	fprintf(fp, "\n\n");
}

// Adds pcb to blocked queue, keeping it sorted by time of next I/O event
void mAddToBlockedQueue(Queue * blockedQueue, ProcessControlBlock * pcb){

	// Enqueues as normal if queue is empty
	if (blockedQueue->count == 0){
		enqueue(blockedQueue, pcb);

	// Adds to front if interupt time is earliest
	} else if (clockCompare(pcb->nextIoEventTime,
				blockedQueue->front->nextIoEventTime) < 0){
		addToFront(blockedQueue, pcb);

	// Adds to position that maintains sorting by time of next I/O event
	} else {
		ProcessControlBlock * previous = blockedQueue->front->previous;
		ProcessControlBlock * next = blockedQueue->front;	

		// Traverses queue while previous has same or earlier I/O event
		while (previous != NULL \
		       && clockCompare(previous->nextIoEventTime,
				       pcb->nextIoEventTime) <= 0){
			next = previous;
			previous = previous->previous;
		}

		// Inserts pcb into blocked queue
		pcb->previous = previous;
		next->previous = pcb;
		blockedQueue->count++;
	}
		
}

// Re-enqueues blocked processes if I/O happened, returns simulated overhead
void checkBlockedProcesses(MultiQueue * multiQ, Clock * now){
	struct processControlBlock * pcb; 
	
	// Traverses blocked queue until the end or I/O event is in the future
	while((pcb = multiQ->blockedQueue.front) != NULL
	      && clockCompare(pcb->nextIoEventTime, *now) <= 0){
		
	
		// Wakes up and adds to appropriate queue
		pcb = dequeue(&multiQ->blockedQueue);
		pcb->state = READY;
		enqueue(&multiQ->readyQueues[pcb->priority], pcb);

		// Updates counts
		multiQ->blockedCount--;
		multiQ->readyCount++;

		// Adds simulated time
		incrementClock(now, WAKE_UP_INCREMENT);

		// Logs wake-up
		logWakeUp(pcb->simPid, pcb->priority, *now);
	}
}

// Adds pcb to a blocked or ready queue, changing its priority if applicable
void mEnqueue(MultiQueue * multiQ, ProcessControlBlock * pcb){
#ifdef DEBUG_Q
	fprintf(stderr, "\nCount: %d\n", multiQ->count);
	printMultiQueue(stderr, multiQ);
#endif
	// Adds to blocked queue if blocked
	if (pcb->state == BLOCKED) {
#ifdef DEBUG_Q
		fprintf(stderr, "Enqueueing on blocked queue\n");
#endif
		mAddToBlockedQueue(&multiQ->blockedQueue, pcb); 
		multiQ->count++;
		multiQ->blockedCount++;
		return;

	// Otherwise, lowers priority if not minimum and not real time or new
	} else if (pcb->priority < NUM_QUEUE_LEVELS - 1
		   && pcb->schedulingClass != REAL_TIME
		   && pcb->state != NEW){
		pcb->priority++;
	}

#ifdef DEBUG_Q
	fprintf(stderr, "Enqueueing on readyQueue %d\n", pcb->priority);
#endif

	// Enqueues pcb
	enqueue(&multiQ->readyQueues[pcb->priority], pcb);
	multiQ->count++;
	multiQ->readyCount++;
}

// Promotes aged processe to higher priority queues to prevent starvation
static void promoteSufficientlyAgedProcesses(MultiQueue * multiQ, Clock now){
	ProcessControlBlock * pcb;
	Clock processWaitTime;
	double cpuUtilization;

	int i;
	for (i = 2; i < NUM_QUEUE_LEVELS; i++){
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
	// promoteSufficientlyAgedProcesses(multiQ, currentTime);
#ifdef DEBUG_Q
	fprintf(stderr, "\nCount: %d\n", multiQ->count);
	printMultiQueue(stderr, multiQ);
#endif
	// Finds the index of the highest priority non-empty queue
	int i;
	for (i = 0; i < NUM_QUEUE_LEVELS; i++){
		if (multiQ->readyQueues[i].count > 0) break;
	}

	// Error if all ready queues are empty
	if (i == NUM_QUEUE_LEVELS) 
		perrorExit("Called mDequeue with no process in ready queue");

#ifdef DEBUG_Q
	fprintf(stderr, "Dequeueing from readyQueue %d\n", i);
#endif
	// Dequeues and returns process from the chosen queue
	multiQ->count--;
	multiQ->readyCount--;
	return dequeue(&multiQ->readyQueues[i]); 
}

// Adds process to head of its queue
void mAddPreempted(MultiQueue * multiQ, ProcessControlBlock * pcb){
	// Returns to the head of the queue
	addToFront(&multiQ->readyQueues[pcb->priority], pcb);
	pcb->state = READY;

	// Updates counts
	multiQ->count++;
	multiQ->readyCount++;

	
#ifdef DEBUG_Q
	fprintf(stderr, "mAddPreempted - adding to front\n");
	printMultiQueue(stderr, multiQ);
#endif
}		

