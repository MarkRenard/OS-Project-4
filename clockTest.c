#include "clock.h"
#include <stdio.h>
#include "constants.h"

int main(){
	Clock small = {3, BILLION - 1};
	Clock large = {11, BILLION - 1};

	// Prints clocks
	fprintf(stderr, "Small: ");
	printTime(stderr, small);
	fprintf(stderr, "\nLarge: ");
	printTimeln(stderr, large);

	// Tests functions
	fprintf(stderr, "\nclockRatio(small, large): %03.3Lf\n", clockRatio(small, large));
	fprintf(stderr, "clockDiff(large, small): ");
	printTimeln(stderr, clockDiff(large, small));


	return 0;
}
