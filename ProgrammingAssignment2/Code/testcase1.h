#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 1			// used in handler.c
#define NUMTASKS 50000*NUMTHREADS	// used in handler.c and testcase1.c
/*
	VERBOSE:
		0: Result only
		1: 0+Display current task
		2: 1+Remaining tasks
*/
#define VERBOSE 0

// Used in test case
#define PERCENTINSERT 100
#define PERCENTFIND 0
#define PERCENTREMOVE 0

#endif
