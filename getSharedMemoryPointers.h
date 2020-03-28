// getSharedMemoryPointers.h was created by Mark Renard on 3/27/2020.
//
// This file contains a header for the function getSharedMemoryPointers to be
// used in assignment 4.

#ifndef GETSHAREDMEMORYPOINTERS_H
#define GETSHAREDMEMORYPOINTERS_H

#include "pcb.h"
#include "clock.h"

void getSharedMemoryPointers(char ** shm,  Clock ** systemClock,
                             ProcessControlBlock ** processTable, int flags);

#endif
