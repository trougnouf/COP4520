#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 8		// used in handler.c
#define NUMTASKS 500000*NUMTHREADS	// used in handler.c and testcase1.c

// Key values
#define MINKEY 0
#define MAXKEY 65536

// Used in test case
#define PERCENTINSERT 70
#define PERCENTFIND 30
#define PERCENTREMOVE 0

#define VERBOSE 0
// debugging
/*	VERBOSE:
		0: Result only
		1: Display each task
		2: spam
		500+: display task# every n tasks
*/

#endif

