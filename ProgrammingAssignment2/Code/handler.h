#ifndef HANDLER_H
#define HANDLER_H

#include "skiplist.h"

typedef enum State {FIND, INSERT, REMOVE, IDLE, TERM} State;

// Used to create tests in the test file.
typedef struct
{
	State task;
	int inData;
} Task;

// Thread IO
typedef struct
{
	State curState;
	int ioData;
	slNode * resultNode;
	
	slNode * slHead;
} pthreadData;

void* thread_dsHandler(void* args);

double getTimeElapsed(struct timeval * begTime);

Task * loadTestCase();	// in testcase1.h

void printResults(struct timeval * begTime);
double getTimeElapsed(struct timeval * begTime);

#endif



