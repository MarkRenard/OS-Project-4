// clock.c was created by Mark Renard on 2/21/2020
//
// This file contains an implementation of the function incrementClock which
// adds some number of nanoseconds to the time recorded in a virtual clock.

#include "clock.h"
#include "randomGen.h"
#include <stdio.h>

#define BILLION 1000000000
#define FORMAT "%03d : %09d"

// Returns a clock initialized to zero seconds, zero nanoseconds
Clock zeroClock(){
	Clock clock = {0, 0};
	return clock;
}

Clock newClock(unsigned int seconds, unsigned int nanoseconds){
	Clock clock = {seconds, nanoseconds};
	return clock;
}

// Returns a clock with a randomly selected time in the specified range
Clock randomTime(const Clock min, const Clock max){
	Clock rTime; // The random time to be returned

	// Sets the number of seconds
	rTime.seconds = min.seconds == max.seconds ? \
		min.seconds : randUnsigned(min.seconds, max.seconds);

	// Sets nanoseconds in the case where min and max seconds is equal
	if (min.seconds == max.seconds){
		rTime.nanoseconds = randUnsigned(
					min.nanoseconds, max.nanoseconds
				    );

	// Sets nanoseconds if the minimum number of seconds was selected
	} else if (rTime.seconds == min.seconds){
		rTime.nanoseconds = randUnsigned(min.nanoseconds, BILLION - 1);

	// Sets nanoseconds if the maximum number of seconds was selected
	} else if (rTime.seconds == max.seconds){
		rTime.nanoseconds = randUnsigned(0, max.nanoseconds);

	// Sets nanoseconds to [0, 999999999] otherwise
	} else {
		rTime.nanoseconds = randUnsigned(0, BILLION - 1);
	}

	return rTime;
}

// Copies the time stored in the second clock to the first
void copyTime(Clock * dest, const Clock src){
	dest->seconds = src.seconds;
	dest->nanoseconds = src.nanoseconds;
}

// Adds a time increment defined in a clock structure to a clock
void incrementClock(Clock * clock, const Clock increment){	
	clock->seconds += increment.seconds;
	clock->nanoseconds += increment.nanoseconds;

	// Carry operations
	clock->seconds += clock->nanoseconds / BILLION;
	clock->nanoseconds = clock->nanoseconds % BILLION;
}

// Returns -1 if the time on clk1 is less, 1 if it's greater, and 0 if equal
int clockCompare(const Clock clk1, const Clock clk2){
	if (clk1.seconds != clk2.seconds){
		// Compares seconds if they are not equal
		if (clk1.seconds < clk2.seconds) return -1;
		if (clk1.seconds > clk2.seconds) return 1;
	} else {
		// Compares nanoseconds if seconds are equal
		if (clk1.nanoseconds < clk2.nanoseconds) return -1;
		if (clk1.nanoseconds > clk2.nanoseconds) return 1;
	}

	// If this statement executes, the two times are equal.
	return 0;
}

// Formats and prints the time on the clock to the file
void printTime(FILE * fp, const Clock clock){
	fprintf(fp,
		FORMAT,
		clock.seconds,
		clock.nanoseconds
	);
}

// Formats and prints the time on the clock to the file and a new line char
void printTimeln(FILE * fp, const Clock clock){
	printTime(fp, clock);
	fprintf(fp, "\n");
}
