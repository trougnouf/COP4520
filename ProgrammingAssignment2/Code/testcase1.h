#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 4		// used in handler.c
#define NUMTASKS 20000*NUMTHREADS	// used in handler.c and testcase1.c


// Used in test case
#define PERCENTINSERT 100
#define PERCENTFIND 0
#define PERCENTREMOVE 0

#endif

// debugging
/*	VERBOSE:
		0: Result only
		1: Display each task
		2: spam
*/
#define VERBOSE 0
