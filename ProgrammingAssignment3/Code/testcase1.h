#ifndef TEST_H
#define TEST_H

/*
Change these values to generate a different test case.
*/

#define NUMTHREADS 8
#define NUMTASKS 500000*NUMTHREADS
// Skiplist-specific:
#define slLEVELS 16		// slLEVELS: 1-128 (default: 4, sane: 4-16)
				// 4 recommended up to MAXKEY=2^30,
				// 16 is faster with insufficient removes
#define slMERGEDUPLICATES 0	// slMERGEDUPLICATES: 0/1 (default: 0)
				// 0: detect duplicates prior to insertion
				// 1: start inserting right away, merge
				//    duplicates if they come up after the 
				//    insertion has begun. Slower insertion, 
				//    faster retrieval of frequently used nodes.
				//    (unstable)

// Key values
#define MINKEY 0
#define MAXKEY 65536	// 2 - 2^32 (default: 65536)

// Used in test case
#define PERCENTINSERT 33
#define PERCENTFIND 34
#define PERCENTREMOVE 33	// Is this really necessary?

// Debugging
#define MAXCHAOS 10	// # of restart before things happen (reset curNode, declare hopelesness, give up)
#define DBG_PRINTSL 0	// Print SL. 0: never, 1: END, 2: when chaos happens

#define DBG_WARNINGS 0	// 0-3. Complete failures are always announced
#define VERBOSE 0
/*	VERBOSE:
		0: Result only
		1: Display each task
		3: spam
		500+: display task# every n tasks
*/

#endif

