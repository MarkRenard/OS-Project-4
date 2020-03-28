OSS        = oss
OSS_OBJ    = oss.o queue.o $(COMMON_O)
OSS_H      = queue.h $(COMMON_H)

USER_PROG     = userProgram
USER_PROG_OBJ = userProgram.o $(COMMON_O)
USER_PROG_H   = $(COMMON_H)

COMMON_O  = clock.o perrorExit.o randomGen.o sharedMemory.o \
	    getSharedMemoryPointers.o pcb.o
COMMON_H  = clock.h perrorExit.h randomGen.h sharedMemory.h \
	    getSharedMemoryPointers.h pcb.h constants.h
OUTPUT     = $(OSS) $(USER_PROG) 
OUTPUT_OBJ = $(OSS_OBJ) $(USER_PROG_OBJ)
CC         = gcc
FLAGS      = -Wall -g -lm 
DEBUG	   = -DDEBUG


.SUFFIXES: .c .o

all: $(OUTPUT)

$(OSS): $(OSS_OBJ) $(OSS_H)
	$(CC) $(FLAGS) -o $@ $(OSS_OBJ)

$(USER_PROG): $(USER_PROG_OBJ) $(USER_PROG_H)
	$(CC) $(FLAGS) -o $@ $(USER_PROG_OBJ)

.c.o:
	$(CC) $(FLAGS) $(DEBUG) -c $<

.PHONY: clean rmfiles cleanall
clean:
	/bin/rm -f $(OUTPUT) $(OUTPUT_OBJ)
rmfiles:
	/bin/rm -f oss_log 
cleanall:
	/bin/rm -f oss_log $(OUTPUT) $(OUTPUT_OBJ)

