OSS	= oss
OSS_OBJ	= oss.o queue.o bitVector.o $(COMMON_O)
OSS_H	= queue.h bitVector.h $(COMMON_H)

USER_PROG	= userProgram
USER_PROG_OBJ	= userProgram.o $(COMMON_O)
USER_PROG_H	= $(COMMON_H)

BV_TEST		= bvTest
BV_TEST_OBJ	= bvTest.o bitVector.o perrorExit.o randomGen.o
BV_TEST_H	= bitVector.h constants.h perrorExit.h randomGen.h

COMMON_O  = clock.o perrorExit.o randomGen.o sharedMemory.o \
	    getSharedMemoryPointers.o pcb.o message.o
COMMON_H  = clock.h perrorExit.h randomGen.h sharedMemory.h \
	    getSharedMemoryPointers.h pcb.h message.h constants.h

OUTPUT     = $(OSS) $(USER_PROG) 
OUTPUT_OBJ = $(OSS_OBJ) $(USER_PROG_OBJ)
CC         = gcc
FLAGS      = -Wall -g -lm 
DEBUG	   = -DDEBUG -DDEBUG_BV

PRIVATIZE  = ; chmod o= *; chmod g= *

.SUFFIXES: .c .o

all: $(OUTPUT)

$(OSS): $(OSS_OBJ) $(OSS_H)
	$(CC) $(FLAGS) -o $@ $(OSS_OBJ) $(PRIVATIZE)

$(USER_PROG): $(USER_PROG_OBJ) $(USER_PROG_H)
	$(CC) $(FLAGS) -o $@ $(USER_PROG_OBJ) $(PRIVATIZE)

$(BV_TEST): $(BV_TEST_OBJ) $(BV_TEST_H)
	$(CC) $(FLAGS) -o $@ $(BV_TEST_OBJ) $(PRIVATIZE)

.c.o:
	$(CC) $(FLAGS) $(DEBUG) -c $<

.PHONY: clean rmfiles cleanall
clean:
	/bin/rm -f $(OUTPUT) $(OUTPUT_OBJ)
cleantest:
	/bin/rm -f $(BV_TEST) $(BV_TEST).o
rmfiles:
	/bin/rm -f oss_log 
cleanall:
	/bin/rm -f oss_log $(OUTPUT) $(OUTPUT_OBJ) $(BV_TEST) $(BV_TEST_OBJ)


