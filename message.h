// message.h was created by Mark Renard on 3/27/2020.
//
// This file contains the definition of a struct used to pass messages between
// oss and user processes and headers for message queue utility functions.

#ifndef MESSAGE_H
#define MESSAGE_H

#include "constants.h"

typedef struct message {
	long int type;
	char str[MSG_SZ];
} Message;

int getMessageQueue(int key, int flags);
void sendMessage(int msgQueueId, const char * msgText, long int type);
void waitForMessage(int msgQueueId, char * msgText, long int type);
void removeMessageQueue(int msgQueueId);

#endif

