// queue.c was created by Mark Renard on 2/5/2020 and modified on 3/26/2020.
// This file defines functions that operate on a queue of process control
// blocks.

#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include <stdio.h>

void initializeQueue(Queue * qPtr){
	qPtr->front = NULL;
	qPtr->back = NULL;
	qPtr->count = 0;
}

// Prints the simPid of process control blocks in the queue from front to back
void printQueue(FILE * fp, const Queue * q){
	ProcessControlBlock * pcb = q->front;

	while (pcb != NULL){
		fprintf(fp, " %02d", pcb->simPid);
		pcb = pcb->previous;
	}
}

// Adds a process control block to the back of the queue
void enqueue(ProcessControlBlock * pcb, Queue * q){
#ifdef DEBUG_Q
	fprintf(stderr, "\tenqueue(%02d): ", pcb->simPid);
	printQueue(stderr, q);
	fprintf(stderr, " <- %02d\n", pcb->simPid);
#endif
	// Adds process control block to queue	
	if (q->back != NULL){
		// Adds to previous pcb of back if queue is not empty
		q->back->previous = pcb;
	} else {
		// Adds to front if queue is empty
		q->front = pcb;
	}
	q->back = pcb;

	// Back of queue shouldn't have a previous element
	q->back->previous = NULL;

	// Increments node count in queue
	q->count++;

}

// Removes and returns ProcessControlBlock reference from the front of the queue
ProcessControlBlock * dequeue(Queue * q){

	// Returns null if the queue is already empty
	if (q->front == NULL) return NULL;

	// Assigns current front of queue to returnVal
	ProcessControlBlock * returnVal = q->front;  	
	
	// Removes the front node from the queue
	q->front = q->front->previous; // Assigns new front of queue
	q->count--; // Decrements queue node count
	returnVal->previous = NULL; // Removes previous from dequeued block
		
	// Sets queue back to null if empty
	if(q->front == NULL) q->back = NULL;
	
#ifdef DEBUG_Q
	fprintf(stderr, "\tdequeue(): %02d <-", returnVal->simPid);
	printQueue(stderr, q);
	fprintf(stderr, "\n");
#endif
	return returnVal;


}

