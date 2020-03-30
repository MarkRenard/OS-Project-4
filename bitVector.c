// bitVector.c was created by Mark Renard on 3/29/2020
//
// This file contains implementations of functions that manipulate a bit
// vector to track which of a set of integers has been used.
//
// Note that the bit vector will be initialized the first time getUnusedInt
// is called. BIT_VECTOR_SIZE and NUM_BITS should be defined in constants.h

#include "constants.h"

#ifdef DEBUG_BV
#include <stdio.h>
#endif

static unsigned int bitVector[BIT_VECTOR_SIZE];

void initializeBitVector(){
        int i;
        for (i = 0; i < BIT_VECTOR_SIZE; i++){
                bitVector[i] = 0;
        }
}

unsigned int isReservedInBitVector(int num){
	if (num > MAX_VALUE) return 0U;
	
	unsigned int result = bitVector[num / NUM_BITS] & 1U << (num % NUM_BITS);
#ifdef DEBUG_BV
	fprintf(stderr, "%d, ", num);
#endif
	return result; }

void reserveInBitVector(int num){
	bitVector[num / NUM_BITS] = \
		bitVector[num / NUM_BITS] | 1U << (num % NUM_BITS);
}

void freeInBitVector(int num){
	bitVector[num / NUM_BITS] = \
		bitVector[num / NUM_BITS] & ~(1U << (num % NUM_BITS));
}

// Returns the next unused
int getIntFromBitVector(){
	static int candidate = 0; 	// Used to generate ints
	int numChecked = 0;		// The number of integers checked

#ifdef DEBUG_BV
	fprintf(stderr, "Checking ints: ");
#endif
	// Finds an unused integer
	while (isReservedInBitVector(candidate)){
		candidate++;

		// Wraps to 0 if MAX_VALUE exceeded
		if (candidate > MAX_VALUE) candidate = 0;

		// Returns -1 if all possible values have been checked
		if (++numChecked > MAX_VALUE + 1){
#ifdef DEBUG_BV
			fprintf(stderr, "\n\n");
#endif
			 return -1;
		}
	}

	reserveInBitVector(candidate);
	
#ifdef DEBUG_BV
	fprintf(stderr, "\n\n");
#endif	
	return candidate;		
}

