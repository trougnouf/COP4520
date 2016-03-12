#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

// Used elsewhere
#define NUMTHREADS 8		// used in handler.c
#define NUMTASKS 10*NUMTHREADS	// used in handler.c and testcase1.c
#define slLEVELS 4		// skiplist.c

// Key values
#define MINKEY 0
#define MAXKEY 10

// Used in test case
#define PERCENTINSERT 50
#define PERCENTFIND 50
#define PERCENTREMOVE 0

#define DBG_PRINTSL 1
#define VERBOSE 0
// debugging
/*	VERBOSE:
		0: Result only
		1: Display each task
		3: spam
		500+: display task# every n tasks
*/

#endif

