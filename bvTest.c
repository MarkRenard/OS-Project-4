// bvTest.c was created by Mark Renard on 3/29/2020.
//
// This file contains a test of the functions in bitVector.c.

#include "bitVector.h"
#include "constants.h"
#include "perrorExit.h"
#include "randomGen.h"

#include <stdlib.h>
#include <stdio.h>

#define NUM_TESTS MAX_VALUE + 20
#define PROB_RESERVE 0.5
#define PROB_FREE 0.5

int getIntFromBoolArray(int []);
void testNumberSelection(int boolArray[]);
void printIntReservations(int boolArray[]);

int main(int argc, char * argv[]){
	unsigned int seed;	
	int boolArray[MAX_VALUE + 1];

	exeName = argv[0];

	// Gets seed from argument vector or exits with an error message
	if (argc < 2 || (seed = (unsigned int)atoi(argv[1])) == 0)
		perrorExit("The first argument must be a non-zero seed");

	srand(seed);

	initializeBitVector();

	// Reserves random integers in boolArray and bitVector
	int i, rand;
	for (i = 0; i <= MAX_VALUE; i++){
		rand = randBinary(PROB_RESERVE);
		boolArray[i] = rand;

		if (rand) reserveInBitVector(i);
	}

	// Tests number reservation and selection
	printIntReservations(boolArray);
	testNumberSelection(boolArray);
	printIntReservations(boolArray);
	
	// Randomly frees reserved ints
	fprintf(stderr, "\nAttempting to free:     ");
	for (i = 0; i <= MAX_VALUE; i++){
		rand = randBinary(PROB_FREE);

		if (rand){
			fprintf(stderr, "% 3d", i);
			freeInBitVector(i);
			boolArray[i] = 0;
		}
	}

	// Tests number reservationa and selection again
	printIntReservations(boolArray);
	testNumberSelection(boolArray);
	printIntReservations(boolArray);

	return 0;
}

// Returns the next unused integer using a bool array
int getIntFromBoolArray(int boolArray[]){
	static int candidate = 0;	// Used to generate ints
	int numChecked = 0;		// The number of integers checked

	// Finds an unused integer
	while(boolArray[candidate]){
		candidate++;

		// Wraps to 0 if MAX_VALUE has been reached
		if (candidate > MAX_VALUE) candidate = 0;

		// Returns -1 if all possible values have been checked
		if (++numChecked > MAX_VALUE + 1) return -1;
	}

	boolArray[candidate] = 1;

	return candidate;

}

// Tests number selection from bool array and bit vector
void testNumberSelection(int boolArray[]){
	int bvInt, boolInt, i;
	printf("\n\nTesting number selection from boolArray and bitVector\n");
	for (i = 0; i <= NUM_TESTS; i++){
		bvInt = getIntFromBitVector();
		boolInt = getIntFromBoolArray(boolArray);

		printf("% 3d, % 3d\n", bvInt, boolInt);
		if (bvInt != boolInt) printf("MISMATCH!\n\n");
	}
	printf("\n");
}


// Prints reserved and non-reserved ints in boolArray and bitVector
void printIntReservations(int boolArray[]){
	int i;

	// Prints reserved ints in boolArray and bitVector
	printf("\nShould be reserved: ");
	for (i = 0; i <= MAX_VALUE; i++){
		if (boolArray[i]) printf("% 3d", i);
	}

	printf("\nAre reserved:       ");
	for (i = 0; i <= MAX_VALUE; i++){
		if (isReservedInBitVector(i)) printf("% 3d", i);
	}

	// Prints unreserved ints in boolArray and bitVector
	printf("\nShould NOT be reserved: ");
	for (i = 0; i <= MAX_VALUE; i++){
		if (!boolArray[i]) printf("% 3d", i);
	}

	printf("\nAre NOT reserved:       ");
	for (i = 0; i <= MAX_VALUE; i++){
		if (!isReservedInBitVector(i)) printf("% 3d", i);
	}

	printf("\n\n");
}
