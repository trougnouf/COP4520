#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 2		// used in handler.c
#define NUMTASKS 50*NUMTHREADS	// used in handler.c and testcase1.c
#define slLEVELS 16		// skiplist.c

// Key values
#define MINKEY 0
#define MAXKEY 100

// Used in test case
#define PERCENTINSERT 90
#define PERCENTFIND 0
#define PERCENTREMOVE 10

#define DBG_PRINTSL 1
#define VERBOSE 3
// debugging
/*	VERBOSE:
		0: Result only
		1: Display each task
		3: spam
		500+: display task# every n tasks
*/

#endif

