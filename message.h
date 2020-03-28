// message.h was created by Mark Renard on 3/27/2020.
//
// This file contains the definition of a struct used to pass messages between
// oss and user processes.

#ifndef MESSAGE_H
#define MESSAGE_H

#include "constants.h"

typedef struct message {
	long int type;
	char string[MSG_SZ];
} Message;

#endif

