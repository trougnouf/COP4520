#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

#define NUMTHREADS 8
#define NUMTASKS 500000*NUMTHREADS
#define slLEVELS 16		// 4 recommended up to MAXKEY=2^30,
				// 16 works better with insufficient removes
#define slMERGEDUPLICATES 0	// 0: detect prior, 1: merge higher (unstable)

// Key values
#define MINKEY 0
#define MAXKEY 65536

// Used in test case
#define PERCENTINSERT 100
#define PERCENTFIND 0
#define PERCENTREMOVE 0

// Debugging
#define MAXCHAOS 50	// # of restart before things happen (reset curNode)
#define DBG_PRINTSL 0	// Print SL. 0: never, 1: end, 2: when chaos happens
#define VERBOSE 0
/*	VERBOSE:
		0: Result only
		1: Display each task
		3: spam
		500+: display task# every n tasks
*/

#endif

