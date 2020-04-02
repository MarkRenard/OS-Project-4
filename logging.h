// logging.h was created by Mark Renard on 3/29/2020.
//
// This file contains headers for functions that aid in the collection,
// formatting, and logging of data pertinent to Assignment 4.

#ifndef LOGGING_H
#define LOGGING_H

#include "clock.h"

// Logs the generation and enqueueing of a new process
void logGeneration(int simPid, int queueNum, Clock time);

// Logs the dispatching of a process 
void logDispatch(int simPid, int queueNum, Clock time);

// Logs the simulated processor time taken to dispatch a process 
void logDispatchTime(unsigned int time);

// Logs messages recieved from user processes
void logMessageReciept(int simPid, unsigned int burstLength);

// Logs that the process that sent the message didn't use its entire timeslice
void logPartialQuantumUse();

// Logs the enqueueing of a process control block
void logEnqueue(int simPid, int queueNum);

void logBlocking(int simPid, Clock nextIoEventTime);

#endif
