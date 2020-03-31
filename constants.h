// constants.h was created by Mark Renard on 3/26/2020
//
// This file contains definitions of constants used in assignment 4 of the
// spring 2020 semester of 4760, grouped according to the source files in which
// they are used and otherwise appearing in the order in which they are
// prescribed in the assignment document.

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <sys/msg.h>
#include <sys/stat.h>


// Used by oss.c
#define MAX_SIMUL_PROCS 20 		// Max number of processes in the system
#define MAX_BLOCKS (MAX_SIMUL_PROCS - 2)// Max simultaneous children

#define maxTimeBetweenNewProcsNS 0U	// Value of nanoseconds in max interval
#define maxTimeBetweenNewProcsSecs 2U	// Value of seconds in max interval

#define USER_PROG_PATH "./userProgram"	// The path to the user program

#define REAL_TIME_PROBABILITY 0.05	// Probability new process is real time

#define LOOP_INCREMENT_SECONDS 1	// Seconds incremented each iterations
#define MIN_LOOP_INCREMENT_NS 0		// Min nanosecond increment
#define MAX_LOOP_INCREMENT_NS 1000	// Max nanosecond increment

#define NUM_QUEUE_LEVELS 4		// Number of levels in multi-level queue
#define BASE_QUANTUM 10000000		// Base time quantum in nanoseconds
#define QUANTUM_FACTOR 2		// Divisor of base quantum per level

#define MIN_SCHEDULING_TIME_NS 100	// Min nanoseconds to schedule a process
#define MAX_SCHEDULING_TIME_NS 1000	// Max nanoseconds to schedule a process

#define LOG_FILE_NAME "oss_log"		// The name of the output file

#define MAX_TOTAL_GENERATED 100		// Max children launched by oss
#define MAX_SECONDS 99999	 		// Max total execution time of oss


// Used by userProgram.c
#define TERMINATION_PROBABILITY 0.3	// Chance of terminating within quantum
#define BLOCK_PROBABILITY 0.1		// Chance of blocking within quantum

#define MIN_WAIT_SEC 0			// Min seconds waiting if blocked
#define MIN_WAIT_MS 0			// Min milliseconds waiting if blocked
#define MAX_WAIT_SEC 3			// Max seconds waiting if blocked
#define MAX_WAIT_MS 1000		// Max milliseconds waiting if blocked


// Used by both oss.c and userProgram.c
#define DISPATCH_MQ_KEY 59597192	// Message queue key for dispatch
#define INTERRUPT_MQ_KEY 38257848	// Message queue key for interrupts
#define MQ_PERMS (S_IRUSR | S_IWUSR)	// Message queue permissions

#define BASE_SEED 8853984		// Used in calls to srand


// Used by bitVector.c
#define NUM_BITS (sizeof(unsigned int) * 8)  // Bits per unsigned int
#define MAX_VALUE (MAX_BLOCKS - 1) 	   // Max int tracked in bit vector
#define BIT_VECTOR_SIZE (MAX_VALUE / NUM_BITS + 1) // Size of bit vector


// Miscelaneous 
#define BILLION 1000000000U		// The number of nanoseconds in a second
#define BUFF_SZ 100			// The size of character buffers 
#define MSG_SZ 30			// Size of Message char arrays

#define MAX_LOG_LINES 10000		// Max number of lines in the log file


// Editable from makefile
#ifdef SEED
#define BASE_SEED SEED
#endif

#endif
