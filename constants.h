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
#define QUANTUM_FACTOR 2		// Divisor of base quantum per level
#define NUM_QUEUE_LEVELS 4		// Number of levels in multi-level queue

#define MAX_SIMULTANEOUS_PROCESSES 20	// Max number of processes in the system
#define MAX_SIMULTANEOUS_USER_PROCESSES MAX_SIMULTANEOUS_PROCESSES - 2
#define MAX_TOTAL_USER_PROCESSES 100	// Max children launched by oss
#define MAX_REAL_LIFE_SECONDS 3		// Max total execution time of oss


// Used by userProgram.c
#define SCHED_TERM_PROBABILITY 0.2	// Chance of terminating within quantum
#define BLOCK_PROBABILITY 0.1		// Chance of blocking within quantum


// Used by both
#define MESSAGE_QUEUE_NAME "/mq"	// The name of the message queue


// Used by miscelaneous functions
#define BILLION 1000000000U		// The number of nanoseconds in a second
#define BUFF_SZ 100			// The size of character buffers 

#endif
