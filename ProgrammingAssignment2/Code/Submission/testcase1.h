#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 4		// used in handler.c
#define NUMTASKS 500000*NUMTHREADS	// used in handler.c and testcase1.c


// Used in test case
#define PERCENTINSERT 10
#define PERCENTFIND 10
#define PERCENTREMOVE 80

#define VERBOSE 0
// debugging
/*	VERBOSE:
		0: Result only
		1: Display each task
		2: spam
		500+: display task# every n tasks
*/

#endif

