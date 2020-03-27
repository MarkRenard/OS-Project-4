// a4constants.h was created by Mark Renard on 3/26/2020
//
// This file contains definitions of constants used in assignment 4 of the
// spring 2020 semester of 4760

#ifndef CONSTANTS_H
#define CONSTANTS_H


// Used by oss.c
#define maxTimeBetweenNewProcsNS 0	// Value of nanoseconds in max interval
#define maxTimeBetweenNewProcsSecs 2	// Value of seconds in max interval

#define MAX_SCHEDULING_OVERHEAD 1000	// Max number of nanoseconds to schedule
#define DELAY_SECONDS 1			// Seconds between each iteration

#define REAL_TIME_PROBABILITY 0.05	// Probability new process is real time

#define BASE_QUANTUM 10000000		// Base time quantum in nanoseconds
#define NUM_QUEUE_LEVELS 4		// Number of levels in multi-level queue

#define MAX_TOTAL_PROCESSES 20		// Max number of processes in the system
#define MAX_USER_PROCESSES MAX_TOTAL_PROCESSES - 2	// Max children of oss


// Used by userProgram.c
#define SCHED_TERM_PROBABILITY 0.2	// Chance of terminating within quantum


// Used by miscelaneous functions
#define BILLION 1000000000U		// The number of nanoseconds in a second

#define BUFF_SZ 100			// The size of character buffers 

#endif
