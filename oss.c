// oss.c was created by Mark Renard on 3/26/2020.
//
// This file contains a program which simulates the scheduling of an operating
// system.

#include <stdio.h>

#include "constants.h"
#include "clock.h"
#include "pcb.h"
#include "queue.h"
#include "randomGen.h"

void testClock(FILE *);

// Tests functions
int main(int argc, char * argv[]){
	testClock(stdout);

	return 0;
}

void testClock(FILE * out){
	Clock clock = zeroClock();
	Clock minTime = {0, 0};
	Clock maxTime = {maxTimeBetweenNewProcsSecs, maxTimeBetweenNewProcsNS};

	printTimeln(out, clock);

	// Prints a series of random times
	fprintf(out, "Series of random time increments:\n");
	int i;
	for (i = 0; i < 20; i++){
		incrementClock(&clock, randomTime(minTime, maxTime));
		printTime(out, clock);
		fprintf(out, "\n");
	}
	fprintf(out, "\n");

	// Prints time after testing copyTime
	copyTime(&clock, maxTime);
	fprintf(out, "Max time: ");
	printTimeln(out, clock);

}

