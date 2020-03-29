// message.c was created by Mark Renard on 3/28/2020.
//
// This file contains implementations of utility functions which aid in the
// use of a message queue to send and recieve messages.

#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>

#include "message.h"
#include "perrorExit.h"

#define MSG_TYPE 1

// Returns the message queue id of a new message queue
int getMessageQueue(int key, int flags){
	int msgQueueId;
	if ((msgQueueId = msgget(key, flags)) == -1)
        	perrorExit("Failed to create message queue");

	return msgQueueId;
}

// Adds a message to the message queue with the specified message queue id
void sendMessage(const char * msgText, int msgQueueId){
	Message msg;	// Buffer for the message to be sent

	// Initializes message
	msg.type = MSG_TYPE;
	strcpy(msg.str, msgText);
	
	// Sends message
	if ((msgsnd(msgQueueId, (const void *)&msg, sizeof(msg.str), 0)) == -1)
		perrorExit("Couldn't send message");
}

// Blocks until a message is recieved in the message queue with specified id
void waitForMessage(char * msgText, int msgQueueId){
	Message msg;	// Buffer for message to be received

	// Waits for message
	if ((msgrcv(msgQueueId, (void *)&msg, \
		sizeof(msg.str), MSG_TYPE, 0)) == -1)
			perrorExit("Error waiting for message");

	// Copies message text
	strcpy(msgText, msg.str);
}

// Removes the message queue with the specified id
void removeMessageQueue(int msgQueueId){
	if ((msgctl(msgQueueId, IPC_RMID, NULL)) == -1)
		perrorExit("Error removing message queue");
}
