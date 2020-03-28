// queue.h was created by Mark Renard on 2/5/2020
// This file defines function prototypes for a string queue structure

#ifndef QUEUE_H
#define QUEUE_H

#include "pcb.h" // Inlcudes definition of ProcessControlBlock

typedef struct Queue {
	ProcessControlBlock * back;
	ProcessControlBlock * front;
	int count;
} Queue;

void initializeQueue(Queue *);
void enqueue(ProcessControlBlock *, Queue *);
ProcessControlBlock * dequeue(Queue *);

#endif

